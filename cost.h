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


#ifndef COST_H
#define COST_H

namespace ResOpt
{


/**
 * @brief Class for costs associated with installing equipment etc.
 * @details A Cost has a price (value in USD), and an installation time. A vector of costs can be supplied to the Objective. The costs are
 *          only used for the NpvObjective when the net present value is calculated. Currently only separators have associated costs.
 *
 */
class Cost
{
private:
    double m_time;
    double m_value;

public:
    Cost();

    // set functions
    void setTime(double t) {m_time = t;}
    void setValue(double v) {m_value = v;}

    // get functions
    double time() const {return m_time;}
    double value() const {return m_value;}

    // overloaded operators
    bool operator<(const Cost &rhs) const;
    bool operator>(const Cost &rhs) const;
};


} // namespace ResOpt

#endif // COST_H
