#ifndef WEBLOADER_H
#define WEBLOADER_H

#include <QtWidgets>
#include <QtNetwork>

#include "element.h"

class Webloader : public Element {
    Q_OBJECT

public:
    Webloader(QWidget *parent, QString name, QString url, int width, int height);
    void update() override;

private:
    QUrl url;
    QString response;
    QPushButton *refresh;
    QLabel *loader;
    sol::protected_function safe_onrequestdone;

protected slots:
    void requestDone();
    void refreshButtonClick();
};

#endif // WEBLOADER_H
