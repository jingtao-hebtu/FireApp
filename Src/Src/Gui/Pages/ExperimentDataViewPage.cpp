#include "ExperimentDataViewPage.h"
#include "ExperimentViewWorker.h"
#include "DbManager.h"

#include <QComboBox>
#include <QDateTimeEdit>
#include <QFrame>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QSlider>
#include <QTableWidget>
#include <QVBoxLayout>
#include <QThread>
#include <QShowEvent>
#include <QPixmap>
#include <QFileInfo>
#include <QMetaType>
#include <QResizeEvent>
#include <QSignalBlocker>
#include <algorithm>
#include <cmath>

namespace TF {

    namespace {
        constexpr int kDefaultDayRange = 7;

        QLabel* createCaptionLabel(const QString &text, QWidget *parent) {
            auto *label = new QLabel(text, parent);
            label->setObjectName(QStringLiteral("PanelTitle"));
            label->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
            return label;
        }

        QFrame* createCard(QWidget *parent) {
            auto *frame = new QFrame(parent);
            frame->setObjectName(QStringLiteral("StatisticsFrame"));
            frame->setFrameShape(QFrame::StyledPanel);
            frame->setFrameShadow(QFrame::Plain);
            return frame;
        }
    }

    ExperimentDataViewPage::ExperimentDataViewPage(QWidget *parent)
        : QWidget(parent) {
        qRegisterMetaType<ExperimentBrief>("ExperimentBrief");
        qRegisterMetaType<SampleIndexItem>("SampleIndexItem");
        qRegisterMetaType<QVector<ExperimentBrief>>("QVector<ExperimentBrief>");
        qRegisterMetaType<QVector<SampleIndexItem>>("QVector<SampleIndexItem>");
        qRegisterMetaType<QVector<QPair<QString, double>>>("QVector<QPair<QString,double>>");

        setupUi();
        setupConnections();
        ensureWorker();
    }

    ExperimentDataViewPage::~ExperimentDataViewPage() {
        if (mWorkerThread) {
            mWorkerThread->quit();
            mWorkerThread->wait();
            mWorkerThread = nullptr;
            mWorker = nullptr;
        }
    }

