#ifndef ELEMENT_H
#define ELEMENT_H

#include <QtWidgets>

#include <sol.hpp>

void ext_switch_window();

class Element : public QLabel {
    Q_OBJECT

public:
    Element(QWidget *parent, QString name);
    static const int GRID_SIZE;
    static const int DEFAULT_ELEMENT_SIZE;
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    QLabel *content;
    sol::state state;
    sol::protected_function safe_onleftclick;
    sol::protected_function safe_onupdate;

protected:
    QHBoxLayout *content_layout;
    void renderLuaError();
    void renderNetworkError();

private:
    QPoint offset;
    QPushButton *refresh;
    QLabel *loader;
    bool canBeUpdated;

public slots:
    void update();

protected slots:
    void refreshButtonClick();
};

#endif // ELEMENT_H
