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


#include "productionwell.h"

#include <iostream>

#include "pipeconnection.h"
#include "binaryvariable.h"
#include "constraint.h"
#include "pipe.h"
#include "midpipe.h"
#include "stream.h"



using std::cout;
using std::endl;

namespace ResOpt
{


ProductionWell::ProductionWell()    
{

}

//-----------------------------------------------------------------------------------------------
// copy constructor
//-----------------------------------------------------------------------------------------------
ProductionWell::ProductionWell(const ProductionWell &w)
    : Well(w)
{

    // copying the constraints

    // the bhp constraints
    for(int i = 0; i < w.numberOfBhpConstraints(); ++i)
    {
        m_bhp_constraints.push_back(shared_ptr<Constraint>(new Constraint(*w.m_bhp_constraints.at(i))));
    }

    // the connection constraint
    p_connection_constraint = shared_ptr<Constraint>(new Constraint(*w.p_connection_constraint));

    // copying the pipe connections
    for(int i = 0; i < w.numberOfPipeConnections(); i++)
    {
        m_pipe_connections.push_back(new PipeConnection(*w.m_pipe_connections.at(i)));
    }

    // copying gas lift controls
    for(int i = 0; i < w.m_gaslift_schedule.size(); i++)
    {
        m_gaslift_schedule.push_back(new WellControl(*w.m_gaslift_schedule.at(i)));
    }




}


ProductionWell::~ProductionWell()
{
    for(int i = 0; i < numberOfPipeConnections(); ++i) delete pipeConnection(i);

    for(int i = 0; i < m_gaslift_schedule.size(); ++i) delete m_gaslift_schedule.at(i);
}

//-----------------------------------------------------------------------------------------------
// sets up the constraints asociated with the well
//-----------------------------------------------------------------------------------------------
void ProductionWell::setupConstraints()
{
    // the bhp constraints

    // first clearing
    m_bhp_constraints.clear();

    // then adding
    for(int i = 0; i < numberOfControls(); ++i)
    {
        shared_ptr<Constraint> bhp_con = shared_ptr<Constraint>(new Constraint(0.5, 1.0, 0.0));
        bhp_con->setName("Bottomhole pressure constraint for well: " + name() + " for time = " + QString::number(control(i)->endTime()));
        m_bhp_constraints.push_back(bhp_con);
    }

    // the connection (routing) constraint
    p_connection_constraint = shared_ptr<Constraint>(new Constraint(1.0, 1.0, 1.0));
    p_connection_constraint->setName("Pipe routing constraint for well: " + name());



}

//-----------------------------------------------------------------------------------------------
// sets the name of the well (overloaded from Well)
//-----------------------------------------------------------------------------------------------
void ProductionWell::setName(const QString &n)
{

    Well::setName(n);

}

//-----------------------------------------------------------------------------------------------
// updates the value of the bhp constraint
//-----------------------------------------------------------------------------------------------
void ProductionWell::updateBhpConstraint()
{
    cout << "Updating the BHP constraint for well " << name().toAscii().data() << endl;


    // checking that all the connected pipes have the same number of streams as the well
    for(int i = 0; i < numberOfPipeConnections(); ++i)
    {
        Pipe *p = pipeConnection(i)->pipe();
        if(numberOfStreams() != p->numberOfStreams())
        {
            cout << endl << "### Runtime Error ###" << endl
                 << "Well and pipe do not have the same number of time steps..." << endl
                 << "WELL: " << name().toAscii().data() << ", N = " << numberOfStreams() << endl
                 << "PIPE: " << p->number() << endl << ", N = " << p->numberOfStreams() << endl;

            exit(1);

        }
    }

    // checking that the number of streams is equal to the number of bhp constraints
    if(numberOfStreams() != numberOfBhpConstraints())
    {
        cout << endl << "### Runtime Error ###" << endl
             << "WELL: " << name().toAscii().data() <<  endl
             << "Does not have the same number of streams and bhp constraints... " << endl;

        exit(1);
    }


    // looping through the streams, updating the constraints
    for(int i = 0; i < numberOfStreams(); i++)
    {
        // the constraint is calculated as:
        // c = (p_wf - p_pipe) / p_wf
        //
        // when the pipe pressure is higher than the bhp, the constraint is violated, and c < 0

        // calculating the weighted average pressure
        double p_in = 0;
        double tot_frac = 0;
        for(int j = 0; j < numberOfPipeConnections(); ++j)
        {
            p_in += pipeConnection(j)->variable()->value() * pipeConnection(j)->pipe()->stream(i)->pressure();
            tot_frac += pipeConnection(j)->variable()->value();
        }

        if(tot_frac == 0) // error checking if no routing to the well
        {
            tot_frac = 1;
            p_in = 2 * stream(i)->pressure();
        }

        p_in = p_in / tot_frac;

        // calculating constraint value
        double p_wf = stream(i)->pressure();
        if(p_wf < 0.001) p_wf = 0.001;

        double c_ts = (p_wf - p_in) / p_wf;



        // updating the value of the constraint for this time
        bhpConstraint(i)->setValue(c_ts);


        // printing if violating
        if(c_ts < 0)
        {
            cout << "BHP constraint for Well " << name().toAscii().data() << " is violated at time = " << stream(i)->time() << ", c = " << c_ts << endl;

            cout << "Well stream:" << endl;
            stream(i)->printToCout();

            for(int k = 0; k < numberOfPipeConnections(); ++k)
            {
                cout << "Pipe #" << pipeConnection(k)->pipe()->number() << " stream:" << endl;
                pipeConnection(k)->pipe()->stream(i)->printToCout();
            }
        }

    }




}

//-----------------------------------------------------------------------------------------------
// updates the value of the pipe connection constraint
//-----------------------------------------------------------------------------------------------
void ProductionWell::updatePipeConnectionConstraint()
{
    cout << "Updating the routing constraint for well " << name().toAscii().data() << endl;

    double c = 0;

    // looping through the pipe connections, adding the value of the variable
    for(int i = 0; i < numberOfPipeConnections(); i++)
    {
        c += pipeConnection(i)->variable()->value();
    }

    // updating the value of the constraint
    pipeConnectionConstraint()->setValue(c);

}

//-----------------------------------------------------------------------------------------------
// finds the fraction of the rates from this well that flows through a pipe
//-----------------------------------------------------------------------------------------------
double ProductionWell::flowFraction(Pipe *p, bool *ok)
{
    double frac = 0.0;


    for(int i = 0; i < numberOfPipeConnections(); i++)
    {
        // first checking if the well is directly connected to the pipe
        if(p->number() == pipeConnection(i)->pipe()->number())
        {
            frac += pipeConnection(i)->variable()->value();
        }

        // then checking if the well is connected indirectly to the pipe (this only appplies to MidPipes)
        MidPipe *p_mid = dynamic_cast<MidPipe*>(pipeConnection(i)->pipe());
        if(p_mid != 0)
        {
            frac += p_mid->flowFraction(p, ok) * pipeConnection(i)->variable()->value();
        }
    }


    return frac;
}


} // namespace ResOpt
