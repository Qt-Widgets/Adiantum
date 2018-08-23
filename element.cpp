#include "element.h"

Element::Element(QString name, int width, int height) {
    this->setObjectName(name);
    this->setFixedWidth(width);
    this->setFixedHeight(height);
    this->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    this->setStyleSheet(
        "#"+name+" {\
            background: rgba(255,255,255,0.1);\
        }\
        #"+name+":hover {\
            background: rgba(255,255,255,0.2);\
        }");
}

void Element::mousePressEvent(QMouseEvent *event) {
    if(event->buttons() & Qt::RightButton) {
        offset = event->pos();
    }
}

void Element::mouseMoveEvent(QMouseEvent *event) {
    if(event->buttons() & Qt::RightButton) {
        this->move(mapToParent(floorToGrid(event->pos() - offset).toPoint()));
    }
}

QPointF Element::floorToGrid(const QPointF& pointP) {
    qreal xV = floor(pointP.x()/GRID_SIZE)*GRID_SIZE;
    qreal yV = floor(pointP.y()/GRID_SIZE)*GRID_SIZE;
    return QPointF(xV, yV);
}
