#include "element.h"
#include "adiantum.h"

extern "C" {
    #include "./lib/lua/include/lua.h"
    #include "./lib/lua/include/lauxlib.h"
    #include "./lib/lua/include/lualib.h"
}

#include <sol.hpp>

void ext_switch_window() {
    Adiantum::getInstance()->switchWindow();
}

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
    content_layout = new QHBoxLayout(this);
    content_layout->setMargin(0);
    content = new QLabel(this);
    content->setFixedWidth(width);
    content->setFixedHeight(height);
    content->setAttribute(Qt::WA_TranslucentBackground);
    content->setAttribute(Qt::WA_TransparentForMouseEvents);
    content->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    content->setTextFormat(Qt::RichText);
    content_layout->addWidget(content);

    state.open_libraries(sol::lib::os, sol::lib::base, sol::lib::string, sol::lib::math, sol::lib::utf8);
    state.set_function("adiantum_switch_window", ext_switch_window);

    try {
        state.safe_script(readFile(":/res/scripts/script.lua"));
    } catch( const sol::error& e ) {
        this->renderLuaError();
        qDebug() << e.what();
    }

    sol::table modules = state["modules"].get_or(sol::table(state, sol::create));
    modules.for_each([&](const sol::object& key, const sol::object& value) {
        std::string module_name = state["tostring"](value);
        QString module_path(":/res/scripts/"+QString::fromStdString(module_name)+".lua");
        if (QFile::exists(module_path)) {
            state.require_script(module_name, readFile(module_path));
        } else {
            qDebug() << "Warning: cannot open module " +module_path;
        }
    });

    safe_onleftclick = state["onLeftClick"];
}

void Element::mousePressEvent(QMouseEvent *event) {
    if(event->buttons() & Qt::RightButton) {
        offset = event->pos();
    }
    if(event->buttons() & Qt::LeftButton) {
        safe_onleftclick();
    }
}

std::string Element::readFile(QString path) {
    QString data;
    QFile file(path);
    if(file.open(QIODevice::ReadOnly)) {
        data = file.readAll();
    }
    file.close();
    return data.toStdString();
}

void Element::renderLuaError() {
    content->setText("<html><img src=':/res/images/error.png'></html>");
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
