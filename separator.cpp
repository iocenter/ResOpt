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


#include "separator.h"

#include <iostream>
#include "pipe.h"
#include "stream.h"
#include "constraint.h"



using std::cout;
using std::endl;

namespace ResOpt
{

Separator::Separator()

{
}

Separator::Separator(const Separator &s)
{

    // copying basic types
    m_name = s.m_name;

    for(int i = 0; i < s.numberOfFeedPipeNumbers(); i++)
    {
        m_feed_pipe_numbers.push_back(s.feedPipeNumber(i));
    }

    // copying constraints

    if(s.p_oil != 0) p_oil = shared_ptr<Constraint>(new Constraint(*s.p_oil));
    if(s.p_gas != 0) p_gas = shared_ptr<Constraint>(new Constraint(*s.p_gas));
    if(s.p_water != 0) p_water = shared_ptr<Constraint>(new Constraint(*s.p_water));
    if(s.p_liquid != 0) p_liquid = shared_ptr<Constraint>(new Constraint(*s.p_liquid));

    // nothing should be done for the feed pipes, they must be resolved from the model
}


//-----------------------------------------------------------------------------------------------
// update constraints
//-----------------------------------------------------------------------------------------------
void Separator::updateConstraints()
{
    double max_gas = 0.0;
    double max_oil = 0.0;
    double max_water = 0.0;
    double max_liq = 0.0;

    // checking if any feed pipes are defined
    if(numberOfFeedPipes() > 0)
    {
        // looping through the time steps
        for(int i = 0; i < feedPipe(0)->numberOfStreams(); i++)
        {
            Stream s;

            // looping through the feed pipes
            for(int j = 0; j < numberOfFeedPipes(); j++)
            {
                s += *feedPipe(j)->stream(i);   // adding feed pipe rate to the separator rate
            }

            // checking if the current time step has higher rates
            if(s.gasRate() > max_gas) max_gas = s.gasRate();
            if(s.oilRate() > max_oil) max_oil = s.oilRate();
            if(s.waterRate() > max_water) max_water = s.waterRate();

            double q_liq = s.oilRate() + s.waterRate();
            if(q_liq > max_liq) max_liq = q_liq;
        }
    }

    // updating the constraints
    if(gasConstraint() != 0) gasConstraint()->setValue(max_gas);
    if(oilConstraint() != 0) oilConstraint()->setValue(max_oil);
    if(waterConstraint() != 0) waterConstraint()->setValue(max_water);
    if(liquidConstraint() != 0) liquidConstraint()->setValue(max_liq);

    // printing if constraint is violated

    //cout << "Separator: " << name().toAscii().data() << endl;
    //cout << "Max gas = " << max_gas << endl;
    //cout << "Max oil = " << max_oil << endl;
    //cout << "Max wat = " << max_water << endl;

}

} // namespace ResOpt
