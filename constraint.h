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


#ifndef CONSTRAINT_H
#define CONSTRAINT_H



#include <QString>


namespace ResOpt
{

/**
 * @brief Container class for constraints
 *
 */
class Constraint
{
private:
    double m_value;
    double m_max; /**< TODO */
    double m_min; /**< TODO */
    QString m_name;

public:
/**
 * @brief
 *
 */
    Constraint();

    Constraint(double value, double max, double min);

    // set functions


    /**
     * @brief Sets the current value of the constraint
     *
     * @param v
     */
    void setValue(double v) {m_value = v;}

    /**
     * @brief Sets the maximum bound for the constraint
     *
     * @param d
     */
    void setMax(double d) {m_max = d;}

    /**
     * @brief Sets the minimum bound for the constraint
     *
     * @param d
     */
    void setMin(double d) {m_min = d;}


    void setName(const QString &n) {m_name = n;}


    // get functions


    /**
     * @brief Returns the current value of the constraint
     *
     * @return double
     */
    double value() {return m_value;}

    /**
     * @brief Returns the maximum bound for the constraint
     *
     * @return double
     */
    double max() {return m_max;}

    /**
     * @brief Returns the minimum bound for the constraint
     *
     * @return double
     */
    double min() {return m_min;}

    const QString& name() {return m_name;}
};

} // namespace ResOpt

#endif // CONSTRAINT_H
