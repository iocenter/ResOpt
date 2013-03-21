#ifndef CONNECTOR_H
#define CONNECTOR_H

#include <QtGui/QGraphicsLineItem>

namespace ResOptGui
{

class ModelItem;


class Connector : public QGraphicsLineItem
{
public:

    Connector(ModelItem *startItem, ModelItem *endItem, bool active = true, QGraphicsItem *parent = 0, QGraphicsScene *scene = 0);


    void updatePosition();



private:

    ModelItem *p_start_item;
    ModelItem *p_end_item;

};

} // namespace

#endif // CONNECTOR_H
