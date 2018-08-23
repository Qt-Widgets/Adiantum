#include "adiantum.h"

Adiantum::Adiantum(QWidget *parent) : QMainWindow(parent) {
    this->setStyleSheet("background:black;");
    this->setWindowFlags(Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint);
}

Adiantum::~Adiantum() {}
