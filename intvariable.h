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


#ifndef INTVARIABLE_H
#define INTVARIABLE_H

#include "variable.h"

/**
 * @brief A class of integer type variables.
 *
 */
class IntVariable : public Variable
{
private:
    int m_min;      // lower bound /**< TODO */
    int m_max;      // upper bound /**< TODO */
    int m_value;    // current value /**< TODO */

public:
/**
 * @brief
 *
 */
    IntVariable();

    // virtual functions

    virtual bool isVariable();


    // set functions

    /**
     * @brief Sets the minimum bound for the variable
     *
     * @param min
     */
    void setMin(int min) {m_min = min;}

    /**
     * @brief Sets the maximum bound for the variable
     *
     * @param max
     */
    void setMax(int max) {m_max = max;}

    /**
     * @brief Sets the current value for the variable
     *
     * @param val
     */
    void setValue(int val) {m_value = val;}

    // get functions

    /**
     * @brief Returns the minimum bound for the variable
     *
     * @return int
     */
    int min() {return m_min;}

    /**
     * @brief Returns the maximum bound for the variable
     *
     * @return int
     */
    int max() {return m_max;}

    /**
     * @brief Returns the current value for the variable
     *
     * @return int
     */
    int value() {return m_value;}

};

#endif // INTVARIABLE_H
