#ifndef ELEMENT_H
#define ELEMENT_H

#include <QtWidgets>

class Element : public QLabel {
    Q_OBJECT

public:
    Element(QString name, int width, int height);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);

private:
    QPoint offset;
};

#endif // ELEMENT_H
