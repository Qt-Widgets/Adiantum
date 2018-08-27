#ifndef ADIANTUM_H
#define ADIANTUM_H

#include <QMainWindow>
#include <QtWidgets>
#include <QtNetwork>

namespace Ui {
class Adiantum;
}

class Adiantum : public QMainWindow {
    Q_OBJECT

public:
    static Adiantum* getInstance();
    bool nativeEvent(const QByteArray &eventType, void *message, long *result);
    ~Adiantum();
    void switchWindow();
    void executeCommand(QString command);
    QNetworkAccessManager* getNAM();

public slots:
   void openAboutWindow();
   void closeApp();

private:
    explicit Adiantum(QWidget *parent = 0);
    static Adiantum *instance;
    QNetworkAccessManager *networkManager;
    QSystemTrayIcon *trayIcon;
    QMenu *trayIconMenu;
};

#endif // ADIANTUM_H
