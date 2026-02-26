# 实验数据保存流程

## 概述

本文档描述了 FireApp 中创建实验、开启实验数据记录以及实验数据异步保存到 SQLite 数据库的完整流程。

---

## 1. 创建实验

### 1.1 用户操作入口

用户点击侧边栏 (`FuSideTabBar`) 上的 **"新建实验"** 按钮，触发 `newExperimentRequested` 信号。

**关键代码路径：**

- `FuSideTabBar.cpp` — `mNewExperimentButton` 的点击事件连接到 `newExperimentRequested` 信号
- `FuMainWid.cpp` — `setupConnections()` 中监听该信号

### 1.2 弹出实验信息对话框

`FuMainWid::setupConnections()` 接收到 `newExperimentRequested` 信号后，弹出 `ExpInfoDialog` 对话框：

```
FuSideTabBar::newExperimentRequested
    → FuMainWid::setupConnections() lambda
        → ExpInfoDialog dialog(this)
```

- 对话框默认使用当前时间 (`yyyyMMdd_HHmmss`) 作为实验名称
- 用户可以修改名称，点击"确认"按钮提交
- 提交时检查实验名称是否已存在（通过 `ExperimentParamManager::experimentNameExists()`）
- 如果名称已存在，提示用户重新输入
- 名称合法后，将实验名称设置到 `FuMainMeaPage`

### 1.3 设置实验名称

```
FuMainMeaPage::setExperimentName(name)
    → mExperimentName = name
    → mUi->setExperimentName(name)   // 更新界面显示
```

此时实验名称已保存在 `FuMainMeaPage::mExperimentName` 中，但**实验尚未正式创建到数据库**，也未开始数据记录。

---

## 2. 开启实验数据记录

### 2.1 用户操作入口

用户在测量页面 (`FuMainMeaPage`) 点击 **"录制"切换按钮** (`mRecordingToggleButton`)，触发 `onRecordingToggled(bool checked)` 槽函数。

### 2.2 开启记录流程

`FuMainMeaPage::onRecordingToggled(true)` 执行以下步骤：

```
1. 检查实验名称是否为空
   → 为空则弹出提示 "请先输入实验名称"，重置按钮状态

2. 检查实验名称是否已存在
   → ExperimentParamManager::experimentNameExists(name)
   → 已存在则提示 "实验已存在，请重新输入"

3. 调用 ExperimentParamManager::startRecording(name, &error)
   → 在数据库中正式创建实验记录
   → 启动异步写入工作线程

4. 启用 AI 结果图像保存
   → AiResultSaveManager::instance().setEnabled(true)

5. 更新界面录制状态
   → updateRecordingStatus(true)
```

### 2.3 ExperimentParamManager::startRecording 详解

该方法是实验记录启动的核心，执行以下操作：

```cpp
bool startRecording(const QString &name, QString *error) {
    // 1. 名称校验：不能为空
    // 2. 名称校验：不能重复
    // 3. 调用 DbManager::CreateExperiment(name, timestamp)
    //    → 在 Experiment 表中插入一条记录，返回 exp_id
    // 4. 保存实验状态
    //    → mExperimentName = name
    //    → mExperimentId = expId
    //    → mNextSampleId = -1（将在首次采样时初始化）
    //    → mRecording = true
    // 5. 启动异步数据库写入线程
    //    → ensureWorker()  创建 ExperimentDbWorker + QThread
    //    → mWorkerThread->start()
}
```

**数据库操作（`DbManager::CreateExperiment`）：**

```sql
INSERT INTO Experiment(name, start_time) VALUES(?, ?)
```

- `name`：实验名称
- `start_time`：当前时间戳（毫秒级 epoch）
- 返回自增主键 `exp_id`

---

## 3. 实验数据保存过程

### 3.1 数据流总览

```
摄像头帧 → DetectorWorker → AI推理 → 提交保存结果
                                        ↓
                              AiResultSaveManager::submitResult()
                                        ↓
                           ExperimentParamManager::prepareSample()
                                        ↓
                        ┌───────────────┴───────────────┐
                        ↓                               ↓
              ExperimentDbWorker                AiResultSaveWorker
              (异步写入数据库)                  (异步保存图片文件)
                        ↓                               ↓
                 SQLite 数据库                    磁盘图片文件
              (Data + DetectImage 表)         (ai_results/exp_X/)
```

