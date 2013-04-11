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
