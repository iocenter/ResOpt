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



#include "component.h"
#include "stream.h"

namespace ResOpt
{

Component::Component()
{
}

Component::Component(const Component &c)
{

    // the streams
    m_streams.clear();
    m_streams.resize(c.m_streams.size());

    for(int i = 0; i < c.numberOfStreams(); i++)
    {
        m_streams.replace(i, new Stream(*c.m_streams.at(i)));
    }



}

Component::~Component()
{
    for(int i = 0; i < numberOfStreams(); ++i) delete m_streams.at(i);
}


//-----------------------------------------------------------------------------------------------
// sets the Stream for interval i
//-----------------------------------------------------------------------------------------------
bool Component::setStream(int i, Stream *s)
{

    // first checking that the stream vector is set up correctly
    if(i >= m_streams.size() || i < 0) return false;

    // then checking that the number of streams correspond to the number of schedule entries
    //if(m_streams.size() != m_schedule.size()) return false;

    // then checking that the time of the stream corresponds to the time of the shecdule entry
    //if(s->time() != m_schedule.at(i)->endTime()) return false;

    // everything seems to be ok, setting the stream
    delete m_streams.at(i);
    m_streams.replace(i, s);


    return true;
}


} // namespace ResOpt
