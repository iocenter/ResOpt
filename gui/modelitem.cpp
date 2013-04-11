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
