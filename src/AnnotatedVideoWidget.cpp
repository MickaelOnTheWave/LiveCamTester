#include "AnnotatedVideoWidget.h"

#include <QPainter>

AnnotatedVideoWidget::AnnotatedVideoWidget(
    QWidget *parent)
  : QWidget{parent}
{}

void AnnotatedVideoWidget::SetFrameImage(const QImage &image)
{
   currentFrame = image;
   update();
}

void AnnotatedVideoWidget::SetFaces(const QList<QRect>& _faces)
{
   faces = _faces;
}

void AnnotatedVideoWidget::paintEvent(QPaintEvent *event)
{
   QPainter painter(this);
   if (!currentFrame.isNull())
      painter.drawImage(rect(), currentFrame);
   else
      painter.fillRect(rect(), Qt::black);

   for (const QRect& face : std::as_const(faces))
      drawBoundingVolume(face.bottomLeft(), face.topRight());
}

void AnnotatedVideoWidget::drawBoundingVolume(const QPoint &p1, const QPoint &p2)
{
   const QPoint p3(p2.x(), p1.y());
   const QPoint p4(p1.x(), p2.y());

   QPainter painter(this);
   painter.setPen(Qt::red);
   painter.drawLine(p1, p3);
   painter.drawLine(p3, p2);
   painter.drawLine(p2, p4);
   painter.drawLine(p4, p1);
}
