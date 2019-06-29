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

Element::Element(QWidget *parent, QString name, QPoint position) {
    this->setParent(parent);
    this->setObjectName(name);
    this->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    content_layout = new QHBoxLayout(this);
    content_layout->setMargin(0);
    content = new QLabel(this);

    /* load Lua libraries and external Adiantum functions */
    state.open_libraries(sol::lib::base);
    state.set_function("ext_switch_window", External::switch_window);
    state.set_function("ext_network_request", External::network_request);
    state.set_function("ext_get_network_interfaces", External::get_network_interfaces);
    state.set_function("ext_read_file", External::read_file);
    state.set_function("ext_execute_process", External::execute_process);

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

    /* flag loaderEnabled used to control preloader creation */
    auto a_loaderEnabled = state["config"]["loader_enabled"];
    loaderEnabled = (a_loaderEnabled.valid()) ? a_loaderEnabled.get<bool>() : true;

    /* flag manualRefreshEnabled used to control refresh button creation */
    auto a_manualRefreshEnabled = state["config"]["manual_refresh_enabled"];
    manualRefreshEnabled = (a_manualRefreshEnabled.valid()) ? a_manualRefreshEnabled.get<bool>() : true;

    /* reading Lua config table */
    int width = state["config"]["w"].get_or(DEFAULT_ELEMENT_SIZE);
    int height = state["config"]["h"].get_or(DEFAULT_ELEMENT_SIZE);
    this->move(position);

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

    corner = new QLabel(this);
    corner->setPixmap(QPixmap(QCoreApplication::applicationDirPath()+"/res/images/default/corner.png"));
    corner->setFixedSize(corner->pixmap()->size());
    corner->setAttribute(Qt::WA_TranslucentBackground);
    corner->move(this->width() - corner->width(), 0);

    if (canBeUpdated) {
        if (loaderEnabled) {
            loader = new QLabel(this);
            QMovie *movie = new QMovie(QCoreApplication::applicationDirPath()+"/res/images/default/loader.gif");
            movie->setScaledSize(QSize(DEFAULT_ELEMENT_SIZE, DEFAULT_ELEMENT_SIZE));
            movie->setSpeed(200);
            loader->setFixedSize(QSize(this->width(), this->height()));
            loader->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            loader->setAttribute(Qt::WA_TranslucentBackground);
            loader->setMovie(movie);
        }
        if (manualRefreshEnabled) {
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
            refresh_error_label = new QLabel(this);
            refresh_error_label->setPixmap(QPixmap(QCoreApplication::applicationDirPath()+"/res/images/default/warning.png"));
            refresh_error_label->setFixedSize(refresh_error_label->pixmap()->size());
            refresh_error_label->setAttribute(Qt::WA_TranslucentBackground);
            refresh_error_label->move(this->width() - refresh->width() - refresh_error_label->width(), this->height() - refresh_error_label->height());
            refresh_error_label->hide();
        }
    }

    auto a_chartEnabled = state["config"]["chart_enabled"];
    if ((a_chartEnabled.valid()) ? a_chartEnabled.get<bool>() : false) {
        this->createChart();
    }

    this->show();
}

void Element::update() {
    if (canBeUpdated) {
        if (loaderEnabled) {
            loader->show();
            loader->movie()->start();
        }
        if (manualRefreshEnabled) {
            refresh->hide();
            refresh_error_label->hide();
        }
        auto auto_result = safe_onupdate();
        std::string result = auto_result;
        if(auto_result.valid()) {
            QString qresult = QString::fromStdString(result);

            if (chartView != nullptr) {
                this->updateChart();
            }

            if (qresult == "NETWORK_ERROR_FLAG") {
                if (!this->contentAcquired) {
                    this->renderNetworkError();
                } else {
                    if (manualRefreshEnabled) {
                        refresh_error_label->show();
                    }
                }
                qDebug() << "[Error] Element "+this->objectName()+" +returned network error";
            } else {
                this->contentAcquired = true;
                content->setText("<html></html>");
                qresult = qresult.replace("%APP_DIR%", QCoreApplication::applicationDirPath());
                content->setText(qresult);
                //qDebug() << "[Update] Element "+this->objectName()+" : "+result;
            }
            int width = state["config"]["w"].get_or(DEFAULT_ELEMENT_SIZE);
            int height = state["config"]["h"].get_or(DEFAULT_ELEMENT_SIZE);
            if (width != this->width()) {
                this->setFixedWidth(width);
                content->setFixedWidth(width);
                corner->move(this->width() - corner->width(), 0);
                if (manualRefreshEnabled) refresh->move(this->width() - refresh->width(), this->height() - refresh->height());
            }
            if (height != this->height()) {
                this->setFixedHeight(height);
                content->setFixedHeight(height);
                if (manualRefreshEnabled) refresh->move(this->width() - refresh->width(), this->height() - refresh->height());
            }
        } else {
            this->renderLuaError();
            qDebug() << "[Error] Element "+this->objectName()+" : "+QString::fromStdString(result);
        }
        if (loaderEnabled) {
            loader->hide();
            loader->movie()->stop();
        }
        if (manualRefreshEnabled) {
            refresh->show();
        }
    }
}

