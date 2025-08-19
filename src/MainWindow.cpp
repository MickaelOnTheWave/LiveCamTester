#include "MainWindow.h"
#include "./ui_MainWindow.h"

#include <chrono>
#include <opencv2/imgproc.hpp>
#include <QCamera>
#include <QMediaDevices>

#include "CnnFaceDetector.h"
#include "HaarFaceDetector.h"

MainWindow::MainWindow(
    QWidget *parent)
  : QMainWindow(parent)
  , ui(new Ui::MainWindow)
{
   ui->setupUi(this);

   const QList<QCameraDevice> cameras = QMediaDevices::videoInputs();
   camera = new QCamera(cameras.front());
   ui->camNameLabel->setText(cameras.front().description());

   captureSession.setCamera(camera);
   captureSession.setVideoSink(&videoSink);

   SetupUiSupportedFeatures();

   connect(ui->startCamButton, &QPushButton::clicked, this, &MainWindow::OnStartCamera);
   connect(ui->stopCamButton, &QPushButton::clicked, this, &MainWindow::OnStopCamera);
   connect(ui->focusModeCombo, &QComboBox::currentIndexChanged, this, &MainWindow::OnFocusModeChange);
   connect(ui->focusSlider, &QSlider::valueChanged, this, &MainWindow::OnFocusDistanceChanged);
   connect(ui->aiFaceBox, &QCheckBox::clicked, this, &MainWindow::OnFaceDetectionEnabled);
   connect(ui->aiAlgoCombo, &QComboBox::currentTextChanged, this, &MainWindow::OnAiAlgoChanged);
   connect(ui->clearErrorsButton, &QPushButton::clicked, this, &MainWindow::OnClearErrors);
   connect(ui->onlyLastFrameBox, &QCheckBox::clicked, this, &MainWindow::OnKeepOnlyLastErrorChanged);

   connect(ui->scaleFactorBox, &QDoubleSpinBox::valueChanged, this, &MainWindow::OnHaarParametersChanged);
   connect(ui->neighbourBox, &QSpinBox::valueChanged, this, &MainWindow::OnHaarParametersChanged);
   connect(ui->minSizeBox, &QSpinBox::valueChanged, this, &MainWindow::OnHaarParametersChanged);
   connect(ui->maxSizeBox, &QSpinBox::valueChanged, this, &MainWindow::OnHaarParametersChanged);

   connect(&videoSink, &QVideoSink::videoFrameChanged, this, &MainWindow::OnHandleVideoFrame);
   connect(&videoSink, &QVideoSink::videoSizeChanged, this, &MainWindow::OnVideoSizeChange);

   OnFaceDetectionEnabled(ui->aiFaceBox->isChecked());
   OnAiAlgoChanged(ui->aiAlgoCombo->currentText());

   SetErrorsToUi({});
}

MainWindow::~MainWindow()
{
   delete ui;
   delete camera;
}

void MainWindow::OnStartCamera()
{
   camera->start();
}

void MainWindow::OnStopCamera()
{
   camera->stop();
}

void MainWindow::OnFocusModeChange(int index)
{
   const QString itemName = ui->focusModeCombo->itemText(index);
   const QCamera::FocusMode focusMode = GetFocusMode(itemName);
   camera->setFocusMode(focusMode);

   const bool isManualMode = (itemName == "Manual");
   ui->labelFocusDistance->setEnabled(isManualMode);
   ui->focusSlider->setEnabled(isManualMode);
}

void MainWindow::OnFocusDistanceChanged(int value)
{
   camera->setFocusDistance(value / 100.f);
}

void MainWindow::OnFaceDetectionEnabled(bool checked)
{
   ui->aiAlgoCombo->setEnabled(checked);
   ui->algoControlsStack->setEnabled(checked);
}

void MainWindow::OnHandleVideoFrame(const QVideoFrame &frame)
{
   auto startT = std::chrono::high_resolution_clock::now();
   if (!frame.isValid())
      return;

   const QImage currentFrame = frame.toImage();
   ui->videoDisplayWidget->SetFrameImage(currentFrame);

   auto startTai = std::chrono::high_resolution_clock::now();
   if (ui->aiFaceBox->isChecked())
   {
      const std::vector<cv::Rect> faces = faceDetector->Detect(ToCvMat(currentFrame));
      ui->videoDisplayWidget->SetFaces(ToQRect(faces));
      SetErrorsToUi(faceDetector->GetErrors());
   }
   else
      ui->videoDisplayWidget->SetFaces(QList<QRect>());

   auto endT = std::chrono::high_resolution_clock::now();

   const double allTime = std::chrono::duration<double, std::milli>(endT - startT).count();
   const double aiTime = std::chrono::duration<double, std::milli>(endT - startTai).count();

   ui->profilingLabel->setText(QString::number(allTime) + " ms");
   ui->aiProfilingLabel->setText(QString::number(aiTime) + " ms");
}

void MainWindow::OnVideoSizeChange()
{
   const auto videoSize = videoSink.videoSize();
   if (videoSize.isValid())
   {
      ui->videoDisplayWidget->setMinimumSize(videoSize);
      ui->videoDisplayWidget->setMaximumSize(videoSize);
      ui->resolutionLabel->setText(QString::number(videoSize.width()) + "x" + QString::number(videoSize.height()));
   }
}

