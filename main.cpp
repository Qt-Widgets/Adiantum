#include "adiantum.h"
#include <QApplication>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    Adiantum adiantum;
    adiantum.show();

    return app.exec();
}
