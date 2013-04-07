#ifndef CONNECTOR_H
#define CONNECTOR_H

#include <QtGui/QGraphicsLineItem>

namespace ResOptGui
{

class ModelItem;


class Connector : public QGraphicsLineItem
{
public:
    enum ConnectionFace{SideConnection, TopConnection};

    Connector(ModelItem *startItem, ModelItem *endItem, bool active = true, ConnectionFace con_face = SideConnection, QGraphicsItem *parent = 0, QGraphicsScene *scene = 0);


    void updatePosition();



private:

    ModelItem *p_start_item;
    ModelItem *p_end_item;

    ConnectionFace m_con_face;

};

} // namespace

#endif // CONNECTOR_H
