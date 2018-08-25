#ifndef ELEMENT_H
#define ELEMENT_H

#include <QtWidgets>

class Element : public QLabel {
    Q_OBJECT

public:
    Element(QString name, int width, int height);
    static const int GRID_SIZE = 16;
    QPointF floorToGrid(const QPointF& pointP);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);

protected:
    QHBoxLayout *content_layout;

private:
    QPoint offset;
};

#endif // ELEMENT_H
