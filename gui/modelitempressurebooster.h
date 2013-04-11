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


#ifndef MODELITEMPRESSUREBOOSTER_H
#define MODELITEMPRESSUREBOOSTER_H

#include "modelitem.h"

namespace ResOpt
{
class PressureBooster;
}

using ResOpt::PressureBooster;



namespace ResOptGui
{

class ModelItemPressureBooster : public ModelItem
{
private:
    PressureBooster *p_booster;

public:

    ModelItemPressureBooster(PressureBooster *booster, const QString &file_name = ":new/images/booster_svg", QGraphicsItem *parent = 0);


    PressureBooster* pressureBooster() {return p_booster;}


};

} // namespace

#endif // MODELITEMPRESSUREBOOSTER_H