### 3.2 检测结果触发保存

`DetectorWorker::processDetect()` 是数据保存的触发点。每当从检测队列中取出一帧图像并完成 AI 推理后：

```
DetectorWorker::processDetect(task)
    → q_ori = mat2Image(cv_im)           // 在推理前保存原始图像
    → TFDetectManager::runDetectWithPreview(frame, detect_num, detections)
    → 计算 max_height 和 max_area（从检测框中提取最大火焰高度和面积）
    → TFMeaManager::receiveStatistics(max_height, max_area)  // 更新实时曲线
    → q_im = mat2Image(cv_im)            // 推理后的检测结果图像
    → AiResultSaveManager::submitResult(q_im, q_ori, sourceFlag, timeCost,
                                         detectionId, detect_num,
                                         max_height, max_area)
```

其中 `q_ori` 是检测前的原始图像，`q_im` 是检测后带有标注的结果图像，两者同时传入保存流程。

### 3.3 AiResultSaveManager::submitResult 详解

该方法是数据保存的调度中心，执行以下过滤和调度逻辑：

```
submitResult(detImage, oriImage, sourceFlag, timeCost, detectionId, detectedCount, fireHeight, fireArea)
│
├── 检查 1：mEnabled 是否为 true（录制是否已开启）
│   → false 则直接返回
│
├── 检查 2：TFDetectManager::isDetecting() 是否为 true（AI 检测是否正在运行）
│   → false 则直接返回
│
├── 检查 3：shouldSaveNow()（频率控制）
│   → 基于 mSaveFrequency（每秒最多保存 N 张图片）
│   → 使用滑动窗口计数器，1 秒窗口内超过上限则跳过
│
├── 调用 ExperimentParamManager::prepareSample(fireHeight, fireArea)
│   → 准备采样记录并异步入库（详见 3.4）
│   → 返回包含两个图片路径（imagePath + oriImagePath）的 ExperimentRecord
│
├── 将检测后图片保存任务提交给 AiResultSaveWorker
│   → mWorker->enqueue(detImage, record->imagePath, description)
│
├── 将检测前原始图片保存任务提交给 AiResultSaveWorker
│   → mWorker->enqueue(oriImage, record->oriImagePath, description)
│
└── 记录元数据到内存（最近 50 条）
    → recordMeta(detFilePath, description)
```

### 3.4 ExperimentParamManager::prepareSample 详解

该方法负责组装一条完整的实验采样记录：

```cpp
std::optional<ExperimentRecord> prepareSample(float fireHeight, float fireArea) {
    // 1. 检查是否正在记录
    if (!isRecording() || mExperimentId < 0) return nullopt;

    // 2. 组装 ExperimentRecord
    record.expId       = mExperimentId;
    record.sampleId    = nextSampleId();          // 自增采样序号
    record.timestampMs = currentTimestampMs();     // 当前毫秒时间戳
    record.dist        = TFMeaManager::currentDist();       // 激光测距值
    record.tilt        = TFMeaManager::currentTiltAngle();  // IMU 倾斜角度
    record.fireHeight  = fireHeight;               // 火焰高度（像素）
    record.fireArea    = fireArea;                 // 火焰面积（像素²）
    record.imagePath    = buildDetImagePath(sampleId); // 检测后图片路径
    record.oriImagePath = buildOriImagePath(sampleId); // 检测前原始图片路径

    // 3. 将记录入队到异步工作线程
    mWorker->enqueue(record);

    return record;
}
```

**采样序号（sampleId）生成逻辑：**

- 首次调用时从数据库查询当前实验的最大 `sample_id`，在此基础上 +1
- 后续调用直接自增（`mNextSampleId++`）

**图片路径生成规则：**

每个采样生成两张图片，保存在同一目录下：

