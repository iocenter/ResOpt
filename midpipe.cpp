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


#include "midpipe.h"

#include <iostream>
#include "stream.h"
#include "pressuredropcalculator.h"
#include "constraint.h"
#include "pipeconnection.h"
#include "binaryvariable.h"



using std::cout;
using std::endl;

namespace ResOpt
{

MidPipe::MidPipe()
    : p_connection_constraint(new Constraint(1.0, 1.0, 0.0))
{

}

MidPipe::MidPipe(const MidPipe &p)
    : Pipe(p)
{
    // copying the pipe connections
    for(int i = 0; i < p.numberOfOutletConnections(); i++)
    {
        m_outlet_connections.push_back(new PipeConnection(*p.m_outlet_connections.at(i)));
    }

    // copying the constraint
    p_connection_constraint = shared_ptr<Constraint>(new Constraint(*p.p_connection_constraint));

}

MidPipe::~MidPipe()
{
    for(int i = 0; i < m_outlet_connections.size(); i++)
    {
        delete m_outlet_connections.at(i);
    }


}

//-----------------------------------------------------------------------------------------------
// sets the number of the pipe (overloaded from Pipe)
//-----------------------------------------------------------------------------------------------
void MidPipe::setNumber(int n)
{
    Pipe::setNumber(n);

    p_connection_constraint->setName("Pipe routing constraint for Pipe #" + QString::number(number()));
}


//-----------------------------------------------------------------------------------------------
// checks if any of the outlet connections are midpipes
//-----------------------------------------------------------------------------------------------
bool MidPipe::midpipeConnectedUpstream()
{
    for(int i = 0; i < numberOfOutletConnections(); ++i)
    {
        MidPipe *p = dynamic_cast<MidPipe*>(outletConnection(i)->pipe());
        if(p != 0) return true;
    }

    return false;
}

//-----------------------------------------------------------------------------------------------
// calculates the inlet pressure of the pipe
//-----------------------------------------------------------------------------------------------
void MidPipe::calculateInletPressure()
{
    // checking if the outlet connections are defined
    if(numberOfOutletConnections() == 0)
    {
        cout << endl << "### Runtime Error ###" << endl
             << "NO Outlet pipe set for PIPE: " << number() << endl << endl;

        exit(1);
    }

    // checking if outlet pipes have the same number of streams
    for(int k = 0; k < numberOfOutletConnections(); k++)
    {

        if(numberOfStreams() != outletConnection(k)->pipe()->numberOfStreams())
        {
            cout << endl << "### Runtime Error ###" << endl
                << "Downstream and upstream pipes do not have the same number of time steps..." << endl
                << "Downstream PIPE: " << number() << ", n = " << numberOfStreams() << endl
                << "Upstream PIPE: " << outletConnection(k)->pipe()->number() << ", n = " << outletConnection(k)->pipe()->numberOfStreams() << endl << endl;

            exit(1);
        }
    }

    // looping through the time steps
    for(int i = 0; i < numberOfStreams(); i++)
    {
        // getting the outlet pressure (calculated as the weighted average of all outlet connections)
        double p_out = 0;
        double frac = 0;
        for(int k = 0; k < numberOfOutletConnections(); k++)
        {
            frac += outletConnection(k)->variable()->value();
            p_out += outletConnection(k)->pipe()->stream(i)->pressure()*outletConnection(k)->variable()->value();
        }

        p_out = p_out / frac;


        double dp = calculator()->pressureDrop(stream(i), p_out);    // the pressure drop in the pipe

        stream(i)->setPressure(dp + p_out);      // setting the inlet pressure for the time step
    }


}

//-----------------------------------------------------------------------------------------------
// gets the fraction of flow from this pipe to an upstream pipe
//-----------------------------------------------------------------------------------------------
double MidPipe::flowFraction(Pipe *upstream_pipe, bool *ok)
{
    double frac_total = 0.0;

    // looping through the outlet connections
    for(int i = 0; i < numberOfOutletConnections(); i++)
    {
        double frac_direct = 0;
        // first checking if the upstream pipe is directly connected to this one
        if(upstream_pipe->number() == outletConnection(i)->pipe()->number())
        {
            frac_direct = outletConnection(i)->variable()->value();
        }

        // then checking if the upstream pipe is indirectly connected to this one (this only applies to MidPipes)
        double frac_indirect = 0;

        MidPipe *p_mid = dynamic_cast<MidPipe*>(outletConnection(i)->pipe());
        if(p_mid != 0)
        {
            frac_indirect = p_mid->flowFraction(upstream_pipe, ok) * outletConnection(i)->variable()->value();
        }

        frac_total += frac_direct + frac_indirect;
    }

    return frac_total;
}

//-----------------------------------------------------------------------------------------------
// updates the outlet connections constraint
//-----------------------------------------------------------------------------------------------
void MidPipe::updateOutletConnectionConstraint()
{
    double c = 0;

    for(int i = 0; i < numberOfOutletConnections(); i++)
    {
        c += outletConnection(i)->variable()->value();
    }

    p_connection_constraint->setValue(c);
}

} // namespace ResOpt
