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

#include "well.h"

#include <iostream>

#include "wellconnection.h"
#include "stream.h"
#include "pipe.h"
#include "constraint.h"

using std::cout;
using std::endl;

Well::Well()
    : m_bhp_inj(WellControl::QWAT)

{


}


Well::~Well()
{
    for(int i = 0; i < m_connections.size(); i++)
    {
        delete m_connections.at(i);
    }

    for(int i = 0; i < m_schedule.size(); i++)
    {
        delete m_schedule.at(i);
    }

    for(int i = 0; i < m_streams.size(); i++)
    {
        delete m_streams.at(i);
    }


}

//-----------------------------------------------------------------------------------------------
// deletes all the streams
//-----------------------------------------------------------------------------------------------
void Well::deleteStreams()
{
    // deleting the objects
    for(int i = 0; i < m_streams.size(); i++) delete m_streams.at(i);

    // dereferencing
    m_streams.clear();
}


//-----------------------------------------------------------------------------------------------
// adds a well control to the schedule
//-----------------------------------------------------------------------------------------------
void Well::addControl(WellControl *c)
{
    m_schedule.push_back(c);
}

