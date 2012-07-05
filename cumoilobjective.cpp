/*
 * This file is part of the ResOpt project.
 *
 * Copyright (C) 2011-2012 Aleksander O. Juell <aleksander.juell@ntnu.no>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
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


#include "cumoilobjective.h"
#include "stream.h"

namespace ResOpt
{


CumoilObjective::CumoilObjective()
{
}


//-----------------------------------------------------------------------------------------------
// Calculates the cumulative oil produced
//-----------------------------------------------------------------------------------------------
void CumoilObjective::calculateValue(QVector<Stream *> s, QVector<Cost *> c)
{
    double cumoil = 0;

    for(int i = 0; i < s.size(); i++)       // looping through all the time steps
    {
        double dt;            // time step duration

        // calculate time step duration
        if(i == 0) dt = s.at(i)->time();
        else dt = s.at(i)->time() - s.at(i-1)->time();

        // adding the time step oil production to the cum
        cumoil += dt * s.at(i)->oilRate();

    }

    // setting the cum oil to the objective value
    setValue(cumoil);

}


} // namespace ResOpt
