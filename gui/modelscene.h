/*
 * This file is part of the ResOpt project.
 *
 * Copyright (C) 2011-2013 Aleksander O. Juell <aleksander.juell@ntnu.no>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */


#ifndef MODELSCENE_H
#define MODELSCENE_H

#include <QtGui/QGraphicsScene>

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
