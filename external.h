#ifndef EXTERNAL_H
#define EXTERNAL_H

void ext_switch_window();
std::string ext_network_request(std::string url);
std::string readFile(QString path);
QPointF floorToGrid(const QPointF& point, int grid_size);

#endif // EXTERNAL_H
