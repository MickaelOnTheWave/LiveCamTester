#include "MainWindow.h"
#include "./ui_MainWindow.h"

#include <QCamera>
#include <QVideoWidget>
#include <QMediaDevices>

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

   setupUiSupportedFeatures();

   connect(ui->startCamButton, &QPushButton::clicked, this, &MainWindow::OnStartCamera);
   connect(ui->stopCamButton, &QPushButton::clicked, this, &MainWindow::OnStopCamera);
   connect(ui->focusModeCombo, &QComboBox::currentIndexChanged, this, &MainWindow::OnFocusModeChange);
   connect(ui->focusSlider, &QSlider::valueChanged, this, &MainWindow::OnFocusDistanceChanged);
}

MainWindow::~MainWindow()
{
   delete ui;
   delete camera;
}

void MainWindow::OnStartCamera()
{
   captureSession.setVideoOutput(ui->videoDisplayWidget);
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

void MainWindow::setupUiSupportedFeatures()
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
