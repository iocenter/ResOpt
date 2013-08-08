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
#include "case.h"
#include "derivative.h"
#include "constraint.h"

#include <iostream>

using std::cout;
using std::endl;

namespace ResOpt
{

AdjointsCoupledModel::AdjointsCoupledModel() :
    m_perturbation(0.001),
    p_results(0)
{
}

AdjointsCoupledModel::AdjointsCoupledModel(const AdjointsCoupledModel &m)
    : CoupledModel(m)
{
    //int m_perturbation;
    //Case *p_results;

    //QVector<AdjointCollection*> m_adjoint_collections;

    m_perturbation = m.m_perturbation;

    p_results = 0;


    // doing adjoints specific initialization

    for(int i = 0; i < numberOfWells(); ++i)
    {
        for(int j = 0; j < well(i)->numberOfControls(); ++j)
        {
            AdjointCollection *ac = new AdjointCollection();
            ac->setVariable(well(i)->control(j)->controlVar());

            for(int k = 0; k < numberOfMasterScheduleTimes(); ++k)
            {
                // looping through each well
                for(int l = 0; l < numberOfWells(); ++l)
                {
                    ac->addAdjoint(new Adjoint(well(l), k));
                }
            }

            m_adjoint_collections.push_back(ac);
        }
    }



}


AdjointsCoupledModel::~AdjointsCoupledModel()
{
    if(p_results != 0) delete p_results;

    for(int i = 0; i < m_adjoint_collections.size(); ++i) delete m_adjoint_collections.at(i);
}

//-----------------------------------------------------------------------------------------------
// initialize the model
//-----------------------------------------------------------------------------------------------
void AdjointsCoupledModel::initialize()
{
    // calling the base class
    CoupledModel::initialize();

    // doing adjoints specific initialization

    for(int i = 0; i < numberOfWells(); ++i)
    {
        for(int j = 0; j < well(i)->numberOfControls(); ++j)
        {
            AdjointCollection *ac = new AdjointCollection();
            ac->setVariable(well(i)->control(j)->controlVar());

            for(int k = 0; k < numberOfMasterScheduleTimes(); ++k)
            {
                // looping through each well
                for(int l = 0; l < numberOfWells(); ++l)
                {
                    ac->addAdjoint(new Adjoint(well(l), k));
                }
            }

            m_adjoint_collections.push_back(ac);
        }
    }



}

//-----------------------------------------------------------------------------------------------
// processes the model after the reservoir simulator is run
//-----------------------------------------------------------------------------------------------
void AdjointsCoupledModel::process()
{
    /*
    // test printing adjoints
    for(int i = 0; i < m_adjoint_collections.size(); ++i)
    {
        AdjointCollection *ac = m_adjoint_collections.at(i);
        cout << "------------------------------------------------------" << endl;
        cout << "Adjoints for variable: " << ac->variable()->name().toLatin1().constData() << endl;
        cout << "------------------------------------------------------" << endl;
        cout << "number of adjoints = " << ac->numberOfAdjoints() << endl;

        for(int j = 0; j < ac->numberOfAdjoints(); ++j)
        {
            //cout << "adjoint # " << j+1 << endl;
            Adjoint *a = ac->adjoint(j);
            cout << "well: " << a->well()->name().toLatin1().constData() << ", time = " << masterScheduleTime(a->time()) << endl;
            cout << "  dp/dx = " << a->dpDx();
            cout << ", dqo/dx = " << a->dqoDx();
            cout << ", dqg/dx = " << a->dqgDx();
            cout << ", dqw/dx = " << a->dqwDx() << endl;

        }

        cout << endl;

    }

    */


    // running perturbations
    QVector<Case*> cases;
    for(int i = 0; i < realVariables().size(); ++i)
    {
        Case *c = processPerturbation(realVariables().at(i));
        cases.push_back(c);
    }

    // running base case
    Case *base_case = processBaseCase();



    // setting up empty derivatives in the case
    base_case->setObjectiveDerivative(new Derivative());
    for(int i = 0; i < constraints().size(); ++i)
    {
        base_case->addConstraintDerivative(new Derivative(constraints().at(i)->id()));
    }

    // calculating derivatives from perturbed cases
    for(int i = 0; i < numberOfRealVariables(); ++i)
    {
        Case *case_perturb = cases.at(i);
        int var_id = realVariables().at(i)->id();

        // calculating partial derivatives for constraints
        for(int j = 0; j < numberOfConstraints(); ++j)
        {
            // dC_j / dx_i
            double dcdx = (case_perturb->constraintValue(j) - base_case->constraintValue(j)) / (case_perturb->realVariableValue(i) - base_case->realVariableValue(i));

            // adding to base case
            base_case->constraintDerivative(j)->addPartial(var_id, dcdx);
        }

        // calculating partial derivative for objective
        double eps_x = (realVariables().at(i)->max() - realVariables().at(i)->min()) * m_perturbation;
        double dfdx = (case_perturb->objectiveValue() - base_case->objectiveValue()) / eps_x;
        //double dfdx = (case_perturb->objectiveValue() - base_case->objectiveValue()) / (case_perturb->realVariableValue(i) - base_case->realVariableValue(i));
        base_case->objectiveDerivative()->addPartial(var_id, dfdx);
    }

    //base_case->printToCout();

    // setting the results
    p_results = base_case;

    // deleting perturbation cases
    for(int i = 0; i < cases.size(); ++i) delete cases.at(i);

    // updating the status of the model
    setUpToDate(true);

    //cout << "done processing adjoints coupled model..." << endl;

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
// processes the model with a perturbation of v, returns a case with constraint and obj values
//-----------------------------------------------------------------------------------------------
Case* AdjointsCoupledModel::processPerturbation(shared_ptr<RealVariable> v)
{
    //cout << "-------- processing perturbation -----------" << endl;

    // calculating the perturbation size of the variable
    double eps_x = (v->max() - v->min()) * m_perturbation;

    //cout << "variable: " << v->name().toLatin1().constData() << endl;
    //cout << "eps_x        = " << eps_x << endl;


    // checking if there is an adjoints collection for the variable
    AdjointCollection *ac = adjointCollection(v);

    if(ac != 0)
    {
        // setting the pertrubed values for all streams that have adjoints wrt. the variable
        ac->perturbStreams(eps_x);
    }

    if(v != 0)
    {
        // changing the value of the variable to the pertrubed value
        // could be variable connected to separator or booster...
        v->setValue(v->value() + eps_x);
    }

    // ------- processing the model with the perturbed values ----------

    // update the streams in the pipe network
    updateStreams();

    // calculating pressures in the Pipe network
    calculatePipePressures();

    // updating the constraints (this must be done after the pressure calc)
    updateConstraints();

    // updating the objective
    updateObjectiveValue();

    // ----- generating case based on current model state -------

    Case *c = new Case(this, true);


    // resetting the variable value
    v->setValue(v->value() - eps_x);


    // resetting streams if changed
    if(ac != 0) ac->perturbStreams(-eps_x);



    return c;

}

//-----------------------------------------------------------------------------------------------
// processes the model with base case values
//-----------------------------------------------------------------------------------------------
Case* AdjointsCoupledModel::processBaseCase()
{
    cout << "-------- processing base case -----------" << endl;


    // update the streams in the pipe network
    updateStreams();

    // calculating pressures in the Pipe network
    calculatePipePressures();

    // updating the constraints (this must be done after the pressure calc)
    updateConstraints();

    // updating the objective
    updateObjectiveValue();


    return new Case(this, true);

}


} // namespace