    void ExperimentDataViewPage::setupUi() {
        auto *mainLayout = new QVBoxLayout(this);
        mainLayout->setContentsMargins(12, 12, 12, 12);
        mainLayout->setSpacing(12);

        // Query bar
        auto *queryCard = createCard(this);
        auto *queryLayout = new QHBoxLayout(queryCard);
        queryLayout->setContentsMargins(12, 10, 12, 10);
        queryLayout->setSpacing(10);

        mExperimentCombo = new QComboBox(queryCard);
        mExperimentCombo->setMinimumWidth(220);

        mFromEdit = new QDateTimeEdit(queryCard);
        mFromEdit->setCalendarPopup(true);
        mToEdit = new QDateTimeEdit(queryCard);
        mToEdit->setCalendarPopup(true);
        const auto now = QDateTime::currentDateTime();
        mFromEdit->setDateTime(now.addDays(-kDefaultDayRange));
        mToEdit->setDateTime(now);

        mQueryButton = new QPushButton(tr("查询"), queryCard);
        mLoadButton = new QPushButton(tr("加载实验"), queryCard);

        queryLayout->addWidget(new QLabel(tr("实验"), queryCard));
        queryLayout->addWidget(mExperimentCombo);
        queryLayout->addWidget(new QLabel(tr("开始"), queryCard));
        queryLayout->addWidget(mFromEdit);
        queryLayout->addWidget(new QLabel(tr("结束"), queryCard));
        queryLayout->addWidget(mToEdit);
        queryLayout->addWidget(mQueryButton);
        queryLayout->addStretch();
        queryLayout->addWidget(mLoadButton);

        // Timeline
        auto *timelineCard = createCard(this);
        auto *timelineLayout = new QGridLayout(timelineCard);
        timelineLayout->setContentsMargins(12, 10, 12, 10);
        timelineLayout->setVerticalSpacing(6);
        timelineLayout->setHorizontalSpacing(10);

        mStartLabel = new QLabel(tr("--"), timelineCard);
        mEndLabel = new QLabel(tr("--"), timelineCard);
        mSampleSlider = new QSlider(Qt::Horizontal, timelineCard);
        mSampleSlider->setMinimum(0);
        mSampleSlider->setMaximum(0);
        mSampleSlider->setEnabled(false);

        mPointEdit = new QDateTimeEdit(timelineCard);
        mPointEdit->setCalendarPopup(true);
        mPointEdit->setDisplayFormat(QStringLiteral("yyyy-MM-dd HH:mm:ss"));
        mViewButton = new QPushButton(tr("查看"), timelineCard);

        timelineLayout->addWidget(createCaptionLabel(tr("起始"), timelineCard), 0, 0);
        timelineLayout->addWidget(mStartLabel, 0, 1);
        timelineLayout->addWidget(createCaptionLabel(tr("结束"), timelineCard), 0, 3);
        timelineLayout->addWidget(mEndLabel, 0, 4);
        timelineLayout->addWidget(mSampleSlider, 1, 0, 1, 5);
        timelineLayout->addWidget(createCaptionLabel(tr("时间点"), timelineCard), 2, 0);
        timelineLayout->addWidget(mPointEdit, 2, 1, 1, 3);
        timelineLayout->addWidget(mViewButton, 2, 4);

        // Content area
        auto *contentWidget = new QWidget(this);
        auto *contentLayout = new QHBoxLayout(contentWidget);
        contentLayout->setSpacing(12);
        contentLayout->setContentsMargins(0, 0, 0, 0);

        auto *imageCard = createCard(contentWidget);
        auto *imageLayout = new QVBoxLayout(imageCard);
        imageLayout->setContentsMargins(12, 10, 12, 10);
        imageLayout->setSpacing(8);

        imageLayout->addWidget(createCaptionLabel(tr("检测图像"), imageCard));

        mImageLabel = new QLabel(imageCard);
        mImageLabel->setObjectName(QStringLiteral("DetectImageLabel"));
        mImageLabel->setAlignment(Qt::AlignCenter);
        mImageLabel->setMinimumSize(320, 240);
        mImageLabel->setText(tr("暂无图像"));
        mImageLabel->setStyleSheet(QStringLiteral("background-color: rgba(14,26,48,120); border: 1px solid #1f2c46;"));
        mImageLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        imageLayout->addWidget(mImageLabel, 1);

        auto *tableCard = createCard(contentWidget);
        auto *tableLayout = new QVBoxLayout(tableCard);
        tableLayout->setContentsMargins(12, 10, 12, 10);
        tableLayout->setSpacing(8);
        tableLayout->addWidget(createCaptionLabel(tr("通道数据"), tableCard));

        mTable = new QTableWidget(tableCard);
        mTable->setColumnCount(2);
        mTable->setHorizontalHeaderLabels({tr("Channel"), tr("Value")});
        mTable->horizontalHeader()->setStretchLastSection(true);
        mTable->verticalHeader()->setVisible(false);
        mTable->setSelectionBehavior(QAbstractItemView::SelectRows);
        mTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
        tableLayout->addWidget(mTable, 1);

        contentLayout->addWidget(imageCard, 1);
        contentLayout->addWidget(tableCard, 1);

        mainLayout->addWidget(queryCard);
        mainLayout->addWidget(timelineCard);
        mainLayout->addWidget(contentWidget, 1);
    }

    void ExperimentDataViewPage::setupConnections() {
        connect(mQueryButton, &QPushButton::clicked, this, &ExperimentDataViewPage::onQueryExperiments);
        connect(mLoadButton, &QPushButton::clicked, this, &ExperimentDataViewPage::onLoadSamples);
        connect(mSampleSlider, &QSlider::valueChanged, this, &ExperimentDataViewPage::onSliderValueChanged);
        connect(mViewButton, &QPushButton::clicked, this, &ExperimentDataViewPage::onViewSample);
        connect(mPointEdit, &QDateTimeEdit::editingFinished, this, [this]() {
            mUserEditedTime = true;
        });
    }

    void ExperimentDataViewPage::ensureWorker() {
        if (mWorkerThread) {
            return;
        }

        const auto dbFile = QString::fromStdString(DbManager::instance().databaseFile());
        mWorkerThread = new QThread(this);
        mWorker = new ExperimentViewWorker(dbFile);
        mWorker->moveToThread(mWorkerThread);

        connect(mWorkerThread, &QThread::finished, mWorker, &QObject::deleteLater);
        connect(this, &ExperimentDataViewPage::destroyed, mWorkerThread, &QThread::quit);

        connect(mWorker, &ExperimentViewWorker::experimentsReady, this, &ExperimentDataViewPage::onExperimentsLoaded);
        connect(mWorker, &ExperimentViewWorker::experimentSamplesReady, this, &ExperimentDataViewPage::onSamplesLoaded);
        connect(mWorker, &ExperimentViewWorker::sampleValuesReady, this, &ExperimentDataViewPage::onSampleValuesLoaded);
        connect(mWorker, &ExperimentViewWorker::errorOccurred, this, &ExperimentDataViewPage::onWorkerError);

        mWorkerThread->start();
    }

    void ExperimentDataViewPage::showEvent(QShowEvent *event) {
        QWidget::showEvent(event);
        if (mFirstShow) {
            mFirstShow = false;
        }
        refreshExperiments();
    }

