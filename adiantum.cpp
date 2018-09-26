#include "adiantum.h"
#include "element.h"
#include "windows.h"

Adiantum* Adiantum::instance = NULL;

Adiantum* Adiantum::getInstance() {
    if (Adiantum::instance == NULL) {
        Adiantum::instance = new Adiantum(0);
    }
    return Adiantum::instance;
}

Adiantum::Adiantum(QWidget *parent) : QMainWindow(parent) {
    this->setObjectName("adiantum");
    this->setStyleSheet("#adiantum {background:black;}");
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

void Adiantum::update() {
    QMapIterator<QString, Element*> i(elements);
    while (i.hasNext()) {
        i.next();
        i.value()->update();
    }
}

QString Adiantum::networkRequest(QString url) {
    QNetworkRequest request(url);
    QNetworkReply* reply = networkManager->get(request);
    QString response = "";
    QEventLoop loop;
    connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
    loop.exec();
    if (reply->error() == QNetworkReply::NoError) {
        QByteArray bytes = reply->readAll();
        response = QString::fromUtf8(bytes.data(), bytes.size());
    } else {
        response = "ERROR";
    }
    reply->deleteLater();
    return response;
}

void Adiantum::openAboutWindow() {
    QMessageBox::about(this, "About", "Adiantum\nQT desktop widget layer\nAuthor: Andrew Chernishov (https://github.com/Screwdr1ver/)\nIcons by https://icons8.com/");
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
