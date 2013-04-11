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


#ifndef MODELITEMMIDPIPE_H
#define MODELITEMMIDPIPE_H

#include "modelitem.h"

namespace ResOpt
{
class MidPipe;
}

using ResOpt::MidPipe;



namespace ResOptGui
{

class ModelItemMidPipe : public ModelItem
{
private:
    MidPipe *p_midpipe;

public:

    ModelItemMidPipe(MidPipe *pipe, const QString &file_name = ":new/images/midpipe_svg", QGraphicsItem *parent = 0);


    MidPipe* midPipe() {return p_midpipe;}


};

} // namespace

#endif // MODELITEMMIDPIPE_H
