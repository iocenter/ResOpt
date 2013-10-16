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


#include "minlpipoptinterface.h"

#include <cassert>
#include <QVector>
#include <QTextStream>
#include <iostream>

#include "optimizer.h"
#include "minlpevaluator.h"
#include "runner.h"
#include "model.h"
#include "adjointscoupledmodel.h"
#include "realvariable.h"
#include "binaryvariable.h"
#include "intvariable.h"
#include "constraint.h"
#include "objective.h"
#include "case.h"
#include "derivative.h"
#include "casequeue.h"
#include "reservoirsimulator.h"

using std::cout;
using std::endl;

namespace ResOpt
{

/* Constructor. */
MINLPIpoptInterface::MINLPIpoptInterface(Optimizer *o, MINLPEvaluator *e, Case *discrete_vars)
    : p_optimizer(o),
      p_evaluator(e),
      p_discrete_vars(discrete_vars),
      p_case_last(0),
      p_case_gradients(0),
      p_best_case(0),
      m_adjoints(false)
{
    m_vars = p_optimizer->runner()->model()->realVariables();
    m_cons = p_optimizer->runner()->model()->constraints();


    // setting up the gradients file
    p_grad_file= new QFile(p_optimizer->runner()->reservoirSimulator()->folder() + "/ipopt_gradients.dat");

    if(!p_grad_file->open(QIODevice::WriteOnly | QIODevice::Text))
    {
        qWarning("Could not connect to IPOPT gradients file: %s", p_grad_file->fileName().toLatin1().constData());

        delete p_grad_file;
        p_grad_file = 0;

        exit(1);
    }
    else
    {
        // deleting content from previous launches
        p_grad_file->resize(0);

    }

    // checking if adjoints are used
    AdjointsCoupledModel *am = dynamic_cast<AdjointsCoupledModel*>(p_optimizer->runner()->model());
    if(am != 0) m_adjoints = true;


}

MINLPIpoptInterface::~MINLPIpoptInterface()
{
    if(p_case_last != 0) delete p_case_last;
    if(p_case_gradients != 0) delete p_case_gradients;
    if(p_best_case != 0) delete p_best_case;
}

bool MINLPIpoptInterface::get_nlp_info(Index& n, Index& m, Index& nnz_jac_g,
                         Index& nnz_h_lag, IndexStyleEnum& index_style)
{
    //cout << "Giving Ipopt the dimensions of the problem..." << endl;

    n = m_vars.size();  // number of variables
    m = m_cons.size();  // number of constraints

    nnz_jac_g = m*n;        // dense Jacobian
    nnz_h_lag = n*(n+1)/2;  // dense, symmetric Hessian

    // index style for row/col entries
    // index_style = FORTRAN_STYLE;
    index_style = TNLP::C_STYLE;    // c style numbering, starting at 0

    //cout << "Number of variables   = " << n << endl;
    //cout << "Number of constraints = " << m << endl;

    return true;
}

bool MINLPIpoptInterface::get_bounds_info(Index n, Number* x_l, Number* x_u,
                            Index m, Number* g_l, Number* g_u)
{
    //cout << "Giving Ipopt bounds of variables and constraints..." << endl;

    assert(n == m_vars.size());
    assert(m == m_cons.size());

    // setting the real variable bounds
    for(int i = 0; i < m_vars.size(); ++i)
    {
        x_l[i] = m_vars.at(i)->min();   // lower bound
        x_u[i] = m_vars.at(i)->max();   // upper bound

       // cout << m_vars.at(i)->min() << " < x[" << i << "] < " << m_vars.at(i)->max() << endl;


    }

    // setting the constraint bounds
    for(int i = 0; i < m_cons.size(); ++i)
    {
        g_l[i] = m_cons.at(i)->min();   // lower bound
        g_u[i] = m_cons.at(i)->max();   // upper bound
    }

    return true;
}

bool MINLPIpoptInterface::get_starting_point(Index n, bool init_x, Number* x,
                               bool init_z, Number* z_L, Number* z_U,
                               Index m, bool init_lambda,
                               Number* lambda)
{
    //cout << "Giving Ipopt the starting point..." << endl;

    assert(n == m_vars.size());

    // only expect to initialize x
    assert(init_x);
    assert(!init_z);
    assert(!init_lambda);


    // setting the variable starting points
    for(int i = 0; i < m_vars.size(); ++i)
    {
        x[i] = m_vars.at(i)->value();  // current value = starting point

        //cout <<  " < x[" << i << "] = " << m_vars.at(i)->value() << endl;


    }

    return true;

}

bool MINLPIpoptInterface::eval_f(Index n, const Number* x, bool new_x, Number& obj_value)
{
    //cout << "Evaluating objective function for Ipopt..." << endl;

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

    // getting the value of the objective (negative since Ipopt is doing minimization)
    obj_value = -p_case_last->objectiveValue();

    //cout << "f = " << obj_value << endl;

    return true;

}

bool MINLPIpoptInterface::eval_grad_f(Index n, const Number* x, bool new_x, Number* grad_f)
{
    //cout << "Evaluating the objective function gradients for Ipopt..." << endl;
    bool ok = true;

    // first checking if gradients are allready calculated
    if(!gradientsAreUpdated(n,x))
    {
       // cout << "need to calculate new gradients..." << endl;
        if(m_adjoints) ok = copyCaseGradients(n,x);
        else calculateGradients(n,x);
       // cout << "done calculating new gradients..." << endl;
    }
    //else cout << "gradients are already calculated for this point..." << endl;

    // copying the calculated gradients to Ipopt
    for(int i = 0; i < n; i++)
    {
        grad_f[i] = m_grad_f.at(i);

        // cout << "grad_f[" << i << "] = " << grad_f[i] << endl;
    }

    return ok;

}

bool MINLPIpoptInterface::eval_g(Index n, const Number* x, bool new_x, Index m, Number* g)
{
    //cout << "Evaluating the constraints for Ipopt..." << endl;

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

    // checking that the number of constraints in the case corresponds to m
    assert(m == p_case_last->numberOfConstraints());

    // getting the value of the constraints
    for(int i = 0; i < p_case_last->numberOfConstraints(); ++i)
    {
        g[i] = p_case_last->constraintValue(i);

        //cout << "g[" << i << "] = " << g[i] << endl;
    }

    return true;
}

bool MINLPIpoptInterface::eval_jac_g(Index n, const Number* x, bool new_x,
                       Index m, Index nele_jac, Index* iRow, Index *jCol,
                       Number* values)
{
    // checking if the structure of the jacobian has been set
    if (values == NULL)
    {
        //cout << "Giving Ipopt the structure of the jacobian..." << endl;

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
        //cout << "Evaluating the jacobian for Ipopt..." << endl;

        // checking if gradients are calculated
        if(!gradientsAreUpdated(n,x))
        {
            if(m_adjoints) copyCaseGradients(n,x);
            else calculateGradients(n,x);
        }

        // copying gradients to Ipopt
        for(int i = 0; i < nele_jac; i++)
        {
            values[i] = m_jac_g.at(i);
        }
    }

    return true;

}

bool MINLPIpoptInterface::eval_h(Index n, const Number* x, bool new_x,
                   Number obj_factor, Index m, const Number* lambda,
                   bool new_lambda, Index nele_hess, Index* iRow,
                   Index* jCol, Number* values)
{
    //cout << "Evaluating the constraint hessian for Ipopt..." << endl;
    return true;
}

void MINLPIpoptInterface::finalize_solution(SolverReturn status,
                              Index n, const Number* x, const Number* z_L, const Number* z_U,
                              Index m, const Number* g, const Number* lambda,
                              Number obj_value,
                  const IpoptData* ip_data,
                  IpoptCalculatedQuantities* ip_cq)
{
    // here is where we would store the solution to variables, or write to a file, etc
    // so we could use the solution. Since the solution is displayed to the console,
    // we currently do nothing here.
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

    cout << "Storing the best case for use by the descrete solver..." << endl;
    Case *c = generateCase(n,x);
    //c->setObjectiveValue(-obj_value);

    p_best_case = c;
}

bool MINLPIpoptInterface::intermediate_callback(AlgorithmMode mode, Index iter,
                                                Number obj_value, Number inf_pr,
                                                Number inf_du, Number mu, Number d_norm,
                                                Number regularization_size, Number alpha_du,
                                                Number alpha_pr, Index ls_trials, const IpoptData *ip_data,
                                                IpoptCalculatedQuantities *ip_cq)
{

     //cout << "%%%%%%%%%%%%%%%%%%%%%% intermediate callback start %%%%%%%%%%%%%%%%%%%%%%" << endl;


    // adding the current objective value to the local vector
    m_objs.push_back(obj_value);
    m_infeas.push_back(inf_pr);

    //cout << "iter      = " << iter << endl;
    //cout << "obj_value = " << obj_value << endl;
    //cout << "inf_pr    = " << inf_pr << endl;
    //cout << "inf_du    = " << inf_du << endl;
    //cout << "mu        = " << mu << endl;

    // checking if the optimization should continue
    bool cont = p_evaluator->shouldContinue(m_objs.size()-1, obj_value, inf_pr);



    if(!cont)
    {
        cout << "---------------------------------------------" << endl;
        cout << "---------------------------------------------" << endl;
        cout << "Terminating IPOPT run due to slow progress..." << endl;
        cout << "---------------------------------------------" << endl;
        cout << "---------------------------------------------" << endl << endl;
    }

    //cout << "%%%%%%%%%%%%%%%%%%%%%%  intermediate callback end  %%%%%%%%%%%%%%%%%%%%%%" << endl;

    return cont;

}



//-----------------------------------------------------------------------------------------------
// Checks if the x values are the same as the current model values for the variables
//-----------------------------------------------------------------------------------------------
bool MINLPIpoptInterface::newVariableValues(Index n, const Number *x)
{
    bool x_new = false;

    // checking if the last case has been set up
    if(p_case_last == 0) return true;

    // checking that the number of variables in the case correspons to n
    int n_var_case = p_case_last->numberOfRealVariables();
    if(n != n_var_case) return false;

    // checking if the real variables are the same


    for(int i = 0; i < p_case_last->numberOfRealVariables(); ++i)
    {
        if(x[i] != p_case_last->realVariableValue(i))
        {
            x_new = true;
            break;
        }

    }

    return x_new;

}

//-----------------------------------------------------------------------------------------------
// Calculates the perturbated value a variable
//-----------------------------------------------------------------------------------------------
double MINLPIpoptInterface::perturbedVariableValue(double value, double max, double min)
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
void MINLPIpoptInterface::calculateGradients(Index n, const Number *x)
{
    //cout << "Starting perturbations to calculate gradients for IPOPT" << endl;
    // checking if the gradient vectors have the correct size
    int n_grad = m_vars.size();
    if(m_grad_f.size() != n_grad) m_grad_f = QVector<double>(n_grad);

    int n_jac = m_vars.size()*m_cons.size();
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
    for(int i = 0; i < p_case_gradients->numberOfRealVariables(); ++i)
    {
        // calculating the perturbed value of the variable
        double x_perturbed = perturbedVariableValue(p_case_gradients->realVariableValue(i), m_vars.at(i)->max(), m_vars.at(i)->min());

        // setting up a new case
        Case *case_perturbed = new Case(*p_case_gradients);

        // changing the value of the variable to the perturbe value
        case_perturbed->setRealVariableValue(i, x_perturbed);

        // running the case
        p_optimizer->runCase(case_perturbed);

        // adding the case to the queue
        case_queue->push_back(case_perturbed);
    }

    // sending the cases to the runner for evaluation
    //p_optimizer->runCases(case_queue);


    // setting up the text stream for gradients info
    QTextStream out(p_grad_file);

    // printing header

    out << "\n ------ GRADIENTS FOR IPOPT -----------\n";
    out << "VAR #\t OBJ \t";

    for(int i = 0; i < m_cons.size(); ++i)
    {
        out << "CON" << i+1 << "\t";
    }

    out << "\n";

    // calculating the gradients of the real variables
    for(int i = 0; i < p_case_gradients->numberOfRealVariables(); ++i)
    {
        // calculating the perturbation size of the variable
        double dx = p_case_gradients->realVariableValue(i) - case_queue->at(i)->realVariableValue(i);

        // calculating the gradient of the objective
        double dfdx = -(p_case_gradients->objectiveValue() - case_queue->at(i)->objectiveValue()) / dx;

        // setting the value to the objective gradient vector
        m_grad_f.replace(i, dfdx);

        //printing var # and obj
        out << i+1 << "\t" << dfdx << "\t";


        // calculating the gradients of the constraints
        int entry = i*p_case_gradients->numberOfConstraints();
        for(int j = 0; j < p_case_gradients->numberOfConstraints(); ++j)
        {
            double dcdx = (p_case_gradients->constraintValue(j) - case_queue->at(i)->constraintValue(j)) / dx;
            m_jac_g.replace(entry, dcdx);
            ++entry;

            // printing con deriv
            out << dcdx << "\t";
        }

        out << "\n";

        /*
        // printing base case
        out << p_case_gradients->realVariableValue(i) << " (b)\t" << p_case_gradients->objectiveValue() << "\t";
        for(int k = 0; k < p_case_gradients->numberOfConstraints(); ++k)
        {
            out << p_case_gradients->constraintValue(k) << "\t";
        }
        out << "\n";

        // printing pertrubarion
        out << case_queue->at(i)->realVariableValue(i) << " (p)\t" << case_queue->at(i)->objectiveValue() << "\t";
        for(int k = 0; k < case_queue->at(i)->numberOfConstraints(); ++k)
        {
            out << case_queue->at(i)->constraintValue(k) << "\t";
        }
        out << "\n";
        */



    }

    // deleting the perturbed cases
    for(int i = 0; i < case_queue->size(); ++i) delete case_queue->at(i);
    delete case_queue;

    p_grad_file->flush();

}

//-----------------------------------------------------------------------------------------------
// Copy gradients from case if adjoints are used
//-----------------------------------------------------------------------------------------------
bool MINLPIpoptInterface::copyCaseGradients(Index n, const Number *x)
{
    //cout << "copyCaseGradients() start" << endl;

    // checking if the gradient vectors have the correct size
    int n_grad = m_vars.size();
    if(m_grad_f.size() != n_grad) m_grad_f = QVector<double>(n_grad);

    int n_jac = m_vars.size()*m_cons.size();
    if(m_jac_g.size() != n_jac) m_jac_g = QVector<double>(n_jac);


    // checking if the case must be run
    if(newVariableValues(n,x))
    {
       // cout << "need to run case..." << endl;

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

      //  cout << "done running case" << endl;


    }

    if(p_case_last->numberOfConstraintDerivatives() != m_cons.size())
    {
        cout << "ERROR  --- number of constraint derivatives is wrong..." << endl;
        return false;
    }


    // setting up the text stream for gradients info
    QTextStream out(p_grad_file);

    // printing header

    out << "\n ------ GRADIENTS FOR IPOPT -----------\n";
    out << "VAR #\t OBJ \t";

    for(int i = 0; i < m_cons.size(); ++i)
    {
        out << "CON" << i+1 << "\t";
    }

    out << "\n";



    //cout << "copying derivatives" << endl;
    // starting to copy gradients
    for(int i = 0; i < p_case_last->numberOfRealVariables(); ++i)
    {
        //cout << "variable " << i << endl;
        // copying objective derivative
        double dfdx = -p_case_last->objectiveDerivative()->value(i);
        m_grad_f.replace(i, dfdx);

        //printing var # and obj
        out << i+1 << "\t" << dfdx << "\t";

        // copying the gradients of the constraints
        int entry = i*p_case_last->numberOfConstraints();

        for(int j = 0; j < p_case_last->numberOfConstraints(); ++j)
        {
            //cout << "constraint " << j << endl;
            double dcdx = p_case_last->constraintDerivative(j)->value(i);
            m_jac_g.replace(entry, dcdx);
            ++entry;

            // printing con deriv
            out << dcdx << "\t";
        }

        out << "\n";
    }

//    cout << "deleting last gradient case" << endl;

    if(p_case_gradients != 0) delete p_case_gradients;
    p_case_gradients = new Case(*p_case_last, true);

  //  cout << "copyCaseGradients() end" << endl;
}

//-----------------------------------------------------------------------------------------------
// Checks if the gradients are up to date
//-----------------------------------------------------------------------------------------------
bool MINLPIpoptInterface::gradientsAreUpdated(Index n, const Number *x)
{

    // checking if the gradients case has been initialized
    if(p_case_gradients == 0) return false;

    bool updated = false;

    if(p_case_gradients->numberOfRealVariables() == m_vars.size())
    {
        // checking if the variable values are the same as the gradients case

        bool new_x = false;

        // real variables
        for(int i = 0; i < p_case_gradients->numberOfRealVariables(); ++i)
        {
            if(x[i] != p_case_gradients->realVariableValue(i))
            {
                new_x = true;
                break;
            }

        }

        if(!new_x) updated = true;

    }

    return updated;

}

//-----------------------------------------------------------------------------------------------
// Generates a Case based on the values in x
//-----------------------------------------------------------------------------------------------
Case* MINLPIpoptInterface::generateCase(Index n, const Number *x)
{
    // checking that the problem has the right dimensions
    assert(n == m_vars.size());

    // creating a new case
    Case *case_new = new Case();

    // updating the variable values
    int n_var = 0;


    // real variables
    for(int i = 0; i < m_vars.size(); ++i)
    {
        case_new->addRealVariableValue(x[n_var]);
        ++n_var;
    }

    // copying int and binary variable values received from NOMAD
    for(int i = 0; i < p_discrete_vars->numberOfBinaryVariables(); ++i)
    {
        case_new->addBinaryVariableValue(p_discrete_vars->binaryVariableValue(i));
    }

    for(int i = 0; i < p_discrete_vars->numberOfIntegerVariables(); ++i)
    {
        case_new->addIntegerVariableValue(p_discrete_vars->integerVariableValue(i));
    }


    return case_new;

}

} // namespace ResOpt
