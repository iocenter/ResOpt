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


#ifndef LSHIPOPTINTERFACE_H
#define LSHIPOPTINTERFACE_H

#include "IpTNLP.hpp"
#include <QVector>
#include <tr1/memory>

using namespace Ipopt;
using std::tr1::shared_ptr;

namespace ResOpt
{

class LshOptimizer;
class RealVariable;
class Constraint;
class Case;
class CaseQueue;

class LshIpoptInterface : public TNLP
{

private:

    LshOptimizer *p_optimizer;

    QVector<shared_ptr<RealVariable> > m_vars;
    QVector<shared_ptr<Constraint> > m_cons;

    QVector<double> m_grad_f;   // calculated values for df/dx
    QVector<double> m_jac_g;    // calculated values for dc/dx
    Case *p_case_last;          // the last case that was run
    Case *p_case_gradients;     // case containing variable values where the gradient and jacobian was calculated

    /**
     * @brief Generates a Case based on the values in x.
     *
     * @param n
     * @param x
     * @return Case
     */
    Case* generateCase(Index n, const Number *x);

    /**
     * @brief Checks if the variable values in x are the same as in the Modelc
     *
     * @param x
     * @return bool
     */
    bool newVariableValues(Index n, const Number *x);

    double perturbedVariableValue(double value, double max, double min);

    void calculateGradients(Index n, const Number *x);

    bool gradientsAreUpdated(Index n, const Number *x);

    /**@name Block default compiler methods */
    LshIpoptInterface(const LshIpoptInterface&);
    LshIpoptInterface& operator=(const LshIpoptInterface&);

public:

    /** default constructor */
    LshIpoptInterface(LshOptimizer *o);

    /** default destructor */
    virtual ~LshIpoptInterface();

    /**@name Overloaded from TNLP */

    /** Method to return some info about the nlp */
    virtual bool get_nlp_info(Index& n, Index& m, Index& nnz_jac_g,
                              Index& nnz_h_lag, IndexStyleEnum& index_style);

    /** Method to return the bounds for my problem */
    virtual bool get_bounds_info(Index n, Number* x_l, Number* x_u,
                                 Index m, Number* g_l, Number* g_u);

    /** Method to return the starting point for the algorithm */
    virtual bool get_starting_point(Index n, bool init_x, Number* x,
                                    bool init_z, Number* z_L, Number* z_U,
                                    Index m, bool init_lambda,
                                    Number* lambda);

    /** Method to return the objective value */
    virtual bool eval_f(Index n, const Number* x, bool new_x, Number& obj_value);

    /** Method to return the gradient of the objective */
    virtual bool eval_grad_f(Index n, const Number* x, bool new_x, Number* grad_f);

    /** Method to return the constraint residuals */
    virtual bool eval_g(Index n, const Number* x, bool new_x, Index m, Number* g);

    /** Method to return:
     *   1) The structure of the jacobian (if "values" is NULL)
     *   2) The values of the jacobian (if "values" is not NULL)
     */
    virtual bool eval_jac_g(Index n, const Number* x, bool new_x,
                            Index m, Index nele_jac, Index* iRow, Index *jCol,
                            Number* values);

    /** Method to return:
     *   1) The structure of the hessian of the lagrangian (if "values" is NULL)
     *   2) The values of the hessian of the lagrangian (if "values" is not NULL)
     */
    virtual bool eval_h(Index n, const Number* x, bool new_x,
                        Number obj_factor, Index m, const Number* lambda,
                        bool new_lambda, Index nele_hess, Index* iRow,
                        Index* jCol, Number* values);


    /** @name Solution Methods */

    /** This method is called when the algorithm is complete so the TNLP can store/write the solution */
    virtual void finalize_solution(SolverReturn status,
                                   Index n, const Number* x, const Number* z_L, const Number* z_U,
                                   Index m, const Number* g, const Number* lambda,
                                   Number obj_value,
                                   const IpoptData* ip_data,
                                   IpoptCalculatedQuantities* ip_cq);

};

} // namespace ResOpt

#endif // LSHIPOPTINTERFACE_H
