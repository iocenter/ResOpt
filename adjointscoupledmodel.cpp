/*
 * This file is part of the ResOpt project.
 *
 * Copyright (C) 2011-2013 Aleksander O. Juell <aleksander.juell@ntnu.no>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
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


#include "adjointscoupledmodel.h"
#include "adjointcollection.h"
#include "adjoint.h"
#include "well.h"

namespace ResOpt
{

AdjointsCoupledModel::AdjointsCoupledModel() :
    m_adjoint_level(1)
{
}

//-----------------------------------------------------------------------------------------------
// initialize the model
//-----------------------------------------------------------------------------------------------
void AdjointsCoupledModel::initialize()
{
    // calling the base class
    CoupledModel::initialize();

    // doing adjoints specific initialization

    if(m_adjoint_level == 1)    // adjoints only for the same timestep and well
    {
        for(int i = 0; i < numberOfWells(); ++i)
        {

            for(int j = 0; j < well(i)->numberOfControls(); ++j)
            {

                AdjointCollection *ac = new AdjointCollection();
                ac->setVariable(well(i)->control(j)->controlVar());

                ac->addAdjoint(new Adjoint(well(i), well(i)->stream(j)));

                m_adjoint_collections.push_back(ac);
            }

        }
    }

    else if(m_adjoint_level == 2) // adjoints for all streams in a given well
    {
        for(int i = 0; i < numberOfWells(); ++i)
        {
            for(int j = 0; j < well(i)->numberOfControls(); ++j)
            {
                AdjointCollection *ac = new AdjointCollection();
                ac->setVariable(well(i)->control(j)->controlVar());

                for(int k = j; k < well(i)->numberOfStreams(); ++k)
                {
                    ac->addAdjoint(new Adjoint(well(i), well(i)->stream(k)));
                }

                m_adjoint_collections.push_back(ac);


            }
        }
    }

    else    // adjoints for all wells, all streams
    {
        for(int i = 0; i < numberOfWells(); ++i)
        {
            for(int j = 0; j < well(i)->numberOfControls(); ++j)
            {
                AdjointCollection *ac = new AdjointCollection();
                ac->setVariable(well(i)->control(j)->controlVar());

                for(int k = j; k < numberOfMasterScheduleTimes(); ++k)
                {
                    // looping through each well
                    for(int l = 0; l < numberOfWells(); ++l)
                    {
                        ac->addAdjoint(new Adjoint(well(l), well(l)->stream(k)));
                    }
                }

            }
        }
    }


}

//-----------------------------------------------------------------------------------------------
// processes the model after the reservoir simulator is run
//-----------------------------------------------------------------------------------------------
void AdjointsCoupledModel::process()
{
    // update the streams in the pipe network
    updateStreams();


    // calculating pressures in the Pipe network
    calculatePipePressures();


    // updating the constraints (this must be done after the pressure calc)
    updateConstraints();

    // updating the objective
    updateObjectiveValue();


    // updating the status of the model
    setUpToDate(true);

}


//-----------------------------------------------------------------------------------------------
// returns the Adjoints asociated with a real variable
//-----------------------------------------------------------------------------------------------
AdjointCollection* AdjointsCoupledModel::adjointCollection(shared_ptr<RealVariable> v)
{
    for(int i = 0; i < m_adjoint_collections.size(); ++i)
    {
        if(m_adjoint_collections.at(i)->variable() == v) return m_adjoint_collections.at(i);
    }

    return 0;
}

//-----------------------------------------------------------------------------------------------
// returns the Adjoint for Stream s wrt. variable v
//-----------------------------------------------------------------------------------------------
Adjoint* AdjointsCoupledModel::adjoint(shared_ptr<RealVariable> v, Stream *s)
{
    // first finding the correct collection
    AdjointCollection *ac = adjointCollection(v);

    // then finding the correct adjoint
    if(ac != 0)
    {
        for(int i = 0; i < ac->numberOfAdjoints(); ++i)
        {
            if(ac->adjoint(i)->stream() == s) return ac->adjoint(i);
        }
    }

    return 0;
}


} // namespace
