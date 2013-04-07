#ifndef MODELSCENE_H
#define MODELSCENE_H

#include <QtGui/QGraphicsScene>
//#include "model.h"

namespace ResOpt
{
class Model;
class Component;

}

using ResOpt::Model;
using ResOpt::Component;

namespace ResOptGui
{

class ModelItem;
class ModelItemProdWell;
class ModelItemInjWell;
class ModelItemMidPipe;
class ModelItemEndPipe;
class ModelItemSeparator;
class ModelItemPressureBooster;
class ModelItemCapacity;


class ModelScene : public QGraphicsScene
{
    Q_OBJECT
private:

    qreal m_current_y;
    qreal m_current_x;

    QList<ModelItemProdWell*> m_prodwell_items;
    QList<ModelItemInjWell*> m_injwell_items;
    QList<ModelItemMidPipe*> m_midpipe_items;
    QList<ModelItemEndPipe*> m_endpipe_items;
    QList<ModelItemSeparator*> m_separator_items;
    QList<ModelItemPressureBooster*> m_pressurebooster_items;
    QList<ModelItemCapacity*> m_capacity_items;


    void addUpstreamToScene(ModelItemProdWell *prod_item);
    void addUpstreamToScene(ModelItemPressureBooster *booster_item);
    void addUpstreamToScene(ModelItemSeparator *sep_item);
    void addUpstreamToScene(ModelItemMidPipe *mid_item);


    ModelItem* addModelItem(Component *c);
    void addCapacities(Model *m);

    void setItemPosition(ModelItem *i);

public:
    explicit ModelScene(QObject *parent = 0);

    void buildSceneFromModel(Model *m);


    ModelItem* itemFromComponent(Component *c);
    
signals:
    void sendMsg(QString);
    
public slots:
    
};

} // namespace

#endif // MODELSCENE_H
