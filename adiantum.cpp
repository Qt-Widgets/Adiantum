#include "adiantum.h"
#include "element.h"
#include "windows.h"

Adiantum::Adiantum(QWidget *parent) : QMainWindow(parent) {
    this->setStyleSheet("background:black;");
    this->setWindowFlags(Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint);
    if(!RegisterHotKey(HWND(winId()), 0, 0, 0x6B)) {
        throw("Can’t register hotkey");
    }
    this->showFullScreen();
    Element *element = new Element("test", 64, 64);
    element->setParent(this);
    element->move(200,200);
    element->show();
}

bool Adiantum::nativeEvent(const QByteArray &eventType, void *message, long *result) {
    Q_UNUSED(eventType);
    Q_UNUSED(result);
    MSG *msg = static_cast<MSG*>(message);
    if(msg->message == WM_HOTKEY) {
        this->active = !this->active;
        if (this->active) {
            this->show();
        } else {
            this->hide();
        }
        return true;
    }
    return false;
}

Adiantum::~Adiantum() {}
