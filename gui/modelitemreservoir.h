/*
 * This file is part of the ResOpt project.
 *
 * Copyright (C) 2011-2014 Aleksander O. Juell <aleksander.juell@ntnu.no>
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


#ifndef MODELITEMRESERVOIR_H
#define MODELITEMRESERVOIR_H

#include "modelitem.h"

namespace ResOpt
{
class Reservoir;
class ReservoirSimulator;
}

using ResOpt::Reservoir;
using ResOpt::ReservoirSimulator;



namespace ResOptGui
{

class ModelItemReservoir : public ModelItem
{
private:
    Reservoir *p_res;
    ReservoirSimulator *p_sim;

protected:
    virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);

public:

    ModelItemReservoir(Reservoir *res, ReservoirSimulator *sim, const QString &file_name = ":new/images/res_svg", QGraphicsItem *parent = 0);


    Reservoir* reservoir() {return p_res;}


};

} // namespace

#endif // MODELITEMRESERVOIR_H
