#ifndef ELEMENT_H
#define ELEMENT_H

#include <QtWidgets>

class Element : public QLabel {
    Q_OBJECT

public:
    Element(QString name, int width, int height);
};

#endif // ELEMENT_H
