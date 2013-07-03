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
#include "wellconnectionvariable.h"
#include "stream.h"
#include "pipe.h"
#include "constraint.h"
#include "wellcontrol.h"
#include "intvariable.h"
#include "cost.h"


using std::cout;
using std::endl;

namespace ResOpt
{


Well::Well()
    : m_bhp_inj(WellControl::QWAT),
      p_cost(0)
{


}

//-----------------------------------------------------------------------------------------------
// copy constructor
//-----------------------------------------------------------------------------------------------
Well::Well(const Well &w)
    : Component(w),
      p_cost(0)
{

    // copying basic types
    m_type = w.m_type;
    m_name = w.m_name;
    m_group = w.m_group;
    m_bhp_limit = w.m_bhp_limit;
    m_bhp_inj = w.m_bhp_inj;

    // copying connections
    for(int i = 0; i < w.m_connections.size(); i++)
    {
        m_connections.push_back(new WellConnection(*w.m_connections.at(i)));
    }

    // copying variable connections
    for(int i = 0; i < w.m_var_connections.size(); ++i)
    {
        m_var_connections.push_back(new WellConnectionVariable(*w.m_var_connections.at(i)));
    }

    // copying controls
    for(int i = 0; i < w.numberOfControls(); i++)
    {
        m_schedule.push_back(new WellControl(*w.m_schedule.at(i)));
    }



    // copying the cost and install time
    if(w.p_cost != 0) p_cost = new Cost(*w.p_cost);
    if(w.p_install_time != 0) p_install_time = shared_ptr<IntVariable>(new IntVariable(*w.p_install_time));


}


Well::~Well()
{
    for(int i = 0; i < m_connections.size(); i++)
    {
        delete m_connections.at(i);
    }

    for(int i = 0; i < m_var_connections.size(); ++i)
    {
        delete m_var_connections.at(i);
    }

    for(int i = 0; i < m_schedule.size(); i++)
    {
        delete m_schedule.at(i);
    }

    if(p_cost != 0) delete p_cost;


}

//-----------------------------------------------------------------------------------------------
// Initializes the well, setting up the streams
//-----------------------------------------------------------------------------------------------
void Well::initialize()
{
    clearStreams();

    //for(int i = 0; i < m_schedule.size(); ++i) addStream(new Stream());
    for(int i = 0; i < m_schedule.size(); ++i) addStream(new Stream(m_schedule.at(i)->endTime(), 0, 0, 0, 0));
}




//-----------------------------------------------------------------------------------------------
// adds a well control to the schedule
//-----------------------------------------------------------------------------------------------
void Well::addControl(WellControl *c)
{
    m_schedule.push_back(c);
}

//-----------------------------------------------------------------------------------------------
// checks if the well is installed at schedule entry i
//-----------------------------------------------------------------------------------------------
bool Well::isInstalled(int i)
{
    if(p_install_time == 0) return true;
    else return (i >= p_install_time->value());
}

//-----------------------------------------------------------------------------------------------
// returns a well connection
//-----------------------------------------------------------------------------------------------
WellConnection* Well::connection(int i) const
{
    if(i < m_connections.size()) return m_connections.at(i);
    else if(i < (m_connections.size() + m_var_connections.size()))
    {
        return m_var_connections.at(i - m_connections.size())->wellConnection();
    }
    else return 0;

}

//-----------------------------------------------------------------------------------------------
// adds a variable connection
//-----------------------------------------------------------------------------------------------
void Well::addVariableConnection(WellConnectionVariable *vc)
{
    vc->iVariable()->setName("I-direction connection variable for well: " + name());
    vc->jVariable()->setName("J-direction connection variable for well: " + name());

    m_var_connections.push_back(vc);
}

} // namespace ResOpt
