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


#include "endpipe.h"

#include <iostream>
#include "pressuredropcalculator.h"
#include "stream.h"

namespace ResOpt
{


using std::cout;
using std::endl;

EndPipe::EndPipe()
    : m_outletpressure(-1.0)
{
}

EndPipe::EndPipe(const EndPipe &p)
    : Pipe(p)
{
    m_outletpressure = p.m_outletpressure;

}

EndPipe::~EndPipe()
{}

//-----------------------------------------------------------------------------------------------
// calculates the inlet pressure of the pipe
//-----------------------------------------------------------------------------------------------
void EndPipe::calculateInletPressure()
{

    // looping through the time steps
    for(int i = 0; i < numberOfStreams(); i++)
    {
        double dp = calculator()->pressureDrop(stream(i), outletPressure());    // the pressure drop in the pipe

        stream(i)->setPressure(dp + outletPressure());      // setting the inlet pressure for the time step
    }


}

} // namespace ResOpt
