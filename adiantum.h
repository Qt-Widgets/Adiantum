#ifndef ADIANTUM_H
#define ADIANTUM_H

#include <QMainWindow>
#include <QtWidgets>

namespace Ui {
class Adiantum;
}

class Adiantum : public QMainWindow {
    Q_OBJECT

public:
    explicit Adiantum(QWidget *parent = 0);
    bool nativeEvent(const QByteArray &eventType, void *message, long *result);
    ~Adiantum();

public slots:
   void openAboutWindow();
   void closeApp();

private:
    bool active = true;
    QSystemTrayIcon *trayIcon;
    QMenu *trayIconMenu;
};

#endif // ADIANTUM_H
