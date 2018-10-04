#include "external.h"
#include "adiantum.h"

void External::switch_window() {
    Adiantum::getInstance()->switchWindow();
}

std::string External::network_request(std::string url) {
    return Adiantum::getInstance()->networkRequest(QString::fromStdString(url)).toStdString();
}

std::string External::read_file(std::string path) {
    QString data;
    QFile file(QString::fromStdString(path));
    if(file.open(QIODevice::ReadOnly)) {
        data = file.readAll();
    }
    file.close();
    return data.toStdString();
}
