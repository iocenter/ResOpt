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


#ifndef CONNECTOR_H
#define CONNECTOR_H

#include <QtWidgets/QGraphicsLineItem>

namespace ResOptGui
{

class ModelItem;


class Connector : public QGraphicsLineItem
{
public:
    enum ConnectionFace{SideConnection, TopConnection};

    Connector(ModelItem *startItem, ModelItem *endItem, bool active = true, ConnectionFace con_face = SideConnection, QGraphicsItem *parent = 0);


    void updatePosition();



private:

    ModelItem *p_start_item;
    ModelItem *p_end_item;

    ConnectionFace m_con_face;

};

} // namespace

#endif // CONNECTOR_H
