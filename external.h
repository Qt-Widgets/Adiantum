#ifndef EXTERNAL_H
#define EXTERNAL_H

#include <QObject>

class External : public QObject {
    Q_OBJECT

public:
    static void switch_window();
    static std::string network_request(std::string url);
    static std::string read_file(std::string path);
};

#endif // EXTERNAL_H