void Element::createChart() {
    QChart *chart = new QChart();
    QLineSeries *series0 = new QLineSeries();
    *series0 << QPointF(0, 0);
    chart->addSeries(new QAreaSeries(series0));
    chart->createDefaultAxes();
    chart->setBackgroundVisible(false);
    chart->setPlotAreaBackgroundVisible(false);
    chart->legend()->hide();
    chart->layout()->setContentsMargins(0, 0, 0, 0);
    chart->setMargins({0, 0, 0, 0});
    chart->setBackgroundRoundness(0);
    chart->setAttribute(Qt::WA_NoSystemBackground);
    chart->setPlotArea(QRect(0, 0, this->width(), this->height()));

    QAbstractAxis *axisx = chart->axes(Qt::Horizontal).takeAt(0);
    QAbstractAxis *axisy = chart->axes(Qt::Vertical).takeAt(0);
    axisx->setGridLinePen(QPen(QColor(255,255,255,64)));
    axisy->setGridLinePen(QPen(QColor(255,255,255,64)));
    axisx->setLinePen(QPen(QColor(255,255,255,64)));
    axisy->setLinePen(QPen(QColor(255,255,255,64)));
    axisy->setShadesVisible(false);
    axisy->setLabelsVisible(false);
    axisy->setGridLineVisible(false);
    axisx->setLabelsVisible(false);
    axisx->setGridLineVisible(false);  //true
    axisx->setMinorGridLineVisible(false);  //true

    chartView = new QChartView(chart);
    chartView->setParent(this);
    chartView->move(QPoint(0, 0));
    chartView->setFixedSize(QSize(this->width(), this->height()));
    chartView->setRenderHint(QPainter::Antialiasing);
    chartView->setAttribute(Qt::WA_NoSystemBackground);
    chartView->setAttribute(Qt::WA_TransparentForMouseEvents);
    chartView->setStyleSheet("background-color: rgba(0,0,0,0)");
    chartView->show();
}

void Element::updateChart() {
    sol::optional<sol::table> series_string = state["config"]["chart_series"];
    if (series_string->size() > 0) {
        QLineSeries *seriesx = new QLineSeries();
        for (const auto& key_value_pair : *series_string ) {
             //sol::object key = key_value_pair.first;
             sol::table value = key_value_pair.second;
             seriesx->append(value[1].get<qreal>(), value[2].get<qreal>());
        }
        this->updateChartSeries(seriesx);
    }
}

void Element::updateChartSeries(QLineSeries *series) {
    qreal xmax = 0, xmin = 0, ymax = 0, ymin = 0;
    for (int i=0; i<series->count(); i++) {
        qreal x = series->at(i).x();
        qreal y = series->at(i).y();
        if(y < ymin || y > ymax){
            if(y < ymin) ymin = y;
            if(y > ymax) ymax = y;
        }
        if(x < xmin || x > xmax){
            if(x < xmin) xmin = x;
            if(x > xmax) xmax = x;
        }
    }
    chartView->chart()->axes(Qt::Horizontal).takeAt(0)->setRange(xmin, xmax);
    chartView->chart()->axes(Qt::Vertical).takeAt(0)->setRange(ymin, ymax);

    QAreaSeries *aseries = new QAreaSeries(series);
    QPen pen(0x059605);
    pen.setWidth(1);
    aseries->setPen(pen);
    aseries->setBrush(QBrush(QColor(5, 155, 5, 128)));
    chartView->chart()->removeAllSeries();
    chartView->chart()->addSeries(aseries);
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

void Element::mouseReleaseEvent(QMouseEvent *event) {
    Adiantum::getInstance()->saveElements();
}
