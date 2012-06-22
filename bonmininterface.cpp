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


#include "bonmininterface.h"

#include <iostream>
#include <QVector>

#include "bonminoptimizer.h"
#include "runner.h"
#include "model.h"
#include "realvariable.h"
#include "binaryvariable.h"
#include "constraint.h"
#include "objective.h"
#include "case.h"
#include "casequeue.h"



using std::cout;
using std::endl;


namespace ResOpt
{

BonminInterface::BonminInterface(BonminOptimizer *o)
    : p_optimizer(o),
      p_case_last(0),
      p_case_gradients(0)

{
    m_vars_binary = p_optimizer->runner()->model()->binaryVariables();
    m_vars_real = p_optimizer->runner()->model()->realVariables();
    m_cons = p_optimizer->runner()->model()->constraints();
}


//-----------------------------------------------------------------------------------------------
// Returns the variable types to Bonmin
//-----------------------------------------------------------------------------------------------
bool BonminInterface::get_variables_types(Index n, VariableType* var_types)
{
    assert(n == (m_vars_real.size() + m_vars_binary.size()));

    // the real (contineous) variables. this includes the well control variables
    int n_var = 0;
    for(int i = 0; i < m_vars_real.size(); i++)
    {
        var_types[i] = CONTINUOUS;
        n_var++;
    }

    // the binary variables. this includes the routing vars
    for(int i = 0; i < m_vars_binary.size(); i++)
    {
        var_types[i] = BINARY;
        n_var++;
    }



    return true;
}

//-----------------------------------------------------------------------------------------------
// Returns the variable liniarity
//-----------------------------------------------------------------------------------------------
bool BonminInterface::get_variables_linearity(Index n, Ipopt::TNLP::LinearityType* var_types)
{
    assert(n == (m_vars_real.size() + m_vars_binary.size()));

    // the real (contineous) variables. this includes the well control variables
    int n_var = 0;
    for(int i = 0; i < m_vars_real.size(); i++)
    {
        var_types[i] = Ipopt::TNLP::NON_LINEAR;
        n_var++;
    }

    // the integer variables. this includes the routing vars
    for(int i = 0; i < m_vars_binary.size(); i++)
    {
        var_types[i] = Ipopt::TNLP::NON_LINEAR;
        n_var++;
    }




    return true;
}


//-----------------------------------------------------------------------------------------------
// Retruns the constraint liniarity
//-----------------------------------------------------------------------------------------------
bool BonminInterface::get_constraints_linearity(Index m, Ipopt::TNLP::LinearityType* const_types)
{
    assert(m_cons.size() == m);

    for(int i = 0; i < m_cons.size(); i++)
    {
        const_types[i] = Ipopt::TNLP::NON_LINEAR;
    }

    return true;
}


//-----------------------------------------------------------------------------------------------
// Passes the dimensions of the problem to Bonmin
//-----------------------------------------------------------------------------------------------
bool BonminInterface::get_nlp_info(Index& n, Index& m, Index& nnz_jac_g,
                          Index& nnz_h_lag, TNLP::IndexStyleEnum& index_style)
{
    cout << "Giving bonmin the dimensions of the problem..." << endl;

    n = m_vars_binary.size() + m_vars_real.size();     // number of variables
    m = m_cons.size();                              // number of constraints

    nnz_jac_g = m*n;        // dense Jacobian
    nnz_h_lag = n*(n+1)/2;  // dense, symmetric Hessian

    index_style = TNLP::C_STYLE;    // c style numbering, starting at 0

    cout << "Number of variables   = " << n << endl;
    cout << "Number of constraints = " << m << endl;


    return true;
}


//-----------------------------------------------------------------------------------------------
// Passes variable and constraint bounds
//-----------------------------------------------------------------------------------------------
bool BonminInterface::get_bounds_info(Index n, Number* x_l, Number* x_u,
                             Index m, Number* g_l, Number* g_u)
{
    assert(n == (m_vars_real.size() + m_vars_binary.size()));
    assert(m == m_cons.size());

    int n_var = 0;  // variable index used by the optimizer (first real, then int)

    // setting the real variable bounds
    for(int i = 0; i < m_vars_real.size(); i++)
    {
        x_l[n_var] = m_vars_real.at(i)->min();  // lower bound
        x_u[n_var] = m_vars_real.at(i)->max();	// upper bound

        n_var++;
    }

    //setting integer variable bounds
    for(int i = 0; i < m_vars_binary.size(); i++)
    {
        x_l[n_var] = m_vars_binary.at(i)->min();   // lower bound
        x_u[n_var] = m_vars_binary.at(i)->max();	// upper bound

        n_var++;
    }

    // setting the constraint bounds
    for(int i = 0; i < m_cons.size(); i++)
    {
        g_l[i] = m_cons.at(i)->min();   // lower bound
        g_u[i] = m_cons.at(i)->max();   // upper bound
    }


    return true;
}

//-----------------------------------------------------------------------------------------------
// Passes the starting points
//-----------------------------------------------------------------------------------------------
bool BonminInterface::get_starting_point(Index n, bool init_x, Number* x,
                                bool init_z, Number* z_L, Number* z_U,
                                Index m, bool init_lambda,
                                Number* lambda)
{
    assert(n == (m_vars_real.size() + m_vars_binary.size()));

    // only expect to initialize x
    assert(init_x);
    assert(!init_z);
    assert(!init_lambda);


    cout << "Giving bonmin the starting point..." << endl;

    int n_var = 0;  // variable index used by the optimizer (first real, then binary)

    // setting the real variable starting points
    for(int i = 0; i < m_vars_real.size(); i++)
    {
        x[n_var] = m_vars_real.at(i)->value();  // current value = starting point

        n_var++;
    }

    //setting binary variable starting points
    for(int i = 0; i < m_vars_binary.size(); i++)
    {
        x[n_var] = m_vars_binary.at(i)->value();  // current value = starting point

        n_var++;
    }



    return true;
}


//-----------------------------------------------------------------------------------------------
// Calculates the objective
//-----------------------------------------------------------------------------------------------
bool BonminInterface::eval_f(Index n, const Number* x, bool new_x, Number& obj_value)
{

    //cout << "Evaluating the function for bonmin..." << endl;

    // checking if this is a new set of variable values
    if(newVariableValues(n, x))
    {
        // deleting the old case
        if(p_case_last != 0) delete p_case_last;
        p_case_last = 0;

        // creating a new case
        Case *case_new = generateCase(n, x);

        // adding the case to a queue
        CaseQueue *case_queue = new CaseQueue();
        case_queue->push_back(case_new);

        // sending the new case to the runner
        p_optimizer->runCases(case_queue);

        // setting the case as the last case
        p_case_last = case_new;


    }

    // getting the value of the objective (negative since bonmin is doing minimization)
    obj_value = -p_case_last->objectiveValue();

    //cout << "f = " << obj_value << endl;


    return true;
}

//-----------------------------------------------------------------------------------------------
// Calculates the derivatives of the objective
//-----------------------------------------------------------------------------------------------
bool BonminInterface::eval_grad_f(Index n, const Number* x, bool new_x, Number* grad_f)
{
   // cout << "Evaluating the gradient for bonmin..." << endl;

    // first checking if gradients are allready calculated
    if(!gradientsAreUpdated(n,x))
    {
        cout << "need to calculate new gradients..." << endl;
        calculateGradients(n,x);
        cout << "done calculating new gradients..." << endl;
    }

    // copying the calculated gradients to BonMin
    for(int i = 0; i < n; i++)
    {
        grad_f[i] = m_grad_f.at(i);

       // cout << "grad_f[" << i << "] = " << grad_f[i] << endl;
    }

    return true;
}


//-----------------------------------------------------------------------------------------------
// Passes the constraint values to Bonmin
//-----------------------------------------------------------------------------------------------
bool BonminInterface::eval_g(Index n, const Number* x, bool new_x, Index m, Number* g)
{
    //cout << "Evaluating the constraints for bonmin..." << endl;

    // checking if this is a new set of variable values
    if(newVariableValues(n, x))
    {
        // deleting the old case
        if(p_case_last != 0) delete p_case_last;
        p_case_last = 0;

        // creating a new case
        Case *case_new = generateCase(n, x);

        // adding the case to a queue
        CaseQueue *case_queue = new CaseQueue();
        case_queue->push_back(case_new);

        // sending the new case to the runner
        p_optimizer->runCases(case_queue);

        // setting the case as the last case
        p_case_last = case_new;

    }

    // checking that the number of constraints in the cas corresponds to m
    assert(m == p_case_last->numberOfConstraints());

    // getting the value of the constraints
    for(int i = 0; i < p_case_last->numberOfConstraints(); ++i)
    {
        g[i] = p_case_last->constraintValue(i);

        //cout << "g[" << i << "] = " << g[i] << endl;
    }

    return true;
}


//-----------------------------------------------------------------------------------------------
// Passes the Jacobian to Bonmin
//-----------------------------------------------------------------------------------------------
bool BonminInterface::eval_jac_g(Index n, const Number* x, bool new_x,
                        Index m, Index nele_jac, Index* iRow, Index *jCol,
                        Number* values)
{
    // checking if the structure of the jacobian has been set
    if (values == NULL)
    {
        cout << "Giving bonmin the structure of the jacobian..." << endl;

        int entry = 0;
        for ( int col = 0; col < n; col++ )
        {
            for ( int row = 0; row < m; row++)
            {
                iRow[entry] = row;
                jCol[entry] = col;

                entry++;
            }
        }
    }
    else    // the structure is already set, getting the values
    {
        //cout << "Evaluating the jacobian for bonmin..." << endl;

        // checking if gradients are calculated
        if(!gradientsAreUpdated(n,x))
        {
            calculateGradients(n,x);
        }

        // copying gradients to BonMin
        for(int i = 0; i < nele_jac; i++)
        {
            values[i] = m_jac_g.at(i);
        }


    }

    return true;
}



//-----------------------------------------------------------------------------------------------
// Passes the Hessian to Bonmin
//-----------------------------------------------------------------------------------------------
bool BonminInterface::eval_h(Index n, const Number* x, bool new_x,
                    Number obj_factor, Index m, const Number* lambda,
                    bool new_lambda, Index nele_hess, Index* iRow,
                    Index* jCol, Number* values)
{
    cout << "Evaluating the hessian for bonmin..." << endl;
    return true;
}


//-----------------------------------------------------------------------------------------------
// Called by Ipopt when finished
//-----------------------------------------------------------------------------------------------
void BonminInterface::finalize_solution(TMINLP::SolverReturn status,
                               Index n, const Number* x, Number obj_value)
{


    cout << "IPOPT returned a status indicating ";

    if ( status == Ipopt::SUCCESS ) cout << "SUCCESS";
    else if ( status == Ipopt::MAXITER_EXCEEDED ) cout << "MAXITER_EXCEEDED";
    else if ( status == Ipopt::CPUTIME_EXCEEDED ) cout << "CPUTIME_EXCEEDED";
    else if ( status == Ipopt::STOP_AT_TINY_STEP ) cout << "STOP_AT_TINY_STEP";
    else if ( status == Ipopt::STOP_AT_ACCEPTABLE_POINT ) cout << "STOP_AT_ACCEPTABLE_POINT";
    else if ( status == Ipopt::LOCAL_INFEASIBILITY ) cout << "LOCAL_INFEASIBILITY";
    else if ( status == Ipopt::USER_REQUESTED_STOP ) cout << "USER_REQUESTED_STOP";
    else if ( status == Ipopt::FEASIBLE_POINT_FOUND ) cout << "FEASIBLE_POINT_FOUND";
    else if ( status == Ipopt::DIVERGING_ITERATES ) cout << "DIVERGING_ITERATES";
    else if ( status == Ipopt::RESTORATION_FAILURE ) cout << "RESTORATION_FAILURE";
    else if ( status == Ipopt::ERROR_IN_STEP_COMPUTATION ) cout << "ERROR_IN_STEP_COMPUTATION";
    else if ( status == Ipopt::INVALID_NUMBER_DETECTED ) cout << "INVALID_NUMBER_DETECTED";
    else if ( status == Ipopt::TOO_FEW_DEGREES_OF_FREEDOM ) cout << "TOO_FEW_DEGREES_OF_FREEDOM";
    else if ( status == Ipopt::INVALID_OPTION ) cout << "INVALID_OPTION";
    else if ( status == Ipopt::OUT_OF_MEMORY ) cout << "OUT_OF_MEMORY";
    else if ( status == Ipopt::INTERNAL_ERROR ) cout << "INTERNAL_ERROR";
    else cout << "UNKNOWN_ERROR";

    cout << endl;
}


//-----------------------------------------------------------------------------------------------
// Checks if the x values are the same as the current model values for the variables
//-----------------------------------------------------------------------------------------------
bool BonminInterface::newVariableValues(Index n, const Number *x)
{
    bool x_new = false;

    // checking if the last case has been set up
    if(p_case_last == 0) return true;

    // checking that the number of variables in the case correspons to n
    int n_var_case = p_case_last->numberOfRealVariables() + p_case_last->numberOfBinaryVariables();
    if(n != n_var_case) return false;

    // checking if the real variables are the same
    int n_var = 0;

    for(int i = 0; i < p_case_last->numberOfRealVariables(); ++i)
    {
        if(x[n_var] != p_case_last->realVariableValue(i))
        {
            x_new = true;
            break;
        }

        ++n_var;
    }

    // checking the binary variables, if nothing new was detected with the real vars
    if(!x_new)
    {
        for(int i = 0; i < p_case_last->numberOfBinaryVariables(); ++i)
        {
            if(x[n_var] != p_case_last->binaryVariableValue(i))
            {
                x_new = true;
                break;
            }

            ++n_var;
        }
    }


    return x_new;
}

//-----------------------------------------------------------------------------------------------
// Calculates the perturbated value  a variable
//-----------------------------------------------------------------------------------------------
double BonminInterface::perturbedVariableValue(double value, double max, double min)
{
    double x_perturbed;

    double span = max - min;
    double l_slack = value - min;
    double u_slack = max - value;

    if(u_slack >= l_slack)
    {
        x_perturbed = value + p_optimizer->pertrurbationSize() * span;
        if(x_perturbed > max) x_perturbed = max;
    }
    else
    {
        x_perturbed = value - p_optimizer->pertrurbationSize() * span;
        if(x_perturbed < min) x_perturbed = min;
    }

    return x_perturbed;

}


//-----------------------------------------------------------------------------------------------
// Calculates the gradients
//-----------------------------------------------------------------------------------------------
void BonminInterface::calculateGradients(Index n, const Number *x)
{
    // checking if the gradient vectors have the correct size
    int n_grad = m_vars_binary.size() + m_vars_real.size();
    if(m_grad_f.size() != n_grad) m_grad_f = QVector<double>(n_grad);

    int n_jac = (m_vars_binary.size() + m_vars_real.size())*m_cons.size();
    if(m_jac_g.size() != n_jac) m_jac_g = QVector<double>(n_jac);


    // checking if these are new variable values
    if(newVariableValues(n, x))
    {
        // deleting the old case
        if(p_case_last != 0) delete p_case_last;
        p_case_last = 0;

        Case *case_new = generateCase(n,x);


        // adding the case to a queue
        CaseQueue *case_queue = new CaseQueue();
        case_queue->push_back(case_new);

        // sending the new case to the runner
        p_optimizer->runCases(case_queue);

        // setting the case as the last case
        p_case_last  = case_new;

        // deleting the case queue
        delete case_queue;

    }

    // deleting the old gradients case, copying the last case
    if(p_case_gradients != 0) delete p_case_gradients;
    p_case_gradients = new Case(*p_case_last, true);


    // setting up the case queue with all the perturbations
    CaseQueue *case_queue = new CaseQueue();

    // adding the real variable perturbations
    for(int i = 0; i < m_vars_real.size(); ++i)
    {
        // calculating the perturbed value of the variable
        double x_perturbed = perturbedVariableValue(p_case_gradients->realVariableValue(i), m_vars_real.at(i)->max(), m_vars_real.at(i)->min());

        // setting up a new case
        Case *case_perturbed = new Case(*p_case_gradients);

        // changing the value of the variable to the perturbe value
        case_perturbed->setRealVariableValue(i, x_perturbed);

        // adding the case to the queue
        case_queue->push_back(case_perturbed);
    }

    // adding the binary variable perturbations
    for(int i = 0; i < m_vars_binary.size(); ++i)
    {
        // calculating the perturbed value of the variable
        double x_perturbed = perturbedVariableValue(p_case_gradients->binaryVariableValue(i), m_vars_binary.at(i)->max(), m_vars_binary.at(i)->min());

        // setting up a new case
        Case *case_perturbed = new Case(*p_case_gradients);

        // changing the value of the variable to the perturbe value
        case_perturbed->setBinaryVariableValue(i, x_perturbed);

        // adding the case to the queue
        case_queue->push_back(case_perturbed);
    }

    // sending the cases to the runner for evaluation
    p_optimizer->runCases(case_queue);



    int n_var = 0;

    // calculating the gradients of the real variables
    for(int i = 0; i < p_case_gradients->numberOfRealVariables(); ++i)
    {
        // calculating the perturbation size of the variable
        double dx = p_case_gradients->realVariableValue(i) - case_queue->at(n_var)->realVariableValue(i);

        // calculating the gradient of the objective
        double dfdx = -(p_case_gradients->objectiveValue() - case_queue->at(n_var)->objectiveValue()) / dx;

        //cout << "x_base = " << p_case_gradients->realVariableValue(i) << endl;
        //cout << "x_pert = " << case_queue->at(n_var)->realVariableValue(i) << endl;
        //cout << "f_base = " << p_case_gradients->objectiveValue() << endl;
        //cout << "f_pert = " << case_queue->at(n_var)->objectiveValue() << endl;

        //cout << "df/dx" << n_var << " = " << dfdx << endl;

        // setting the value to the objective gradient vector
        m_grad_f.replace(n_var, dfdx);


        // calculating the gradients of the constraints
        int entry = n_var*p_case_gradients->numberOfConstraints();
        for(int j = 0; j < p_case_gradients->numberOfConstraints(); ++j)
        {
            double dcdx = (p_case_gradients->constraintValue(j) - case_queue->at(n_var)->constraintValue(j)) / dx;
            m_jac_g.replace(entry, dcdx);
            ++entry;


        }


        ++n_var;
    }

    // calculating the gradients of the binary variables
    for(int i = 0; i < p_case_gradients->numberOfBinaryVariables(); ++i)
    {
        // calculating the perturbation size of the variable
        double dx = p_case_gradients->binaryVariableValue(i) - case_queue->at(n_var)->binaryVariableValue(i);

        // calculating the gradient of the objective
        double dfdx = -(p_case_gradients->objectiveValue() - case_queue->at(n_var)->objectiveValue()) / dx;

        // setting the value to the objective gradient vector
        m_grad_f.replace(n_var, dfdx);


        // calculating the gradients of the constraints
        int entry = n_var*p_case_gradients->numberOfConstraints();
        for(int j = 0; j < p_case_gradients->numberOfConstraints(); ++j)
        {
            double dcdx = (p_case_gradients->constraintValue(j) - case_queue->at(n_var)->constraintValue(j)) / dx;
            m_jac_g.replace(entry, dcdx);
            ++entry;


        }


        ++n_var;
    }

    // deleting the perturbed cases
    for(int i = 0; i < case_queue->size(); ++i) delete case_queue->at(i);
    delete case_queue;



}

//-----------------------------------------------------------------------------------------------
// Checks if the gradients are up to date
//-----------------------------------------------------------------------------------------------
bool BonminInterface::gradientsAreUpdated(Index n, const Number *x)
{

    // fist checking if the gradients case has been initialized
    if(p_case_gradients == 0) return false;

    if(p_case_gradients->numberOfRealVariables() == m_vars_real.size() && p_case_gradients->numberOfBinaryVariables() == m_vars_binary.size()) //checking if the gradient case is initialized
    {
        // checking if the variable values are the same as the gradients case
        int n_var = 0;
        bool new_x = false;

        // real variables
        for(int i = 0; i < p_case_gradients->numberOfRealVariables(); ++i)
        {
            if(x[n_var] != p_case_gradients->realVariableValue(i))
            {
                new_x = true;
                break;
            }
            ++n_var;
        }

        if(new_x) return new_x;

        // binary variables
        for(int i = 0; i < p_case_gradients->numberOfBinaryVariables(); ++i)
        {
            if(x[n_var] != p_case_gradients->binaryVariableValue(i))
            {
                new_x = true;
                break;
            }


            ++n_var;
        }

        return new_x;

    }

    else return false;
}

//-----------------------------------------------------------------------------------------------
// Generates a Case based on the values in x
//-----------------------------------------------------------------------------------------------
Case* BonminInterface::generateCase(Index n, const Number *x)
{
    // checking that the problem has the right dimensions
    assert(n == (m_vars_real.size() + m_vars_binary.size()));

    // creating a new case
    Case *case_new = new Case();

    // updating the variable values
    int n_var = 0;

    // real variables
    for(int i = 0; i < m_vars_real.size(); i++)
    {
        case_new->addRealVariableValue(x[n_var]);
        ++n_var;
    }

    // binary variables
    for(int i = 0; i < m_vars_binary.size(); i++)
    {
        case_new->addBinaryVariableValue(x[n_var]);
        ++n_var;
    }

    return case_new;


}

} // namespace ResOpt
