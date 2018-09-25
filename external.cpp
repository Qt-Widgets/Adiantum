#include "adiantum.h"

void ext_switch_window() {
    Adiantum::getInstance()->switchWindow();
}

std::string ext_network_request(std::string url) {
    return Adiantum::getInstance()->networkRequest(QString::fromStdString(url)).toStdString();
}

std::string readFile(QString path) {
    QString data;
    QFile file(path);
    if(file.open(QIODevice::ReadOnly)) {
        data = file.readAll();
    }
    file.close();
    return data.toStdString();
}


QPointF floorToGrid(const QPointF& point, int grid_size) {
    qreal xV = floor(point.x()/grid_size)*grid_size;
    qreal yV = floor(point.y()/grid_size)*grid_size;
    return QPointF(xV, yV);
}
