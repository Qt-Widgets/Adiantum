#ifndef ADIANTUM_H
#define ADIANTUM_H

#include <QMainWindow>

namespace Ui {
class Adiantum;
}

class Adiantum : public QMainWindow {
    Q_OBJECT

public:
    explicit Adiantum(QWidget *parent = 0);
    bool nativeEvent(const QByteArray &eventType, void *message, long *result);
    ~Adiantum();

private:
    bool active = true;
};

#endif // ADIANTUM_H
