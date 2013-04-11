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


#ifndef MODELITEMINJWELL_H
#define MODELITEMINJWELL_H

#include "modelitem.h"

namespace ResOpt
{
class InjectionWell;
}

using ResOpt::InjectionWell;



namespace ResOptGui
{

class ModelItemInjWell : public ModelItem
{
private:
    InjectionWell *p_inj_well;

public:

    ModelItemInjWell(InjectionWell *inj, const QString &file_name = ":new/images/inj_svg", QGraphicsItem *parent = 0);


    InjectionWell* injectionWell() {return p_inj_well;}


};

} // namespace

#endif // MODELITEMINJWELL_H
