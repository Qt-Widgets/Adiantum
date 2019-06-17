#include "adiantum.h"
#include "element.h"
#include "windows.h"

struct NetworkInterfaceData {
   QString name;
   QString mac;
   QString address;
   QString netmask;
   QString type;
   NetworkInterfaceData(QString name, QString mac, QString address, QString netmask, QString type) {
       this->name = name;
       this->mac = mac;
       this->address = address;
       this->netmask = netmask;
       this->type = type;
   }
   QJsonObject toJson() const {
       return {{"name", this->name}, {"mac", this->mac}, {"address", this->address}, {"netmask", this->netmask}, {"type", this->type}};
   }
};

Adiantum* Adiantum::instance = nullptr;

Adiantum* Adiantum::getInstance() {
    if (Adiantum::instance == nullptr) {
        Adiantum::instance = new Adiantum(nullptr);
    }
    return Adiantum::instance;
}

Adiantum::Adiantum(QWidget *parent) : QMainWindow(parent) {
    this->setObjectName("adiantum");
    QFile File(QCoreApplication::applicationDirPath()+"/res/styles/global.css");
    File.open(QFile::ReadOnly);
    this->setStyleSheet(QLatin1String(File.readAll()));
    File.close();
    this->setWindowIcon(QIcon(QPixmap(QCoreApplication::applicationDirPath()+"/res/images/default/app_icon.png")));
    this->setWindowFlags(Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint);
    if(!RegisterHotKey(HWND(winId()), 0, 0, 0x6B)) {
        throw("Can’t register hotkey");
    }

    networkManager = new QNetworkAccessManager();
    QSslConfiguration::defaultConfiguration();

    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setToolTip("Adiantum");

    trayIconMenu = new QMenu(this);

    QAction *aboutAction = new QAction("&About", this);
    connect(aboutAction, SIGNAL(triggered()), this, SLOT(openAboutWindow()));
    trayIconMenu->addAction(aboutAction);
    QAction *quitAction = new QAction("&Quit", this);
    connect(quitAction, SIGNAL(triggered()), this, SLOT(closeApp()));
    trayIconMenu->addAction(quitAction);

    trayIcon->setContextMenu(trayIconMenu);
    trayIcon->setIcon(QIcon(QPixmap(QCoreApplication::applicationDirPath()+"/res/images/default/tray_icon.png")));
    trayIcon->show();

    this->showFullScreen();
}

void Adiantum::loadElements() {
    QStringList files = QDir(QCoreApplication::applicationDirPath()+"/scripts").entryList(QStringList() << "*.lua", QDir::Files);
    foreach (QString filename, files) {
        QString name = filename.split(".",QString::SkipEmptyParts).at(0);
        elements.insert(name, new Element(this, name));
    }
}

std::string Adiantum::getNetworkInterfaces() {
    QList<NetworkInterfaceData> interfaces;
    foreach (QNetworkInterface interface, QNetworkInterface::allInterfaces()) {
        if (interface.flags().testFlag(QNetworkInterface::IsLoopBack)) continue;
        if (!interface.flags().testFlag(QNetworkInterface::IsRunning)) continue;
        QList<QNetworkAddressEntry> entries = interface.addressEntries();
        if (!entries.isEmpty()) {
            QNetworkAddressEntry entry = entries.first();
            QString type = "";
            switch (QString::number(interface.type()).toInt()) {
                case 8: type = "wireless"; break;
                case 3: type = "ethernet"; break;
                default: type = "default";
            }
            interfaces.append(NetworkInterfaceData(
                                  interface.humanReadableName(),
                                  interface.hardwareAddress(),
                                  entry.ip().toString(),
                                  entry.netmask().toString(),
                                  type
            ));
        }
    }
    QJsonArray jsonarray;
    for (auto & item : interfaces) {
        jsonarray.append(item.toJson());
    }
    return std::string(QJsonDocument(jsonarray).toJson().constData());
}

QString Adiantum::networkRequest(QString url) {
    QNetworkRequest request(url);
    QNetworkReply* reply = networkManager->get(request);
    QString response = "";

    QTimer timer;
    timer.setSingleShot(true);

    QEventLoop loop;
    connect(&timer, SIGNAL(timeout()), &loop, SLOT(quit()));
    connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
    timer.start(5000); // 5 secs timeout
    loop.exec();

    if(timer.isActive()) {
        timer.stop();
        if(reply->error() > 0) { // server returned error
            response = "NETWORK_ERROR_FLAG";
        }
        else {
            int v = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
            if (v >= 200 && v < 300) { // server responded with no errors
                QByteArray bytes = reply->readAll();
                response = QString::fromUtf8(bytes.data(), bytes.size());
            }
        }
    } else { // timeout
        response = "NETWORK_ERROR_FLAG";
        disconnect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
        reply->abort();
    }

    reply->deleteLater();
    return response;
}

void Adiantum::openAboutWindow() {
    QMessageBox::about(this, "About", "Adiantum\nQT desktop widget layer\nAuthor: Andrey Chernishov (https://github.com/Screwdr1ver/)\nIcons by https://icons8.com/");
}

void Adiantum::closeApp() {
    QApplication::quit();
}

void Adiantum::switchWindow() {
    if (this->isVisible()) {
        this->hide();
    } else {
        this->show();
    }
}

void Adiantum::closeEvent(QCloseEvent *e) {
    trayIcon->hide();
    e->accept();
}

bool Adiantum::nativeEvent(const QByteArray &eventType, void *message, long *result) {
    Q_UNUSED(eventType);
    Q_UNUSED(result);
    MSG *msg = static_cast<MSG*>(message);
    if(msg->message == WM_HOTKEY) {
        this->switchWindow();
        return true;
    }
    return false;
}

Adiantum::~Adiantum() {}
