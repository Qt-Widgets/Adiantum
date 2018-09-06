#ifndef WEBLOADER_H
#define WEBLOADER_H

#include <QtWidgets>
#include <QtNetwork>

#include "element.h"

class Webloader : public Element {
    Q_OBJECT

struct OnLoadCallback {
    void (*func)(Webloader*, QString);
};

public:
    Webloader(QString name, QString url, int width, int height);
    OnLoadCallback onLoad;

protected:
    void load();

private:
    QUrl url;
    QString response;
    QPushButton *refresh;
    QLabel *loader;

protected slots:
    void requestDone();
    void refreshButtonClick();
};

#endif // WEBLOADER_H
