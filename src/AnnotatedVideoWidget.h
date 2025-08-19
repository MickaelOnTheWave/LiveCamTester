#ifndef ANNOTATEDVIDEOWIDGET_H
#define ANNOTATEDVIDEOWIDGET_H

#include <QWidget>

class AnnotatedVideoWidget : public QWidget
{
   Q_OBJECT
public:
   explicit AnnotatedVideoWidget(QWidget *parent = nullptr);

   void SetFrameImage(const QImage& image);
   void SetFaces(const QList<QRect>& _faces);

signals:

protected:
   void paintEvent(QPaintEvent *) override;

private:
   void drawBoundingVolume(const QPoint& p1, const QPoint& p2);

   QImage currentFrame;
   QList<QRect> faces;
};

#endif // ANNOTATEDVIDEOWIDGET_H