void MainWindow::OnHaarParametersChanged()
{
   auto haarDetector = static_cast<HaarFaceDetector*>(faceDetector);
   haarDetector->setParameters(ui->scaleFactorBox->value(), ui->neighbourBox->value(),
                               ui->minSizeBox->value(), ui->maxSizeBox->value());
}

void MainWindow::OnAiAlgoChanged(const QString &algorithm)
{
   const AiAlgorithmFeatures currentAiFeatures = GetAiFeatures(algorithm);

   ui->algoControlsStack->setCurrentWidget(currentAiFeatures.controlWidget);
   delete faceDetector;
   faceDetector = currentAiFeatures.algorithm;
   faceDetector->Initialize(currentAiFeatures.modelFiles);
   SetErrorsToUi(faceDetector->GetErrors());

   if (algorithm == "HaarCascades")
      OnHaarParametersChanged();
}

void MainWindow::OnClearErrors()
{
   ui->errorsBrowser->setPlainText("");
}

void MainWindow::OnKeepOnlyLastErrorChanged(bool checked)
{

}

void MainWindow::SetupUiSupportedFeatures()
{
   const QCamera::Features camFeatures = camera->supportedFeatures();

   ui->camFeatureColorBox->setChecked(camFeatures & QCamera::Feature::ColorTemperature);
   ui->camExposureBox->setChecked(camFeatures & QCamera::Feature::ExposureCompensation);
   ui->camIsoBox->setChecked(camFeatures & QCamera::Feature::IsoSensitivity);
   ui->camExposureTimeBox->setChecked(camFeatures & QCamera::Feature::ManualExposureTime);
   ui->camFocusPointBox->setChecked(camFeatures & QCamera::Feature::CustomFocusPoint);
   ui->camFocusDistanceBox->setChecked(camFeatures & QCamera::Feature::FocusDistance);

   if (camera->isFocusModeSupported(QCamera::FocusModeAuto))
      ui->focusModeCombo->addItem("Auto");
   if (camera->isFocusModeSupported(QCamera::FocusModeAutoNear))
      ui->focusModeCombo->addItem("Auto Near");
   if (camera->isFocusModeSupported(QCamera::FocusModeAutoFar))
      ui->focusModeCombo->addItem("Auto Far");
   if (camera->isFocusModeSupported(QCamera::FocusModeHyperfocal))
      ui->focusModeCombo->addItem("Hyperfocal");
   if (camera->isFocusModeSupported(QCamera::FocusModeInfinity))
      ui->focusModeCombo->addItem("Infinity");
   if (camera->isFocusModeSupported(QCamera::FocusModeManual))
      ui->focusModeCombo->addItem("Manual");
}

void MainWindow::SetErrorsToUi(const std::vector<std::string> &errors)
{
   std::string errorString;
   for (const auto& error : errors)
      errorString += error + "\n";
   if (ui->onlyLastFrameBox->isChecked())
      ui->errorsBrowser->setPlainText(errorString.data());
   else
      ui->errorsBrowser->append(errorString.data());
   ui->errorsGroupBox->setVisible(!errors.empty());
}

QCamera::FocusMode MainWindow::GetFocusMode(const QString &optionName) const
{
   if (optionName == "Auto")
      return QCamera::FocusModeAuto;
   else if (optionName == "Auto Near")
      return QCamera::FocusModeAutoNear;
   else if (optionName == "Auto Far")
      return QCamera::FocusModeAutoFar;
   else if (optionName == "Hyperfocal")
      return QCamera::FocusModeHyperfocal;
   else if (optionName == "Infinity")
      return QCamera::FocusModeInfinity;
   else // if (optionName == "Manual")
      return QCamera::FocusModeManual;
}

cv::Mat MainWindow::ToCvMat(const QImage &image) const
{
   switch (image.format())
   {
      case QImage::Format_RGB32:
      {
         cv::Mat bgra(image.height(), image.width(), CV_8UC4, const_cast<uchar*>(image.bits()), image.bytesPerLine());
         cv::Mat bgr;
         cv::cvtColor(bgra, bgr, cv::COLOR_BGRA2BGR);
         return bgr;
      }

      default:
         return cv::Mat();
   }
}

QList<QRect> MainWindow::ToQRect(const std::vector<cv::Rect> &box) const
{
   QList<QRect> faces;
   for (const auto& faceBox : box)
   {
      const QRect convertedBox(faceBox.x, faceBox.y, faceBox.width, faceBox.height);
      faces.append(convertedBox);
   }
   return faces;
}

AiAlgorithmFeatures MainWindow::GetAiFeatures(const QString &option) const
{
   AiAlgorithmFeatures features;
   if (option == "HaarCascades")
   {
      features.controlWidget = ui->pageHaar;
      features.algorithm = new HaarFaceDetector();
      features.modelFiles = {"/home/mickael/Prog/Toolboxes/ImageProcessingToolsLib/data/haarcascade_frontalface_default.xml"};
   }
   else // cnn
   {
      features.controlWidget = ui->pageMobileNetSSD;
      features.algorithm = new CnnFaceDetector();
      features.modelFiles = {"/home/mickael/Prog/Toolboxes/ImageProcessingToolsLib/data/deploy.prototxt",
                             "/home/mickael/Prog/Toolboxes/ImageProcessingToolsLib/data/res10_300x300_ssd_iter_140000.caffemodel"};
   }
   return features;
}
