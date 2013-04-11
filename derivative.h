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


#ifndef DERIVATIVE_H
#define DERIVATIVE_H

#include <QPair>
#include <QVector>

namespace ResOpt
{

class Derivative
{
private:
    int m_constraint_id;
    QVector<QPair<int, double> > m_partial_derivatives;

public:
    Derivative();
    Derivative(int con_id);


    // add functions
    void addPartial(int var_id, double value) {m_partial_derivatives.push_back(QPair<int, double>(var_id, value));}

    // get functions
    int constraintId() {return m_constraint_id;}
    int numberOfPartials() {return m_partial_derivatives.size();}


    /**
     * @brief Retruns the value of the partial derivative for variable with id = var_id
     *
     * @param var_id
     * @return double
     */
    double value(int var_id);


};


} // namespace

#endif // DERIVATIVE_H
