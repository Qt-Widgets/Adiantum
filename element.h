#ifndef ELEMENT_H
#define ELEMENT_H

#include <QtWidgets>

#include <sol.hpp>

class Element : public QLabel {
    Q_OBJECT

public:
    Element(QString name, int width, int height);
    static const int GRID_SIZE = 16;
    QPointF floorToGrid(const QPointF& pointP);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    QLabel *content;
    sol::state state;
    sol::protected_function safe_onleftclick;

protected:
    QHBoxLayout *content_layout;

private:
    QPoint offset;
};

#endif // ELEMENT_H
