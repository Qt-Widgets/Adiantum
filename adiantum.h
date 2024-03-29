#ifndef ADIANTUM_H
#define ADIANTUM_H

#include <QMainWindow>
#include <QtWidgets>
#include <QtNetwork>

#include "element.h"

namespace Ui {
class Adiantum;
}

class Adiantum : public QMainWindow {
    Q_OBJECT

public:
    static Adiantum* getInstance();
    bool nativeEvent(const QByteArray &eventType, void *message, long *result);
    void closeEvent(QCloseEvent *e);
    ~Adiantum();
    void switchWindow();
    QString networkRequest(QString url);
    std::string getNetworkInterfaces();
    void loadElements();
    void saveElements();

public slots:
   void openAboutWindow();
   void closeApp();

private:
    explicit Adiantum(QWidget *parent = 0);
    static Adiantum *instance;
    QNetworkAccessManager *networkManager;
    QSystemTrayIcon *trayIcon;
    QMenu *trayIconMenu;
    QMap<QString, Element*> elements;
    QFile *config;
};

#endif // ADIANTUM_H
