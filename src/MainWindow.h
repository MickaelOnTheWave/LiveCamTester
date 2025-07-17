#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QCamera>
#include <QMediaCaptureSession>

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

private:
   void setupUiSupportedFeatures();
   QCamera::FocusMode GetFocusMode(const QString& optionName) const;

   Ui::MainWindow *ui;
   QCamera* camera = nullptr;
   QMediaCaptureSession captureSession;
};
#endif // MAINWINDOW_H