```
检测后图像：{当前工作目录}/ai_results/exp_{expId}/sample_det_{sampleId:06d}.png
检测前图像：{当前工作目录}/ai_results/exp_{expId}/sample_ori_{sampleId:06d}.png
```

例如：
- `ai_results/exp_3/sample_det_000001.png`（检测后，带标注框）
- `ai_results/exp_3/sample_ori_000001.png`（检测前，原始图像）

### 3.5 异步数据库写入（ExperimentDbWorker）

`ExperimentDbWorker` 运行在独立的 `QThread` 中，通过生产者-消费者模式处理数据：

**入队（生产者端）：**

```cpp
void enqueue(const ExperimentRecord &record) {
    QMutexLocker locker(&mMutex);
    mQueue.enqueue(record);
    mCond.wakeOne();  // 唤醒消费者线程
}
```

**消费循环（工作线程）：**

```cpp
void startWork() {
    mRunning = true;
    while (mRunning || !mQueue.isEmpty()) {
        // 加锁等待队列中有数据（超时 200ms）
        // 取出一条记录
        // 调用 process(record) 写入数据库
    }
}
```

**写入数据库（process 方法）：**

每条 `ExperimentRecord` 被拆分为 **4 个通道** 写入 `Data` 表：

| 通道 ID | 含义 | 字段来源 |
|---------|------|---------|
| 1 | 距离（dist） | 激光测距仪 |
| 2 | 倾斜角（tilt） | IMU 传感器 |
| 3 | 火焰高度（fireHeight） | AI 检测结果 |
| 4 | 火焰面积（fireArea） | AI 检测结果 |

```cpp
void process(const ExperimentRecord &record) {
    // 1. 批量插入 4 条数据行（使用 INSERT OR REPLACE）
    DbManager::instance().InsertBatch(rows, ConflictPolicy::Replace);

    // 2. 如果有图片路径，插入/更新检测图片记录（包含检测后和检测前两个路径）
    DbManager::instance().UpsertDetectImage(expId, sampleId, imagePath, oriImagePath);
}
```

**对应 SQL 操作：**

```sql
-- Data 表写入（4 行，每个通道 1 行）
INSERT OR REPLACE INTO Data(exp_id, channel_id, sample_id, DateTime, value)
VALUES(?, ?, ?, ?, ?)

-- DetectImage 表写入（同时存储检测后和检测前图片路径）
INSERT INTO DetectImage(exp_id, sample_id, image_path, ori_image_path)
VALUES(?, ?, ?, ?)
ON CONFLICT(exp_id, sample_id) DO UPDATE SET
  image_path=excluded.image_path,
  ori_image_path=excluded.ori_image_path
```

### 3.6 异步图片保存（AiResultSaveWorker）

`AiResultSaveWorker` 同样运行在独立的 `QThread` 中：

```
入队 → enqueue(image, filePath, description)
        ↓
工作线程循环 → 等待条件变量
        ↓
取出任务 → 确保目录存在 → image.save(filePath)
```

每个采样会产生两个图片保存任务，均以 PNG 格式写入磁盘：
- 检测后图像：`ai_results/exp_{expId}/sample_det_{sampleId:06d}.png`
- 检测前原始图像：`ai_results/exp_{expId}/sample_ori_{sampleId:06d}.png`

---

## 4. 停止实验记录

### 4.1 用户操作

用户再次点击"录制"按钮（取消选中），触发 `onRecordingToggled(false)`。

### 4.2 停止流程

```
FuMainMeaPage::onRecordingToggled(false)
│
├── ExperimentParamManager::stopRecording()
│   ├── mRecording = false
│   ├── DbManager::EndExperiment(expId, endTimestamp)
│   │   → UPDATE Experiment SET end_time=? WHERE exp_id=?
│   ├── shutdownWorker()
│   │   ├── ExperimentDbWorker::stopWork()  // 设置 mRunning=false，唤醒线程
│   │   ├── mWorkerThread->quit()
│   │   └── mWorkerThread->wait()          // 等待线程结束
│   ├── mExperimentId = -1
│   └── mExperimentName.clear()
│
├── AiResultSaveManager::setEnabled(false)
│   ├── AiResultSaveWorker::stopWork()
│   ├── mThread->quit() → mThread->wait()
│   └── 清理 worker 和 thread
│
└── updateRecordingStatus(false)
    → 更新界面录制状态指示
```

