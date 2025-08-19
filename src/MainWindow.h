#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QCamera>
#include <QMediaCaptureSession>
#include <QVideoSink>

#include "AbstractFaceDetector.h"

#include "AiAlgorithmFeatures.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
   Q_OBJECT

public:
   MainWindow(QWidget *parent = nullptr);
   ~MainWindow();

private slots:
   void OnStartCamera();
   void OnStopCamera();
   void OnFocusModeChange(int index);
   void OnFocusDistanceChanged(int value);
   void OnFaceDetectionEnabled(bool checked);
   void OnHandleVideoFrame(const QVideoFrame& frame);
   void OnVideoSizeChange();
   void OnHaarParametersChanged();
   void OnAiAlgoChanged(const QString& algorithm);
   void OnClearErrors();
   void OnKeepOnlyLastErrorChanged(bool checked);

private:
   void SetupUiSupportedFeatures();
   void SetErrorsToUi(const std::vector<std::string>& errors);

   QCamera::FocusMode GetFocusMode(const QString& optionName) const;
   cv::Mat ToCvMat(const QImage& image) const;
   QList<QRect> ToQRect(const std::vector<cv::Rect>& box) const;
   AiAlgorithmFeatures GetAiFeatures(const QString& option) const;

   Ui::MainWindow *ui;
   QCamera* camera = nullptr;
   QVideoSink videoSink;
   QMediaCaptureSession captureSession;
   AbstractFaceDetector* faceDetector = nullptr;
};
#endif // MAINWINDOW_H
