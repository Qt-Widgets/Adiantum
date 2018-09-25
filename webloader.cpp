#include "adiantum.h"
#include "webloader.h"
#include "element.h"
#include <QtNetwork>

Webloader::Webloader(QWidget *parent, QString name, QString url, int width, int height) : Element(parent, name, width, height) {
    this->url = QUrl(url);
    loader = new QLabel(this);
    QMovie *movie = new QMovie(":res/images/loader.gif");
    movie->setScaledSize(QSize(64, 64));
    movie->setSpeed(200);
    loader->setFixedSize(QSize(this->width(), this->height()));
    loader->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    loader->setAttribute(Qt::WA_TranslucentBackground);
    loader->setMovie(movie);
    refresh = new QPushButton(this);
    refresh->setIcon(QIcon(":res/images/refresh.png"));
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
    refresh->show();
    refresh->move(this->width() - refresh->width(), this->height() - refresh->height());
    connect(refresh, SIGNAL(clicked()), this, SLOT (refreshButtonClick()));
    safe_onrequestdone = state["onRequestDone"];
}

void Webloader::update() {
    loader->show();
    loader->movie()->start();
    refresh->hide();
    content->setText("<html></html>");
    QNetworkRequest request(url);
    QNetworkReply* reply = Adiantum::getInstance()->getNAM()->get(request);
    connect(reply, SIGNAL(finished()), this, SLOT(requestDone()));
}

void Webloader::refreshButtonClick() {
    this->update();
}

void Webloader::requestDone() {
    loader->hide();
    loader->movie()->stop();
    refresh->show();
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    if (reply->error() == QNetworkReply::NoError) {
        content->setText("<html></html>");
        QByteArray bytes = reply->readAll();
        response = QString::fromUtf8(bytes.data(), bytes.size());
        auto auto_result = safe_onrequestdone(response.toStdString());
        std::string result = auto_result;
        if (auto_result.valid()) {
            content->setText(QString::fromStdString(result));
        } else {
            this->renderLuaError();
            qDebug() << QString::fromStdString(result);
        }
    } else {
        content->setText("<html><img src=':/res/images/disconnect.png'></html>");
    }
    reply->deleteLater();
}
