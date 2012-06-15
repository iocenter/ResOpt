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


#include "model.h"

#include <iostream>
#include "reservoir.h"
#include "well.h"
#include "separator.h"
#include "intvariable.h"
#include "realvariable.h"
#include "binaryvariable.h"
#include "constraint.h"
#include "objective.h"
#include "midpipe.h"
#include "endpipe.h"
#include "stream.h"
#include "productionwell.h"
#include "pipeconnection.h"

using std::cout;
using std::endl;


namespace ResOpt
{



Model::Model()
{
}

//-----------------------------------------------------------------------------------------------
// Copy constructor
//-----------------------------------------------------------------------------------------------
Model::Model(const Model &m)
{

    // copying the reservoir
    p_reservoir = new Reservoir(*m.reservoir());

    // copying the wells
    for(int i = 0; i < m.numberOfWells(); i++)
    {
        m_wells.push_back(m.well(i)->clone());
    }

    // copying the pipes
    for(int i = 0; i < m.numberOfPipes(); i++)
    {
        m_pipes.push_back(m.pipe(i)->clone());
    }

    // copying the separators
    for(int i = 0; i < m.numberOfSeparators(); i++)
    {
        m_separators.push_back(new Separator(*m.m_separators.at(i)));
    }

    // copying the objective
    p_obj = m.p_obj->clone();

    // now it is time to connect the pipes and separators

}


Model::~Model()
{
    if(p_reservoir != 0) delete p_reservoir;

    for(int i = 0; i < m_wells.size(); i++) delete m_wells.at(i);
    for(int i = 0; i < m_pipes.size(); i++) delete m_pipes.at(i);
    for(int i = 0; i < m_separators.size(); i++) delete m_separators.at(i);
}


//-----------------------------------------------------------------------------------------------
// Connects wells and pipes to the outlets
//-----------------------------------------------------------------------------------------------
bool Model::resolvePipeRouting()
{
    cout << "Resolving the pipe routing..." << endl;

    bool ok = true;

    // first cleaning up the current feeds connected to the pipes
    for(int k = 0; k < m_pipes.size(); k++) m_pipes.at(k)->cleanFeedConnections();


    // connecting the wells
    for(int i = 0; i < m_wells.size(); i++)     // looping through each well
    {

        // checking if this is a production well

        ProductionWell *prod_well = dynamic_cast<ProductionWell*>(well(i));

        if(prod_well != 0)     // skipping injection wells
        {

            // looping through the pipe connections for the well
            for(int k = 0; k < prod_well->numberOfPipeConnections(); k++)
            {
                bool connection_ok;
                int pipe_num = prod_well->pipeConnection(k)->pipeNumber();

                for(int j = 0; j < m_pipes.size(); j++)     // looping through the pipes to find the correct one
                {
                    if(m_pipes.at(j)->number() == pipe_num)     // found the correct pipe
                    {
                        m_pipes.at(j)->addFeedWell(prod_well);              // adding the well as a feed to the pipe
                        prod_well->pipeConnection(k)->setPipe(m_pipes.at(j));   // setting the pipe as outlet pipe for the pipe connection

                        connection_ok = true;
                        break;
                    }
                } // pipe for pipe connection k

                // checking if the well - pipe connection was ok
                if(!connection_ok)
                {
                    cout << endl << "###  Runtime Error  ###" << endl
                        << "Well to Pipe connection could not be established..." << endl
                        << "PIPE: " << pipe_num << endl
                        << "WELL: " << prod_well->name().toAscii().data() << endl;

                    exit(1);

                }


            } // pipe connections for well i

            // if the well only has one pipe connection, this should not be considered a variable
            if(prod_well->numberOfPipeConnections() == 1)
            {
                prod_well->pipeConnection(0)->variable()->setIsVariable(false);
                prod_well->pipeConnection(0)->variable()->setValue(1.0);
            }
            else if(prod_well->numberOfPipeConnections() == 0)
            {
                cout << endl << "###  Runtime Error  ###" << endl
                     << "Well " << prod_well->name().toAscii().data() << endl
                     << "Is not connected to any pipe..." << endl << endl;
                exit(1);

            }

        }

    }

    // connecting the pipes
    for(int i = 0; i < m_pipes.size(); i++)     // looping through the pipes
    {
        // only the MidPipe types should be checked
        MidPipe *p_mid = dynamic_cast<MidPipe*>(m_pipes.at(i));

        if(p_mid != 0)  // this is a MidPipe
        {
            // looping through the outlet connections for the pipe
            for(int k = 0; k < p_mid->numberOfOutletConnections(); k++)
            {
                bool pipe_ok = false;

                int pipe_num = p_mid->outletConnection(k)->pipeNumber();

                // checking if it is connected to it self
                if(pipe_num == p_mid->number())
                {
                    cout << endl << "###  Runtime Error  ###" << endl
                         << "Pipe " << pipe_num << " is connected to itself!" << endl
                         << "OUTLETPIPE Connection: " << k << endl << endl;

                    exit(1);
                }

                // looping through the pipes to find the correct one
                for(int j = 0; j < numberOfPipes(); j++)
                {
                    if(pipe_num == pipe(j)->number())
                    {
                        p_mid->outletConnection(k)->setPipe(pipe(j));
                        pipe(j)->addFeedPipe(p_mid);

                        pipe_ok = true;
                        break;
                    }

                }

                // checking if the pipe - pipe connection was ok
                if(!pipe_ok)
                {
                    cout << endl << "###  Runtime Error  ###" << endl
                         << "Pipe to Pipe connection could not be established..." << endl
                         << "UPSTREAM PIPE: " << pipe_num << endl
                         << "DOWNSTREAM PIPE: " << pipe(i)->number() << endl;

                    exit(1);

                }

            } // connection k

            // if the well only has one pipe connection, this should not be considered a variable
            if(p_mid->numberOfOutletConnections() == 1)
            {
                p_mid->outletConnection(0)->variable()->setIsVariable(false);
                p_mid->outletConnection(0)->variable()->setValue(1.0);
            }
            else if(p_mid->numberOfOutletConnections() == 0)
            {
                cout << endl << "###  Runtime Error  ###" << endl
                     << "Pipe " << p_mid->number() << endl
                     << "Is not connected to any upstream pipe..." << endl << endl;
                exit(1);

            }

        } // midpipe
    }   // pipe i


    return ok;
}


//-----------------------------------------------------------------------------------------------
// Calculates the pressures in all the pipes
//-----------------------------------------------------------------------------------------------
bool Model::calculatePipePressures()
{
    bool ok = true;

    // finding the endnodes
    QVector<EndPipe*> end_pipes;

    for(int i = 0; i < numberOfPipes(); i++)
    {
        EndPipe *p = dynamic_cast<EndPipe*>(pipe(i));   // trying to cast as EndPipe
        if(p != 0) end_pipes.push_back(p);              // adding if cast was ok
    }

    // if no endpipes were found, return error
    if(end_pipes.size() == 0)
    {
        cout << endl << "### Warning ###" << endl
             << "From: Model" << endl
             << "No end nodes found in the pipe system..." << endl
             << "At least one PIPE must have an OUTLETPRESSURE defined..." << endl << endl;

        ok = false;
    }
    else    // found end pipes, getting on with the calculations...
    {
        // looping through the end nodes
        for(int i = 0; i < end_pipes.size(); i++)
        {
            // finding the connected wells for this branch
            end_pipes.at(i)->findConnectedWells();

            // calculating the inlet pressures for the end pipes, and the branch of pipes connected
            end_pipes.at(i)->calculateBranchInletPressures();
        }

    }


    return ok;

}

//-----------------------------------------------------------------------------------------------
// Connects separators to the pipes
//-----------------------------------------------------------------------------------------------
bool Model::resolveSeparatorConnections()
{
    cout << "Resolving separator - pipe connections..." << endl;
    bool ok = true;

    for(int i = 0; i < m_separators.size(); i++)        // looping through all separators
    {
        Separator *s = m_separators.at(i);

        for(int j = 0; j < s->numberOfFeedPipeNumbers(); j++)   // looping through all the pipes specified in the driver file
        {
            int pipe_num = s->feedPipeNumber(j);
            bool pipe_ok = false;

            for(int k = 0; k < m_pipes.size(); k++)     // looping through pipes to find the correct one
            {
                if(m_pipes.at(k)->number() == pipe_num) // this is the correct pipe
                {
                    s->addFeedPipe(m_pipes.at(k));

                    pipe_ok = true;
                    break;
                }
            }

            // checking if the pipe number was found
            if(!pipe_ok)
            {
                cout << endl << "###  Runtime Error  ###" << endl
                     << "Separator to Pipe connection could not be established..." << endl
                     << "PIPE: " << pipe_num << endl
                     << "SEPARATOR: " << s->name().toAscii().data() << endl;

                exit(1);

            }

        }
    }

    return ok;
}

//-----------------------------------------------------------------------------------------------
// Updates the separator constraints
//-----------------------------------------------------------------------------------------------
bool Model::updateSeparatorConstraints()
{
    bool ok = true;

    cout << "Updating the separator constraints..." << endl;

    for(int i = 0; i < numberOfSeparators(); i++)
    {
        separator(i)->updateConstraints();
    }

    return ok;
}

//-----------------------------------------------------------------------------------------------
// Updates the Well BHP and connection constraints
//-----------------------------------------------------------------------------------------------
bool Model::updateWellConstaints()
{
    bool ok = true;

    cout << "Updating the well BHP and connection constraints" << endl;

    for(int i = 0; i < numberOfWells(); i++)
    {
        // checking if this is a production well
        ProductionWell *prod_well = dynamic_cast<ProductionWell*>(well(i));

        if(prod_well != 0)
        {
            prod_well->updateBhpConstraint();
            prod_well->updatePipeConnectionConstraint();
        }
    }

    return ok;
}

//-----------------------------------------------------------------------------------------------
// Updates the Pipe connection constraints
//-----------------------------------------------------------------------------------------------
bool Model::updatePipeConstraints()
{
    bool ok = true;

    for(int i = 0; i < numberOfPipes(); i++)
    {
        // checking if this is a mid pipe
        MidPipe *p_mid = dynamic_cast<MidPipe*>(pipe(i));
        if(p_mid != 0)
        {
            p_mid->updateOutletConnectionConstraint();
        }
    }

    return ok;
}

//-----------------------------------------------------------------------------------------------
// Updates all the constraints in the model
//-----------------------------------------------------------------------------------------------
bool Model::updateConstraints()
{
    bool ok = true;

    if(!updateSeparatorConstraints()) ok = false;
    if(!updateWellConstaints()) ok = false;
    if(!updatePipeConstraints()) ok = false;

    return ok;
}


//-----------------------------------------------------------------------------------------------
// Reads all the pipe pressure drop files
//-----------------------------------------------------------------------------------------------
void Model::readPipeFiles()
{
    for(int i = 0; i < numberOfPipes(); i++)
    {
        pipe(i)->readInputFile();
    }
}


//-----------------------------------------------------------------------------------------------
// Collects all the binary variables
//-----------------------------------------------------------------------------------------------
QVector<shared_ptr<BinaryVariable> >& Model::binaryVariables()
{


    if(m_vars_binary.size() == 0)
    {

        // finding well routnig variables
        for(int i = 0; i < numberOfWells(); i++)
        {
            // checking if this is a production well
            ProductionWell* prod_well = dynamic_cast<ProductionWell*>(well(i));

            if(prod_well != 0)
            {
                // looping through the pipe connections
                for(int j = 0; j < prod_well->numberOfPipeConnections(); j++)
                {
                    if(prod_well->pipeConnection(j)->variable()->isVariable()) m_vars_binary.push_back(prod_well->pipeConnection(j)->variable());
                }
            }

        }



        // finding pipe routing variables
        for(int j = 0; j < m_pipes.size(); j++)
        {

            MidPipe *p_mid = dynamic_cast<MidPipe*>(m_pipes.at(j));     // end pipes do not have routing

            if(p_mid != 0)
            {
                // looping through the outlet connections
                for(int j = 0; j < p_mid->numberOfOutletConnections(); j++)
                {
                    if(p_mid->outletConnection(j)->variable()->isVariable()) m_vars_binary.push_back(p_mid->outletConnection(j)->variable());
                }

            }
        }
    }


    return m_vars_binary;
}


//-----------------------------------------------------------------------------------------------
// Collects all the real variables
//-----------------------------------------------------------------------------------------------
QVector<shared_ptr<RealVariable> >& Model::realVariables()
{

    if(m_vars_real.size() == 0)
    {

        for(int i = 0; i < m_wells.size(); i++)     // looping through all the wells
        {
            Well *w = m_wells.at(i);

            for(int j = 0; j < w->numberOfControls(); j++)  // looping through each element in the wells schedule
            {
                // checking if this shcedule entry is a variable
                if(w->control(j)->controlVar()->isVariable()) m_vars_real.push_back(w->control(j)->controlVar());
            }

        }

    }
    return m_vars_real;
}

//-----------------------------------------------------------------------------------------------
// Collects all the constraints
//-----------------------------------------------------------------------------------------------
QVector<shared_ptr<Constraint> >& Model::constraints()
{
    if(m_cons.size() == 0)
    {

        // getting the well bhp constraints
        for(int i = 0; i < numberOfWells(); i++)
        {
            // checking if this is a production well
            ProductionWell* prod_well = dynamic_cast<ProductionWell*>(well(i));

            if(prod_well != 0) m_cons.push_back(prod_well->bhpConstraint());
        }

        // getting the well pipe connection constraints
        for(int i = 0; i < numberOfWells(); i++)
        {
            // checking if this is a production well
            ProductionWell* prod_well = dynamic_cast<ProductionWell*>(well(i));

            if(prod_well != 0) m_cons.push_back(prod_well->pipeConnectionConstraint());
        }

        // getting the mid pipe connection constraints
        for(int i = 0; i < numberOfPipes(); i++)
        {
            // checking if this is a mid pipe
            MidPipe *p_mid = dynamic_cast<MidPipe*>(pipe(i));

            if(p_mid != 0) m_cons.push_back(p_mid->outletConnectionConstraint());
        }



        // getting the separator capacity constraints
        for(int i = 0; i < numberOfSeparators(); i++)
        {
            Separator *sep = separator(i);

            // only adding constraints if defined
            if(sep->gasConstraint() != 0) m_cons.push_back(sep->gasConstraint());
            if(sep->oilConstraint() != 0) m_cons.push_back(sep->oilConstraint());
            if(sep->waterConstraint() != 0) m_cons.push_back(sep->waterConstraint());
            if(sep->liquidConstraint() != 0) m_cons.push_back(sep->liquidConstraint());
        }
    }


    return m_cons;
}


//-----------------------------------------------------------------------------------------------
// Updates the value of the objective
//-----------------------------------------------------------------------------------------------
void Model::updateObjectiveValue()
{
    // first adding together the streams from all the wells
    QVector<Stream*> field_rates;

    for(int i = 0; i < m_wells.at(0)->numberOfStreams(); i++)   // looping through the time steps
    {
        Stream *s = new Stream();

        for(int j = 0; j < numberOfWells(); j++)    // looping through the wells
        {
            if(well(j)->type() == Well::PROD) *s += *well(j)->stream(i);
        }

        field_rates.push_back(s);
    }

    // calculating the new objective value
    objective()->calculateValue(field_rates);

    cout << "Objective value = " << objective()->value() << endl;

    // deleting the generated streams
    for(int i = 0; i < field_rates.size(); i++) delete field_rates.at(i);
}

} // namespace ResOpt
