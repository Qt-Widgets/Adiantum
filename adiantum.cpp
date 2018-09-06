#include "adiantum.h"
#include "element.h"
#include "webloader.h"
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
    this->setWindowIcon(QIcon(QPixmap(":/res/images/app_icon.png")));
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
    trayIcon->setIcon(QIcon(QPixmap(":/res/images/tray_icon.png")));
    trayIcon->show();

    this->showFullScreen();

    Element *element = new Element("test", 64, 64);
    element->onLeftClick.func = [](){
        Adiantum::getInstance()->executeCommand("cmd.exe");
    };
    element->setPixmap(QPixmap(":/res/images/cmd.png"));
    element->setParent(this);
    element->move(200,200);
    element->show();

    Webloader *webloader = new Webloader("test", "https://jsonplaceholder.typicode.com/todos/1", 64, 64);
    webloader->setParent(this);
    webloader->move(264,200);
    webloader->update();
    webloader->show();
    webloader->onLoad.func = [](Webloader* webloader, QString s) { webloader->content->setText(s); };

    Webloader *webloader2 = new Webloader("test2", "http://invalidurl", 64, 64);
    webloader2->setParent(this);
    webloader2->move(328,200);
    webloader2->update();
    webloader2->show();
}

QNetworkAccessManager* Adiantum::getNAM() {
    return networkManager;
}

void Adiantum::openAboutWindow() {
    QMessageBox::about(this, "About", "Adiantum\nQT desktop widget layer\nAuthor: Andrew Chernishov (https://github.com/Screwdr1ver/)\nIcons by https://icons8.com/");
}

void Adiantum::closeApp() {
    QApplication::quit();
}

void Adiantum::executeCommand(QString command) {
    ShellExecute(0, 0, reinterpret_cast<const WCHAR*>(command.utf16()), 0, 0, SW_NORMAL);
    switchWindow();
}

void Adiantum::switchWindow() {
    if (this->isVisible()) {
        this->hide();
    } else {
        this->show();
    }
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
