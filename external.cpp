#include "external.h"
#include "adiantum.h"

void External::switch_window() {
    Adiantum::getInstance()->switchWindow();
}

std::string External::network_request(std::string url) {
    return Adiantum::getInstance()->networkRequest(QString::fromStdString(url)).toStdString();
}

std::string External::get_network_interfaces() {
    return Adiantum::getInstance()->getNetworkInterfaces();
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

std::string External::execute_process(std::string path) {
    QProcess *process = new QProcess;
    QEventLoop loop;
    connect(process, SIGNAL(finished(int , QProcess::ExitStatus)), &loop, SLOT(quit()));
    process->start(QString::fromStdString(path));
    loop.exec();
    return QString::fromLocal8Bit(process->readAllStandardOutput()).toStdString();
}

