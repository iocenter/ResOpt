#include "modelitem.h"
#include "connector.h"

namespace ResOptGui
{

ModelItem::ModelItem(const QString &file_name, QGraphicsItem *parent)
    : QGraphicsSvgItem(file_name, parent)

{

    setFlag(QGraphicsItem::ItemIsMovable, true);
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
}

//-----------------------------------------------------------------------------------------------
// updates the position of the connectors
//-----------------------------------------------------------------------------------------------
QVariant ModelItem::itemChange(GraphicsItemChange change,
                     const QVariant &value)
{
    if (change == QGraphicsItem::ItemPositionChange)
    {
        // updating the inlet connectors
        foreach (Connector *c, m_inlet_connectors)
        {
            c->updatePosition();
        }

        // updating the outlet connectors
        foreach (Connector *c, m_outlet_connectors)
        {
            c->updatePosition();
        }
    }

    return value;
}


} // namespace