    void ExperimentDataViewPage::resizeEvent(QResizeEvent *event) {
        QWidget::resizeEvent(event);
        if (!mCurrentPixmap.isNull()) {
            const auto scaled = mCurrentPixmap.scaled(mImageLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
            mImageLabel->setPixmap(scaled);
        }
    }

    void ExperimentDataViewPage::refreshExperiments() {
        setQueryButtonsEnabled(false);
        const auto from = mFromEdit->dateTime().toMSecsSinceEpoch();
        const auto to = mToEdit->dateTime().toMSecsSinceEpoch();
        QMetaObject::invokeMethod(mWorker, &ExperimentViewWorker::queryExperiments, Qt::QueuedConnection,
                                  from, to);
    }

    void ExperimentDataViewPage::onQueryExperiments() {
        refreshExperiments();
    }

    void ExperimentDataViewPage::onExperimentsLoaded(const QVector<ExperimentBrief> &experiments) {
        setQueryButtonsEnabled(true);
        mExperimentCombo->clear();
        setSampleControlsEnabled(false);
        mSampleSlider->setRange(0, 0);
        mCurrentExpId = -1;
        mSamples.clear();
        mStartLabel->setText(tr("--"));
        mEndLabel->setText(tr("--"));
        mTable->setRowCount(0);
        mImageLabel->setPixmap(QPixmap());
        mImageLabel->setText(tr("暂无图像"));
        for (const auto &exp : experiments) {
            const QString text = QStringLiteral("%1 - %2").arg(exp.expId).arg(exp.name);
            mExperimentCombo->addItem(text, exp.expId);
        }
        if (!experiments.isEmpty()) {
            mExperimentCombo->setCurrentIndex(0);
        }
    }

    void ExperimentDataViewPage::onLoadSamples() {
        if (mExperimentCombo->count() <= 0) {
            return;
        }
        const int expId = mExperimentCombo->currentData().toInt();
        if (expId < 0) {
            return;
        }
        mCurrentExpId = expId;
        mSamples.clear();
        mTable->setRowCount(0);
        mImageLabel->setPixmap(QPixmap());
        mImageLabel->setText(tr("暂无图像"));
        setSampleControlsEnabled(false);
        QMetaObject::invokeMethod(mWorker, &ExperimentViewWorker::loadExperimentSamples, Qt::QueuedConnection,
                                  expId);
    }

    void ExperimentDataViewPage::onSamplesLoaded(int expId, const QVector<SampleIndexItem> &samples) {
        if (expId != mCurrentExpId) {
            return;
        }
        mSamples = samples;
        for (auto &s : mSamples) {
            s.datetime = normalizeToMillis(s.datetime);
        }
        setSampleControlsEnabled(true);

        if (mSamples.isEmpty()) {
            mSampleSlider->setEnabled(false);
            mSampleSlider->setRange(0, 0);
            mStartLabel->setText(tr("--"));
            mEndLabel->setText(tr("--"));
            mPointEdit->setDateTime(QDateTime());
            mImageLabel->setText(tr("暂无图像"));
            mImageLabel->setPixmap(QPixmap());
            mTable->setRowCount(0);
            return;
        }

        mSampleSlider->setEnabled(true);
        mSampleSlider->setRange(0, mSamples.size() - 1);
        mSampleSlider->setValue(0);
        updateTimelineLabels();
        updateTimeEditFromIndex(0);
    }

    void ExperimentDataViewPage::updateTimelineLabels() {
        if (mSamples.isEmpty()) {
            mStartLabel->setText(tr("--"));
            mEndLabel->setText(tr("--"));
            return;
        }
        const auto startDt = QDateTime::fromMSecsSinceEpoch(mSamples.front().datetime);
        const auto endDt = QDateTime::fromMSecsSinceEpoch(mSamples.back().datetime);
        mStartLabel->setText(startDt.toString(QStringLiteral("yyyy-MM-dd HH:mm:ss")));
        mEndLabel->setText(endDt.toString(QStringLiteral("yyyy-MM-dd HH:mm:ss")));
    }

    qint64 ExperimentDataViewPage::normalizeToMillis(qint64 timestamp) const {
        const qint64 absValue = std::abs(timestamp);
        if (absValue > 1000000000000000LL) { // likely ns
            return timestamp / 1000000;
        }
        if (absValue > 10000000000000LL) { // likely us
            return timestamp / 1000;
        }
        return timestamp;
    }

    void ExperimentDataViewPage::updateTimeEditFromIndex(int index) {
        if (index < 0 || index >= mSamples.size()) {
            return;
        }
        QSignalBlocker blocker(mPointEdit);
        const auto dt = QDateTime::fromMSecsSinceEpoch(mSamples.at(index).datetime);
        mPointEdit->setDateTime(dt);
        mUserEditedTime = false;
    }

    void ExperimentDataViewPage::onSliderValueChanged(int value) {
        updateTimeEditFromIndex(value);
    }

    int ExperimentDataViewPage::findNearestSampleIndex(qint64 datetime) const {
        if (mSamples.isEmpty()) {
            return -1;
        }

        const auto millis = normalizeToMillis(datetime);
        auto comp = [](const SampleIndexItem &item, qint64 value) {
            return item.datetime < value;
        };
        auto it = std::lower_bound(mSamples.begin(), mSamples.end(), millis, comp);
        if (it == mSamples.begin()) {
            return 0;
        }
        if (it == mSamples.end()) {
            return mSamples.size() - 1;
        }
        const auto idx = std::distance(mSamples.begin(), it);
        const auto prevIdx = idx - 1;
        const auto diffPrev = std::abs(mSamples.at(prevIdx).datetime - millis);
        const auto diffCurr = std::abs(mSamples.at(idx).datetime - millis);
        return diffPrev <= diffCurr ? static_cast<int>(prevIdx) : static_cast<int>(idx);
    }

    void ExperimentDataViewPage::onViewSample() {
        if (mSamples.isEmpty() || mCurrentExpId < 0) {
            return;
        }

        int index = mSampleSlider->value();
        if (mUserEditedTime) {
            const auto targetMs = mPointEdit->dateTime().toMSecsSinceEpoch();
            const int nearest = findNearestSampleIndex(targetMs);
            if (nearest >= 0) {
                index = nearest;
                mSampleSlider->setValue(index);
            }
        }

        if (index < 0 || index >= mSamples.size()) {
            return;
        }

        const auto &sample = mSamples.at(index);
        mPendingImagePath = sample.imagePath;
        requestSampleData(mCurrentExpId, sample.sampleId);
    }

    void ExperimentDataViewPage::requestSampleData(int expId, int sampleId) {
        setSampleControlsEnabled(false);
        QMetaObject::invokeMethod(mWorker, &ExperimentViewWorker::loadSampleValues, Qt::QueuedConnection,
                                  expId, sampleId);
    }

    void ExperimentDataViewPage::onSampleValuesLoaded(int expId, int sampleId, const QVector<QPair<QString, double>> &values, const QString &imagePath) {
        if (expId != mCurrentExpId) {
            return;
        }
        Q_UNUSED(sampleId);
        setSampleControlsEnabled(true);
        updateTable(values);
        const QString resolved = !imagePath.isEmpty() ? imagePath : mPendingImagePath;
        showSampleImage(resolved);
    }

    void ExperimentDataViewPage::onWorkerError(const QString &msg) {
        setQueryButtonsEnabled(true);
        setSampleControlsEnabled(true);
        QMessageBox::warning(this, tr("提示"), msg);
    }

    void ExperimentDataViewPage::setQueryButtonsEnabled(bool enabled) {
        mQueryButton->setEnabled(enabled);
        mLoadButton->setEnabled(enabled);
    }

    void ExperimentDataViewPage::setSampleControlsEnabled(bool enabled) {
        const bool available = enabled && !mSamples.isEmpty();
        mSampleSlider->setEnabled(available);
        mViewButton->setEnabled(available);
        mPointEdit->setEnabled(available);
    }

    void ExperimentDataViewPage::showSampleImage(const QString &imagePath) {
        const QFileInfo info(imagePath);
        if (!info.exists() || !info.isFile()) {
            mImageLabel->setText(tr("未找到图像"));
            mImageLabel->setPixmap(QPixmap());
            return;
        }

        QPixmap pix(imagePath);
        if (pix.isNull()) {
            mImageLabel->setText(tr("图像加载失败"));
            mImageLabel->setPixmap(QPixmap());
            return;
        }

        mImageLabel->setText(QString());
        mCurrentPixmap = pix;
        const auto scaled = mCurrentPixmap.scaled(mImageLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
        mImageLabel->setPixmap(scaled);
    }

    void ExperimentDataViewPage::updateTable(const QVector<QPair<QString, double>> &values) {
        mTable->setRowCount(values.size());
        for (int row = 0; row < values.size(); ++row) {
            const auto &pair = values.at(row);
            auto *nameItem = new QTableWidgetItem(pair.first);
            auto *valueItem = new QTableWidgetItem(QString::number(pair.second, 'f', 3));
            nameItem->setFlags(nameItem->flags() & ~Qt::ItemIsEditable);
            valueItem->setFlags(valueItem->flags() & ~Qt::ItemIsEditable);
            mTable->setItem(row, 0, nameItem);
            mTable->setItem(row, 1, valueItem);
        }
    }
}

