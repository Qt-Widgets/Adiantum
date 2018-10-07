#include "element.h"
#include "adiantum.h"
#include "external.h"

extern "C" {
    #include "./lib/lua/include/lua.h"
    #include "./lib/lua/include/lauxlib.h"
    #include "./lib/lua/include/lualib.h"
}

#include <sol.hpp>

const int Element::GRID_SIZE = 16;
const int Element::DEFAULT_ELEMENT_SIZE = 64;

Element::Element(QWidget *parent, QString name) {
    this->setParent(parent);
    this->setObjectName(name);
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

    /* load Lua libraries and external Adiantum functions */
    state.open_libraries(sol::lib::base);
    state.set_function("ext_switch_window", External::switch_window);
    state.set_function("ext_network_request", External::network_request);
    state.set_function("ext_read_file", External::read_file);

    /* open Lua script with same name as this instance */
    try {
        state.safe_script(External::read_file((QCoreApplication::applicationDirPath()+"/scripts/"+name+".lua").toStdString()));
    } catch( const sol::error& e ) {
        this->renderLuaError();
        qDebug() << "[Error] Element "+this->objectName()+" : "+e.what();
    }

    /* load Lua modules defined in "modules" table */
    sol::table modules = state["config"]["modules"].get_or(sol::table(state, sol::create));
    modules.for_each([&](const sol::object&, const sol::object& value) {
        std::string module_name = state["tostring"](value);
        /* reserved names for default Lua libraries */
        if (module_name == "package") { state.open_libraries(sol::lib::package); return; }
        if (module_name == "coroutine") { state.open_libraries(sol::lib::coroutine); return; }
        if (module_name == "string") { state.open_libraries(sol::lib::string); return; }
        if (module_name == "os") { state.open_libraries(sol::lib::os); return; }
        if (module_name == "math") { state.open_libraries(sol::lib::math); return; }
        if (module_name == "table") { state.open_libraries(sol::lib::table); return; }
        if (module_name == "debug") { state.open_libraries(sol::lib::debug); return; }
        if (module_name == "bit32") { state.open_libraries(sol::lib::bit32); return; }
        if (module_name == "io") { state.open_libraries(sol::lib::io); return; }
        if (module_name == "ffi") { state.open_libraries(sol::lib::ffi); return; }
        if (module_name == "jit") { state.open_libraries(sol::lib::jit); return; }
        if (module_name == "utf8") { state.open_libraries(sol::lib::utf8); return; }
        QString module_path(QCoreApplication::applicationDirPath()+"/scripts/modules/"+QString::fromStdString(module_name)+".lua");
        if (QFile::exists(module_path)) {
            state.require_script(module_name, External::read_file(module_path.toStdString()));
        } else {
            qDebug() << "[Warning] Element "+this->objectName()+": cannot open module " +module_path;
        }
    });

    /* register Lua widget functions */
    safe_onleftclick = state["onLeftClick"];
    safe_onupdate = state["onUpdate"];

    /* flag canBeUpdated used to define visibility of refresh button */
    auto auto_onupdate = state["onUpdate"];
    canBeUpdated = auto_onupdate.valid();

    /* reading Lua config table */
    int width = state["config"]["w"].get_or(DEFAULT_ELEMENT_SIZE);
    int height = state["config"]["h"].get_or(DEFAULT_ELEMENT_SIZE);
    int x = state["config"]["x"].get_or(0);
    int y = state["config"]["y"].get_or(0);
    this->move(QPoint(x,y));

    int interval = state["config"]["update_interval"].get_or(0);
    if (interval > 0) {
        QTimer *timer = new QTimer(this);
        connect(timer, SIGNAL(timeout()), this, SLOT(update()));
        timer->start(interval);
        QTimer *single = new QTimer(this);
        connect(single, SIGNAL(timeout()), this, SLOT(update()));
        single->setSingleShot(true);
        single->start(200);
    }

    std::string icon = state["config"]["icon"].get_or(std::string(""));
    setPixmap(QPixmap(QString::fromStdString(icon).replace("%APP_DIR%", QCoreApplication::applicationDirPath())));

    this->setFixedWidth(width);
    this->setFixedHeight(height);
    content->setFixedWidth(width);
    content->setFixedHeight(height);
    content->setAttribute(Qt::WA_TranslucentBackground);
    content->setAttribute(Qt::WA_TransparentForMouseEvents);
    content->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    content->setTextFormat(Qt::RichText);
    content_layout->addWidget(content);

    if (canBeUpdated) {
        loader = new QLabel(this);
        QMovie *movie = new QMovie(QCoreApplication::applicationDirPath()+"/res/images/default/loader.gif");
        movie->setScaledSize(QSize(DEFAULT_ELEMENT_SIZE, DEFAULT_ELEMENT_SIZE));
        movie->setSpeed(200);
        loader->setFixedSize(QSize(this->width(), this->height()));
        loader->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        loader->setAttribute(Qt::WA_TranslucentBackground);
        loader->setMovie(movie);
        refresh = new QPushButton(this);
        refresh->setIcon(QIcon(QCoreApplication::applicationDirPath()+"/res/images/default/refresh.png"));
        refresh->setFixedSize(refresh->iconSize());
        refresh->setObjectName("refresh");
        refresh->setStyleSheet(
                    "QPushButton#refresh {\
                        background: transparent;\
                        border: none;\
                    }\
                    QPushButton#refresh:hover {\
                        background: rgba(255,255,255,0.2);\
                    }");

        refresh->move(this->width() - refresh->width(), this->height() - refresh->height());
        connect(refresh, SIGNAL(clicked()), this, SLOT (refreshButtonClick()));
    }

    this->show();
}

void Element::update() {
    if (canBeUpdated) {
        loader->show();
        loader->movie()->start();
        refresh->hide();
        content->setText("<html></html>");
        auto auto_result = safe_onupdate();
        std::string result = auto_result;
        if(auto_result.valid()) {
            QString qresult = QString::fromStdString(result);
            if (qresult == "NETWORK_ERROR_FLAG") {
                this->renderNetworkError();
                qDebug() << "[Error] Element "+this->objectName()+" : Network error";
            } else {
                qresult = qresult.replace("%APP_DIR%", QCoreApplication::applicationDirPath());
                content->setText(qresult);
                //qDebug() << "[Update] Element "+this->objectName()+" : "+result;
            }
        } else {
            this->renderLuaError();
            qDebug() << "[Error] Element "+this->objectName()+" : "+QString::fromStdString(result);
        }
        loader->hide();
        loader->movie()->stop();
        refresh->show();
    }
}

void Element::refreshButtonClick() {
    this->update();
}

void Element::mousePressEvent(QMouseEvent *event) {
    if(event->buttons() & Qt::RightButton) {
        offset = event->pos();
    }
    if(event->buttons() & Qt::LeftButton) {
        safe_onleftclick();
    }
}

void Element::renderLuaError() {
    content->setText("<html><img src='"+QCoreApplication::applicationDirPath()+"/res/images/default/lua_error.png'></html>");
}

void Element::renderNetworkError() {
    content->setText("<html><img src='"+QCoreApplication::applicationDirPath()+"/res/images/default/network_error.png'></html>");
}

void Element::mouseMoveEvent(QMouseEvent *event) {
    if(event->buttons() & Qt::RightButton) {
        qreal xV = floor((event->pos() - offset).x()/GRID_SIZE)*GRID_SIZE;
        qreal yV = floor((event->pos() - offset).y()/GRID_SIZE)*GRID_SIZE;
        this->move(mapToParent(QPointF(xV, yV).toPoint()));
    }
}
