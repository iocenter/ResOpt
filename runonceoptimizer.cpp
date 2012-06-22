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


#include "runonceoptimizer.h"

#include <iostream>
#include "runner.h"
#include "case.h"
#include "casequeue.h"

#include "model.h"
#include "realvariable.h"
#include "binaryvariable.h"
#include "constraint.h"
#include "objective.h"
#include <QVector>

#include <tr1/memory>



using std::tr1::shared_ptr;



using std::cout;
using std::endl;

namespace ResOpt
{


RunonceOptimizer::RunonceOptimizer(Runner *r)
    : Optimizer(r)
{
}


//-----------------------------------------------------------------------------------------------
// Initializes the optimizer
//-----------------------------------------------------------------------------------------------
void RunonceOptimizer::initialize()
{
    // nothing to do here

    cout << "Initializing RUN_ONCE..." << endl;
}


//-----------------------------------------------------------------------------------------------
// Starts the optimizer
//-----------------------------------------------------------------------------------------------
void RunonceOptimizer::start()
{
    cout << "Starting RUN_ONCE..." << endl;

    // creating a case for the current values in the model
    CaseQueue *cases = new CaseQueue();

    cases->push_back(new Case(runner()->model()));  // this creates a case with the varaible values currently in the model.


    // launching the runner on the case
    runCases(cases);


    delete cases;

    // testing out the gradients code

    /*
    calculateGradients();



    // creating a case for the current values in the model
    CaseQueue *cases = new CaseQueue();

    cases->push_back(new Case(runner()->model()));  // this creates a case with the varaible values currently in the model.


    // debug, testing to run two parallel runs
    Case *test_case = new Case(runner()->model());
    test_case->setRealVariableValue(0, test_case->realVariableValue(0)+10);
    cases->push_back(test_case);

    // launching the runner on the case
    runCases(cases);


    delete cases;
    */

    // letting the runner know that the optimization has finished
    emit finished();

    cout << "Optimization finished..." << endl;
}


//-----------------------------------------------------------------------------------------------
// Calculates the gradients, test code
//-----------------------------------------------------------------------------------------------
void RunonceOptimizer::calculateGradients()
{

    cout.precision(9);

    // getting the variable vectors from the model
    QVector<shared_ptr<RealVariable> > real_vars = runner()->model()->realVariables();
    QVector<shared_ptr<BinaryVariable> > binary_vars = runner()->model()->binaryVariables();


    // setting up the base case
    Case *base_case = new Case(runner()->model());



    // adding the case to a queue

    CaseQueue *base_case_queue = new CaseQueue();

    base_case_queue->push_back(base_case);


    // sending the new case to the runner

    runCases(base_case_queue);




    // setting up the case queue with all the perturbations
    CaseQueue *case_queue = new CaseQueue();

    // adding the real variable perturbations
    for(int i = 0; i < real_vars.size(); ++i)
    {
        // calculating the perturbed value of the variable
        double x_perturbed = perturbedVariableValue(base_case->realVariableValue(i), real_vars.at(i)->max(), real_vars.at(i)->min());

        // setting up a new case
        Case *case_perturbed = new Case(*base_case);

        // changing the value of the variable to the perturbe value
        case_perturbed->setRealVariableValue(i, x_perturbed);

        // adding the case to the queue
        case_queue->push_back(case_perturbed);
    }

    // adding the binary variable perturbations
    for(int i = 0; i < binary_vars.size(); ++i)
    {
        // calculating the perturbed value of the variable
        double x_perturbed = perturbedVariableValue(base_case->binaryVariableValue(i), binary_vars.at(i)->max(), binary_vars.at(i)->min());

        // setting up a new case
        Case *case_perturbed = new Case(*base_case);

        // changing the value of the variable to the perturbe value
        case_perturbed->setBinaryVariableValue(i, x_perturbed);

        // adding the case to the queue
        case_queue->push_back(case_perturbed);
    }

    // sending the cases to the runner for evaluation
    runCases(case_queue);



    int n_var = 0;

    // calculating the gradients of the real variables
    for(int i = 0; i < base_case->numberOfRealVariables(); ++i)
    {
        // calculating the perturbation size of the variable
        double dx = base_case->realVariableValue(i) - case_queue->at(n_var)->realVariableValue(i);

        // calculating the gradient of the objective
        double dfdx = -(base_case->objectiveValue() - case_queue->at(n_var)->objectiveValue()) / dx;

        // setting the value to the objective gradient vector
        //m_grad_f.replace(n_var, dfdx);
        cout << "x_base = " << base_case->realVariableValue(i) << endl;
        cout << "x_pert = " << case_queue->at(n_var)->realVariableValue(i) << endl;
        cout << "f_base = " << base_case->objectiveValue() << endl;
        cout << "f_pert = " << case_queue->at(n_var)->objectiveValue() << endl;


        cout << "df/dx" << n_var << " = " << dfdx << endl;

        // calculating the gradients of the constraints
        int entry = n_var*base_case->numberOfConstraints();
        for(int j = 0; j < base_case->numberOfConstraints(); ++j)
        {
            double dcdx = (base_case->constraintValue(j) - case_queue->at(n_var)->constraintValue(j)) / dx;

            //m_jac_g.replace(entry, dcdx);

            cout << "dc" << j << "/dx" << n_var << " = " << dcdx  << ", entry = " << entry << endl;

            ++entry;


        }


        ++n_var;
    }

    // calculating the gradients of the binary variables
    for(int i = 0; i < base_case->numberOfBinaryVariables(); ++i)
    {
        // calculating the perturbation size of the variable
        double dx = base_case->binaryVariableValue(i) - case_queue->at(n_var)->binaryVariableValue(i);

        // calculating the gradient of the objective
        double dfdx = -(base_case->objectiveValue() - case_queue->at(n_var)->objectiveValue()) / dx;

        // setting the value to the objective gradient vector
        //m_grad_f.replace(n_var, dfdx);
        cout << "df/dx" << n_var << " = " << dfdx << endl;


        // calculating the gradients of the constraints
        int entry = n_var*base_case->numberOfConstraints();
        for(int j = 0; j < base_case->numberOfConstraints(); ++j)
        {
            double dcdx = (base_case->constraintValue(j) - case_queue->at(n_var)->constraintValue(j)) / dx;
            //m_jac_g.replace(entry, dcdx);
            cout << "dc" << j << "/dx" << n_var << " = " << dcdx << endl;
            ++entry;


        }


        ++n_var;
    }

    // deleting the perturbed cases
    for(int i = 0; i < case_queue->size(); ++i) delete case_queue->at(i);
    delete case_queue;



}

//-----------------------------------------------------------------------------------------------
// Calculates the perturbated value  a variable
//-----------------------------------------------------------------------------------------------
double RunonceOptimizer::perturbedVariableValue(double value, double max, double min)
{
    double x_perturbed;

    double span = max - min;
    double l_slack = value - min;
    double u_slack = max - value;

    if(u_slack >= l_slack)
    {
        x_perturbed = value + pertrurbationSize() * span;
        if(x_perturbed > max) x_perturbed = max;
    }
    else
    {
        x_perturbed = value - pertrurbationSize() * span;
        if(x_perturbed < min) x_perturbed = min;
    }

    return x_perturbed;

}



} // namespace ResOpt
