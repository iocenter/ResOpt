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
    : m_outletpressure(-1.0),
      m_outlet_unit(Stream::FIELD)
{
}

EndPipe::EndPipe(const EndPipe &p)
    : Pipe(p)
{
    m_outletpressure = p.m_outletpressure;
    m_outlet_unit = p.m_outlet_unit;

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
        // checking that the outlet pressure unit matches the stream
        double out_pres = outletPressure();
        if(outletUnit() != stream(i)->inputUnits())
        {
            if(outletUnit() == Stream::METRIC) out_pres = outletPressure()*14.5037738;
            else out_pres = outletPressure() / 14.5037738;
        }

        double dp = calculator()->pressureDrop(stream(i), outletPressure(), stream(i)->inputUnits());    // the pressure drop in the pipe

        stream(i)->setPressure(dp + outletPressure());      // setting the inlet pressure for the time step
    }


}

//-----------------------------------------------------------------------------------------------
// generates a description for driver file
//-----------------------------------------------------------------------------------------------
QString EndPipe::description() const
{
    QString str("START PIPE\n");
    str.append(" NUMBER " + QString::number(number()) + "\n");
    str.append(" FILE " + fileName() + "\n");

    str.append(" OUTLETPRESSURE " + QString::number(outletPressure()));
    if(outletUnit() == Stream::METRIC) str.append(" BARA\n");
    else str.append(" PSIA\n");



    str.append("END PIPE\n\n");
    return str;
}


} // namespace ResOpt
