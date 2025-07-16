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

   connect(ui->startCamButton, &QPushButton::clicked, this, &MainWindow::onStartCamera);
   connect(ui->stopCamButton, &QPushButton::clicked, this, &MainWindow::onStopCamera);
}

MainWindow::~MainWindow()
{
   delete ui;
   delete camera;
}

void MainWindow::onStartCamera()
{
   captureSession.setVideoOutput(ui->videoDisplayWidget);
   camera->start();
}

void MainWindow::onStopCamera()
{
   camera->stop();
}
