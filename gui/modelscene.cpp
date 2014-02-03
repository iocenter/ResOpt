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


#include "modelscene.h"

#include "modelitemseparator.h"
#include "modelitemprodwell.h"
#include "modeliteminjwell.h"
#include "modelitemmidpipe.h"
#include "modelitemendpipe.h"
#include "modelitempressurebooster.h"
#include "modelitemcapacity.h"
#include "modelitemreservoir.h"
#include "connector.h"

#include "model.h"
#include "productionwell.h"
#include "injectionwell.h"
#include "pipeconnection.h"
#include "midpipe.h"
#include "endpipe.h"
#include "separator.h"
#include "pressurebooster.h"
#include "capacity.h"
#include "binaryvariable.h"

using ResOpt::ProductionWell;
using ResOpt::InjectionWell;
using ResOpt::PipeConnection;
using ResOpt::Separator;
using ResOpt::MidPipe;
using ResOpt::EndPipe;
using ResOpt::PressureBooster;
using ResOpt::Capacity;
using ResOpt::BinaryVariable;

#include <iostream>

using std::cout;
using std::endl;


//#include <QtSvg>


namespace ResOptGui
{

ModelScene::ModelScene(QObject *parent) :
    QGraphicsScene(parent),
    m_current_y(0),
    m_current_x(0)
{

/*

    ModelItemSeparator *well = new ModelItemSeparator();
    ModelItemSeparator *sep = new ModelItemSeparator();
   // ModelItemInjWell *inj = new ModelItemInjWell();


    sep->setPos(300, 200);
    //inj->setPos(0, 400);


    // testing connector

    Connector *con = new Connector(qgraphicsitem_cast<ModelItem *>(well), qgraphicsitem_cast<ModelItem *>(sep));


    addItem(well);
    addItem(sep);
    //addItem(inj);
    addItem(con);

*/

    //setMinimumSize(600,400);
}


//-----------------------------------------------------------------------------------------------
// Builds all the graphical elements from a model
//-----------------------------------------------------------------------------------------------
void ModelScene::buildSceneFromModel(Model *m, ReservoirSimulator *sim)
{
    // reservoir




    ModelItemReservoir *res_item = new ModelItemReservoir(m->reservoir(), sim);
    int x_res = (m->numberOfWells()-1)*40 -4;
    res_item->setPos(-200, x_res);
    addItem(res_item);

    // wells

    for(int i = 0; i < m->numberOfWells(); ++i)
    {

        ProductionWell *prod_well = dynamic_cast<ProductionWell*>(m->well(i));
        InjectionWell *inj_well = dynamic_cast<InjectionWell*>(m->well(i));

        if(prod_well != 0)
        {
            m_current_x = 0;

            // adding the production well to the scene
            ModelItemProdWell *prod_item = new ModelItemProdWell(prod_well);
            prod_item->setPos(m_current_x, m_current_y);

            addItem(prod_item);
            m_prodwell_items.append(prod_item);

            // connecting to reservoir
            Connector *c = new Connector(res_item, prod_item, true);
            addItem(c);


            // adding upstream items to the scene
            addUpstreamToScene(prod_item);

        }

        else if(inj_well != 0)
        {
            m_current_x = 0;

            ModelItemInjWell *inj_item = new ModelItemInjWell(inj_well);

            inj_item->setPos(m_current_x, m_current_y);

            //inj_item->setPos(inj_item->mapFromScene(m_current_x, m_current_y));

            addItem(inj_item);
            m_injwell_items.append(inj_item);

            // connecting to reservoir
            Connector *c = new Connector(res_item, inj_item, true);
            addItem(c);


        }


        m_current_y = (i+1)*80;

    }


    // adding the capacities to the scene
    addCapacities(m);
}

//-----------------------------------------------------------------------------------------------
// Adds the upstream connected components of a production well
//-----------------------------------------------------------------------------------------------
void ModelScene::addUpstreamToScene(ModelItemProdWell *prod_item)
{

    m_current_y += 30;

    ProductionWell *prod_well = prod_item->productionWell();

    // looping through the outlet connections of the well
    for(int i = 0; i < prod_well->numberOfPipeConnections(); ++i)
    {
        PipeConnection *pipe_con = prod_well->pipeConnection(i);

        // checking if the current connected item is allready present
        ModelItem *item = itemFromComponent(pipe_con->pipe());

        if(item == 0) // the item is not present yet
        {

            // adding the item to the scene
            item = addModelItem(pipe_con->pipe());


        }

        // need to add the connector between the well and the item
        bool active = pipe_con->variable()->value() > 0.0;
        Connector *c = new Connector(prod_item, item, active);
        addItem(c);


        // then adding the connected items upstream connections
        ModelItemPressureBooster *upstream_booster_item = dynamic_cast<ModelItemPressureBooster*>(item);
        ModelItemSeparator *upstream_sep_item = dynamic_cast<ModelItemSeparator*>(item);
        ModelItemMidPipe *upstream_mid_item = dynamic_cast<ModelItemMidPipe*>(item);

        if(upstream_booster_item != 0) addUpstreamToScene(upstream_booster_item);
        else if(upstream_sep_item != 0) addUpstreamToScene(upstream_sep_item);
        else if(upstream_mid_item != 0) addUpstreamToScene(upstream_mid_item);






    }
}

//-----------------------------------------------------------------------------------------------
// Adds the upstream connected component of a pressure booster
//-----------------------------------------------------------------------------------------------
void ModelScene::addUpstreamToScene(ModelItemPressureBooster *booster_item)
{
    PressureBooster *booster = booster_item->pressureBooster();

    // getting the outlet connection
    PipeConnection *pipe_con = booster->outletConnection();

    // checking if the current connected item is allready present
    ModelItem *item = itemFromComponent(pipe_con->pipe());

    if(item == 0) // the item is not present yet
    {
        // adding the item to the scene
        item = addModelItem(pipe_con->pipe());
    }

    // need to add the connector between the booster and the item
    bool active = pipe_con->variable()->value() > 0.0;
    Connector *c = new Connector(booster_item, item, active);
    addItem(c);


    // then adding the connected items upstream connections
    ModelItemPressureBooster *upstream_booster_item = dynamic_cast<ModelItemPressureBooster*>(item);
    ModelItemSeparator *upstream_sep_item = dynamic_cast<ModelItemSeparator*>(item);
    ModelItemMidPipe *upstream_mid_item = dynamic_cast<ModelItemMidPipe*>(item);

    if(upstream_booster_item != 0) addUpstreamToScene(upstream_booster_item);
    else if(upstream_sep_item != 0) addUpstreamToScene(upstream_sep_item);
    else if(upstream_mid_item != 0) addUpstreamToScene(upstream_mid_item);



}

//-----------------------------------------------------------------------------------------------
// Adds the upstream connected component of a pressure booster
//-----------------------------------------------------------------------------------------------
void ModelScene::addUpstreamToScene(ModelItemSeparator *sep_item)
{
    Separator *sep = sep_item->separator();

    // getting the outlet connection
    PipeConnection *pipe_con = sep->outletConnection();

    // checking if the current connected item is allready present
    ModelItem *item = itemFromComponent(pipe_con->pipe());

    if(item == 0) // the item is not present yet
    {
        // adding the item to the scene
        item = addModelItem(pipe_con->pipe());
    }

    // need to add the connector between the booster and the item
    bool active = pipe_con->variable()->value() > 0.0;
    Connector *c = new Connector(sep_item, item, active);
    addItem(c);


    // then adding the connected items upstream connections
    ModelItemPressureBooster *upstream_booster_item = dynamic_cast<ModelItemPressureBooster*>(item);
    ModelItemSeparator *upstream_sep_item = dynamic_cast<ModelItemSeparator*>(item);
    ModelItemMidPipe *upstream_mid_item = dynamic_cast<ModelItemMidPipe*>(item);

    if(upstream_booster_item != 0) addUpstreamToScene(upstream_booster_item);
    else if(upstream_sep_item != 0) addUpstreamToScene(upstream_sep_item);
    else if(upstream_mid_item != 0) addUpstreamToScene(upstream_mid_item);



}

//-----------------------------------------------------------------------------------------------
// Adds the upstream connected components of a mid pipe
//-----------------------------------------------------------------------------------------------
void ModelScene::addUpstreamToScene(ModelItemMidPipe *mid_item)
{
    MidPipe *mid = mid_item->midPipe();

    // looping through the outlet connections of the mid pipe
    for(int i = 0; i < mid->numberOfOutletConnections(); ++i)
    {
        PipeConnection *pipe_con = mid->outletConnection(i);

        // checking if the current connected item is allready present
        ModelItem *item = itemFromComponent(pipe_con->pipe());

        if(item == 0) // the item is not present yet
        {

            // adding the item to the scene
            item = addModelItem(pipe_con->pipe());


        }

        // need to add the connector between the well and the item
        bool active = pipe_con->variable()->value() > 0.0;
        Connector *c = new Connector(mid_item, item, active);
        addItem(c);


        // then adding the connected items upstream connections
        ModelItemPressureBooster *upstream_booster_item = dynamic_cast<ModelItemPressureBooster*>(item);
        ModelItemSeparator *upstream_sep_item = dynamic_cast<ModelItemSeparator*>(item);
        ModelItemMidPipe *upstream_mid_item = dynamic_cast<ModelItemMidPipe*>(item);

        if(upstream_booster_item != 0) addUpstreamToScene(upstream_booster_item);
        else if(upstream_sep_item != 0) addUpstreamToScene(upstream_sep_item);
        else if(upstream_mid_item != 0) addUpstreamToScene(upstream_mid_item);




    }
}

//-----------------------------------------------------------------------------------------------
// Returns the model item asociated with component c
//-----------------------------------------------------------------------------------------------
ModelItem* ModelScene::itemFromComponent(Component *c)
{

    // checking the mid pipes
    for(int i = 0; i < m_midpipe_items.size(); ++i)
    {
        if(c->id() == m_midpipe_items.at(i)->midPipe()->id())
        {
            return m_midpipe_items.at(i);
        }
    }

    // checking the end pipes
    for(int i = 0; i < m_endpipe_items.size(); ++i)
    {
        if(c->id() == m_endpipe_items.at(i)->endPipe()->id())
        {
            return m_endpipe_items.at(i);
        }
    }

    // checking the separators
    for(int i = 0; i < m_separator_items.size(); ++i)
    {
        if(c->id() == m_separator_items.at(i)->separator()->id())
        {
            return m_separator_items.at(i);
        }
    }

    // checking the pressure boosters
    for(int i = 0; i < m_pressurebooster_items.size(); ++i)
    {
        if(c->id() == m_pressurebooster_items.at(i)->pressureBooster()->id())
        {
            return m_pressurebooster_items.at(i);
        }
    }

    // checing the production wells
    for(int i = 0; i < m_prodwell_items.size(); ++i)
    {
        if(c->id() == m_prodwell_items.at(i)->productionWell()->id())
        {
            return m_prodwell_items.at(i);
        }
    }

    // checing the injection wells
    for(int i = 0; i < m_injwell_items.size(); ++i)
    {
        if(c->id() == m_injwell_items.at(i)->injectionWell()->id())
        {
            return m_injwell_items.at(i);
        }
    }



    return 0;
}

//-----------------------------------------------------------------------------------------------
// Adds a model item to represent component c to the scene
//-----------------------------------------------------------------------------------------------
ModelItem* ModelScene::addModelItem(Component *c)
{
    m_current_x += 100;

    // checking if the component is a production well
    ProductionWell *prod_well = dynamic_cast<ProductionWell*>(c);
    if(prod_well != 0)
    {
        ModelItemProdWell *prod_item = new ModelItemProdWell(prod_well);
        prod_item->setPos(m_current_x, m_current_y);

        addItem(prod_item);
        m_prodwell_items.append(prod_item);

        return prod_item;
    }

    // checking if the component is an injection well
    InjectionWell *inj_well = dynamic_cast<InjectionWell*>(c);
    if(inj_well != 0)
    {
        ModelItemInjWell *inj_item = new ModelItemInjWell(inj_well);
        inj_item->setPos(m_current_x, m_current_y);

        addItem(inj_item);
        m_injwell_items.append(inj_item);

        return inj_item;
    }

    // checking if the component is a mid pipe
    MidPipe *midpipe = dynamic_cast<MidPipe*>(c);
    if(midpipe != 0)
    {
        ModelItemMidPipe *mid_item = new ModelItemMidPipe(midpipe);


        setItemPosition(mid_item);

        addItem(mid_item);
        m_midpipe_items.append(mid_item);

        return mid_item;
    }

    // checking if the component is an end pipe
    EndPipe *endpipe = dynamic_cast<EndPipe*>(c);
    if(endpipe != 0)
    {
        ModelItemEndPipe *end_item = new ModelItemEndPipe(endpipe);
        setItemPosition(end_item);

        addItem(end_item);
        m_endpipe_items.append(end_item);

        return end_item;
    }

    // checking if the component is a separator
    Separator *sep = dynamic_cast<Separator*>(c);
    if(sep != 0)
    {
        ModelItemSeparator *sep_item = new ModelItemSeparator(sep);
        setItemPosition(sep_item);

        addItem(sep_item);
        m_separator_items.append(sep_item);

        return sep_item;
    }


    // checking if the component is a pressure booster
    PressureBooster *booster = dynamic_cast<PressureBooster*>(c);
    if(booster != 0)
    {
        ModelItemPressureBooster *booster_item = new ModelItemPressureBooster(booster);
        setItemPosition(booster_item);

        addItem(booster_item);
        m_pressurebooster_items.append(booster_item);

        return booster_item;
    }

    return 0;




}


//-----------------------------------------------------------------------------------------------
// Adds the capacities to the scene
//-----------------------------------------------------------------------------------------------
void ModelScene::addCapacities(Model *m)
{

    for(int i = 0; i < m->numberOfCapacities(); ++i)
    {
        ModelItemCapacity *cap_item = new ModelItemCapacity(m->capacity(i));
        //cap_item->setPos(i*100 + 272, 0);


        m_capacity_items.append(cap_item);

        // adding connectors
        for(int j = 0; j < cap_item->capacity()->numberOfFeedPipes(); ++j)
        {
            ModelItem *feed = itemFromComponent(cap_item->capacity()->feedPipe(j));

            if(i == 0) cap_item->setPos(feed->pos().x() + 6, 0);

            Connector *con = new Connector(feed, cap_item, true, Connector::TopConnection);
            addItem(con);
        }

        addItem(cap_item);
    }

}

//-----------------------------------------------------------------------------------------------
// sets the position of the model item on the scene
//-----------------------------------------------------------------------------------------------
void ModelScene::setItemPosition(ModelItem *i)
{

    QPointF tl = i->mapToScene(i->boundingRect().topLeft());
    QPointF br = i->mapToScene(i->boundingRect().bottomRight());

    qreal w = br.x() - tl.x();
    qreal h = br.y() - tl.y();

    qreal x = m_current_x - w/2;
    qreal y = m_current_y - h/2;

    i->setPos(x,y);
    //i->setPos(i->mapFromScene(x, y));
    //i->setPos(i->mapFromScene(m_current_x, m_current_y));

   // i->setPos(m_current_x, m_current_y);

}

} // namespace