---

## 5. 数据库表结构

### 5.1 Experiment 表

存储实验元信息。

| 列名 | 类型 | 说明 |
|------|------|------|
| exp_id | INTEGER PRIMARY KEY | 实验 ID（自增） |
| name | TEXT NOT NULL | 实验名称 |
| start_time | INTEGER NOT NULL | 开始时间（epoch 毫秒） |
| end_time | INTEGER | 结束时间（NULL 表示未结束） |

### 5.2 Data 表

存储实验采样数据，每条采样生成 4 行（4 个通道）。

| 列名 | 类型 | 说明 |
|------|------|------|
| exp_id | INTEGER NOT NULL | 实验 ID |
| channel_id | INTEGER NOT NULL | 通道 ID（1=距离, 2=倾斜角, 3=火焰高度, 4=火焰面积） |
| sample_id | INTEGER NOT NULL | 采样序号 |
| DateTime | INTEGER NOT NULL | 时间戳（epoch 毫秒） |
| value | REAL NOT NULL | 数据值 |

主键：`(exp_id, channel_id, sample_id)`

### 5.3 DetectImage 表

存储检测图片路径（检测后图像和检测前原始图像）。

| 列名 | 类型 | 说明 |
|------|------|------|
| exp_id | INTEGER NOT NULL | 实验 ID |
| sample_id | INTEGER NOT NULL | 采样序号 |
| image_path | TEXT | 检测后图片文件路径（`sample_det_*.png`） |
| ori_image_path | TEXT | 检测前原始图片文件路径（`sample_ori_*.png`） |

主键：`(exp_id, sample_id)`

> **注意：** 显示实验结果时，仅使用 `image_path`（检测后图像），不显示检测前图像。

---

## 6. 线程模型

整个实验数据保存涉及 3 个工作线程：

| 线程 | 类 | 职责 |
|------|----|----|
| DetectorWorker 线程 | `DetectorWorkerManager` → `DetectorWorker` | AI 推理 + 触发数据保存 |
| ExperimentDbWorker 线程 | `ExperimentParamManager` → `ExperimentDbWorker` | 异步写入实验数据到 SQLite |
| AiResultSaveWorker 线程 | `AiResultSaveManager` → `AiResultSaveWorker` | 异步保存检测结果图片到磁盘 |

三个线程均采用 **生产者-消费者模式**（`QMutex` + `QWaitCondition` + `QQueue`），确保主线程和检测线程不会因数据库 I/O 或磁盘 I/O 而阻塞。

---

## 7. 关键类职责一览

| 类 | 文件 | 职责 |
|----|------|------|
| `ExpInfoDialog` | `Gui/Controls/ExpInfo/ExpInfoDialog.cpp` | 实验名称输入对话框 |
| `FuMainMeaPage` | `Gui/Pages/FuMainMeaPage.cpp` | 测量主页面，管理录制按钮状态 |
| `ExperimentParamManager` | `Measurement/ExperimentParamManager.cpp` | 实验参数管理，采样数据组装与异步入库调度 |
| `ExperimentDbWorker` | `Measurement/ExperimentParamManager.cpp` | 后台线程，将采样记录写入 SQLite |
| `AiResultSaveManager` | `Detector/AiResultSaveManager.cpp` | AI 结果保存管理，频率控制与图片保存调度 |
| `AiResultSaveWorker` | `Detector/AiResultSaveManager.cpp` | 后台线程，将检测前/后图片写入磁盘 |
| `DetectorWorker` | `Detector/DetectorWorker.cpp` | AI 推理工作线程，推理完成后触发数据保存 |
| `DbManager` | `Db/DbManager.cpp` | SQLite 数据库访问层，提供实验/数据的 CRUD 操作 |
| `TFMeaManager` | `Measurement/TFMeaManager.cpp` | 测量管理器，提供实时距离和倾斜角数据 |
