#pragma once

#include <QWidget>
#include <QVector>
#include <QDateTime>
#include <QString>
#include <QPair>
#include <QPixmap>

class QComboBox;
class QDateTimeEdit;
class QPushButton;
class QSlider;
class QLabel;
class QTableWidget;
class QFrame;
class QThread;

namespace TF {

    class ExperimentViewWorker;

    struct ExperimentBrief {
        int expId{ -1 };
        QString name;
        qint64 startTime{ 0 };
        bool hasEndTime{ false };
        qint64 endTime{ 0 };
    };

    struct SampleIndexItem {
        int sampleId{ -1 };
        qint64 datetime{ 0 };
        QString imagePath;
    };

    class ExperimentDataViewPage : public QWidget {
        Q_OBJECT
    public:
        explicit ExperimentDataViewPage(QWidget *parent = nullptr);
        ~ExperimentDataViewPage() override;

    protected:
        void showEvent(QShowEvent *event) override;
        void resizeEvent(QResizeEvent *event) override;

    private slots:
        void onQueryExperiments();
        void onExperimentsLoaded(const QVector<ExperimentBrief> &experiments);
        void onLoadSamples();
        void onSamplesLoaded(int expId, const QVector<SampleIndexItem> &samples);
        void onSliderValueChanged(int value);
        void onPrevSample();
        void onNextSample();
        void onViewSample();
        void onSampleValuesLoaded(int expId, int sampleId, const QVector<QPair<QString, double>> &values, const QString &imagePath);
        void onWorkerError(const QString &msg);

    private:
        void setupUi();
        void setupConnections();
        void ensureWorker();
        void refreshExperiments();
        void setQueryButtonsEnabled(bool enabled);
        void setSampleControlsEnabled(bool enabled, bool keepNavigationEnabled = false);
        void updateTimelineLabels();
        void updateTimeEditFromIndex(int index);
        int findNearestSampleIndex(qint64 datetime) const;
        void requestSampleData(int expId, int sampleId);
        void showSampleImage(const QString &imagePath);
        void updateTable(const QVector<QPair<QString, double>> &values);
        qint64 normalizeToMillis(qint64 timestamp) const;
        void updateSampleCountLabel(int count);

    private:
        QComboBox *mExperimentCombo{ nullptr };
        QDateTimeEdit *mFromEdit{ nullptr };
        QDateTimeEdit *mToEdit{ nullptr };
        QPushButton *mQueryButton{ nullptr };
        QPushButton *mLoadButton{ nullptr };
        QLabel *mSampleCountLabel{ nullptr };

        QSlider *mSampleSlider{ nullptr };
        QLabel *mStartLabel{ nullptr };
        QLabel *mEndLabel{ nullptr };
        QDateTimeEdit *mPointEdit{ nullptr };
        QPushButton *mViewButton{ nullptr };
        QPushButton *mPrevButton{ nullptr };
        QPushButton *mNextButton{ nullptr };

        QLabel *mImageLabel{ nullptr };
        QTableWidget *mTable{ nullptr };

        QVector<SampleIndexItem> mSamples;
        int mCurrentExpId{ -1 };
        bool mUserEditedTime{ false };
        bool mFirstShow{ true };
        bool mLoadingSample{ false };
        int mPendingIndex{ -1 };
        QString mPendingImagePath;

        QThread *mWorkerThread{ nullptr };
        ExperimentViewWorker *mWorker{ nullptr };

        QPixmap mCurrentPixmap;
    };

}

Q_DECLARE_METATYPE(TF::ExperimentBrief)
Q_DECLARE_METATYPE(TF::SampleIndexItem)
