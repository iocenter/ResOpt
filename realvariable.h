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

#ifndef REALVARIABLE_H
#define REALVARIABLE_H

#include "variable.h"

namespace ResOpt
{


/**
 * @brief A class for contineous (real) variables.
 * @details Countineous variables are normally well control variables, like productions rate or bottomhole pressure
 *          set points. A RealVariable has a current value, and maximum and minimum bounds. The current values should
 *          never go outside the bounds.
 *
 */
class RealVariable : public Variable
{
private:
    double m_max;           // upper bound /**< TODO */
    double m_min;           // lower bound /**< TODO */
    double m_value;         // current value /**< TODO */

public:
/**
 * @brief
 *
 */
    RealVariable(Component *parent);

    // virtual functions

    virtual bool isVariable();


    // set functions

    /**
     * @brief Sets the minimum bound
     *
     * @param min
     */
    void setMin(double min) {m_min = min;}

    /**
     * @brief Sets the maximum bound
     *
     * @param max
     */
    void setMax(double max) {m_max = max;}

    /**
     * @brief Sets the current value
     *
     * @param val
     */
    void setValue(double val) {m_value = val;}

    // get functions

    /**
     * @brief Returns the minimum bound
     *
     * @return double
     */
    double min() {return m_min;}

    /**
     * @brief Returns the maximum bound
     *
     * @return double
     */
    double max() {return m_max;}

    /**
     * @brief Returns the current value
     *
     * @return double
     */
    double value() {return m_value;}
};

} // namespace ResOpt

#endif // REALVARIABLE_H
