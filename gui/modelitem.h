#ifndef MODELITEM_H
#define MODELITEM_H

#include <QtSvg>
#include <QString>
#include <QList>

namespace ResOptGui
{

class Connector;

class ModelItem : public QGraphicsSvgItem
{
private:
    QList<Connector*> m_inlet_connectors;
    QList<Connector*> m_outlet_connectors;


public:

    ModelItem(const QString &file_name, QGraphicsItem *parent = 0);

    void addInletConnector(Connector *c) {m_inlet_connectors.append(c);}
    void addOutletConnector(Connector *c) {m_outlet_connectors.append(c);}

protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant &value);




};

} // namespace

#endif // MODELITEM_H
