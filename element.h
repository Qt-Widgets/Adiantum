#ifndef ELEMENT_H
#define ELEMENT_H

#include <QtWidgets>
#include <QtCharts>

#include <sol.hpp>

void ext_switch_window();

class Element : public QLabel {
    Q_OBJECT

public:
    Element(QWidget *parent, QString name, QPoint position);
    static const int GRID_SIZE;
    static const int DEFAULT_ELEMENT_SIZE;
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void createChart();
    void updateChart();
    void updateChartSeries(QLineSeries *series);
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
    QLabel *corner;
    QLabel *refresh_error_label;
    QChartView *chartView = nullptr;
    bool canBeUpdated;
    bool manualRefreshEnabled;
    bool loaderEnabled;
    bool contentAcquired = false;

public slots:
    void update();

protected slots:
    void refreshButtonClick();
};

#endif // ELEMENT_H
