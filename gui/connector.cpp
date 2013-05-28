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


#include "connector.h"
#include "modelitem.h"

#include <QtGui/QPen>
#include <QtGui/QPainter>
#include <QLineF>
#include <QPointF>
#include <QRectF>

namespace ResOptGui
{

Connector::Connector(ModelItem *startItem, ModelItem *endItem, bool active, ConnectionFace con_face, QGraphicsItem *parent, QGraphicsScene *scene)
    : QGraphicsLineItem(parent),
      p_start_item(startItem),
      p_end_item(endItem),
      m_con_face(con_face)
{


    if(con_face == SideConnection)
    {
        if(active) setPen(QPen(Qt::black, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        else setPen(QPen(Qt::lightGray, 2, Qt::DashLine, Qt::RoundCap, Qt::RoundJoin));
    }
    else
    {
        setPen(QPen(Qt::red, 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    }

    // setting the line
    updatePosition();

    // adding this connector to the items
    p_start_item->addOutletConnector(this);
    p_end_item->addInletConnector(this);
}


//-----------------------------------------------------------------------------------------------
// Updates the position of the line
//-----------------------------------------------------------------------------------------------
void Connector::updatePosition()
{

    // calculating the start position
    QPointF start_x = p_start_item->mapToScene(p_start_item->boundingRect().topRight());
    QPointF start_y = p_start_item->mapToScene(p_start_item->boundingRect().center());

    QPointF start;
    if(m_con_face == SideConnection)
    {
        //start(start_x.x(), start_y.y());
        start.setX(start_x.x());
        start.setY(start_y.y());
    }
    else
    {
        start.setX(start_y.x());
        start.setY(start_x.y());
    }

    // calculating the end position
    QPointF end_x = p_end_item->mapToScene(p_end_item->boundingRect().bottomLeft());
    QPointF end_y = p_end_item->mapToScene(p_end_item->boundingRect().center());

    QPointF end;
    if(m_con_face == SideConnection)
    {
        //QPointF end(end_x.x(), end_y.y());
        end.setX(end_x.x());
        end.setY(end_y.y());
    }
    else
    {
        end.setX(end_y.x());
        end.setY(end_x.y());
    }




    // Setting the line
    QLineF line(start, end);


    setLine(line);

}


} // namespace
