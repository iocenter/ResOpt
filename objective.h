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

#ifndef OBJECTIVE_H
#define OBJECTIVE_H

#include <QVector>



namespace ResOpt
{

class Stream;
class Cost;


/**
 * @brief Abstract class for objective functions.
 *
 */
class Objective
{
private:
    double m_value; /**< TODO */

public:
/**
 * @brief Container class for objectives
 *
 */
    Objective();

    // virtual functions

    virtual Objective* clone() = 0;

    /**
     * @brief Calculates the objective value based on a vector of streams.
     * @details The function calculates and stores the objective value. To retreive the calculated value, use value(). This
     *          function must be implemented in each sub class of Objective to correctly calculate the objective value.
     *
     * @param s
     */
    virtual void calculateValue(QVector<Stream*> s, QVector<Cost*> c) = 0;

    virtual QString description() const = 0;

    // set functions

    /**
     * @brief Sets the objective value
     *
     * @param v
     */
    void setValue(double v) {m_value = v;}

    // get functions

    /**
     * @brief Returns the objective value
     *
     * @return double
     */
    double value() const {return m_value;}



};

} // namespace ResOpt


#endif // OBJECTIVE_H
