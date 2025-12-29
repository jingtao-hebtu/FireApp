# 布局问题修复说明

## 根因
- 触摸屏实际可用宽度 954px，而窗口 `minimumSizeHint().width()` 为 1358px，窗口在创建时就比屏幕宽，WM 无法提供最大化按钮也无法放大至全屏。
- 主要宽度贡献来自：
  - 六个 TechToggleButton/FuVideoButtons 在基类里设置了 `setMinimumSize(120, 120)`，控制区仅按钮就要 ≥720px。
  - BatteryPanel 设定 `setMinimumWidth(260)` 且尺寸策略为 Fixed，横向不可压缩。
  - 页面初始尺寸 `mWid->resize(height, width)` 参数反转，默认宽度被错误地设到 1200px，更放大了最小尺寸基准。

## 修改内容
- 为主窗口新增 `dumpLayoutDiagnostics()`，可通过 `--dump-layout` 或 `UI_DUMP_LAYOUT=1` 输出屏幕与布局约束、子控件最小宽度贡献，便于现场定位。
- 将 TechButtonBase 最小尺寸改为更灵活的 `72x66`，保持高度但允许水平压缩。
- BatteryPanel 改为 `Preferred` 宽度策略，最小宽度降至 200px；MetricsPanel 也允许压缩，同时设定 140px 底线以稳定布局。
- 修正测量页面初始化尺寸的 width/height 传参顺序，避免错误的超宽默认值。

## 复测步骤
1. 触摸屏（1024x600）执行：`UI_DUMP_LAYOUT=1 ./FireApp --dump-layout`，确认 `[Window] minSize` 宽度 ≤ 可用 954px，窗口可被最大化。
2. 桌面显示器（1920x1080）执行同样命令，确认最小宽度保持在 ~930px，布局未明显缩小。
3. 正常启动应用，确认窗口可自由缩放，最大化按钮可用，功能不受影响。
