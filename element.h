#ifndef ELEMENT_H
#define ELEMENT_H

#include <QtWidgets>

typedef std::function<void()> Callable;

class Element : public QLabel {
    Q_OBJECT

public:
    Element(QString name, int width, int height);
    static const int GRID_SIZE = 16;
    QPointF floorToGrid(const QPointF& pointP);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    Callable onLeftClickFunction;

protected:
    QHBoxLayout *content_layout;
    QLabel *content;

private:
    QPoint offset;
};

#endif // ELEMENT_H
