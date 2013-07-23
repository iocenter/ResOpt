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


#include "cost.h"

#include <math.h>


namespace ResOpt
{


Cost::Cost() :
    m_time(0),
    m_const(0),
    m_mult_frac(0),
    m_mult_cap(0),
    m_fraction(0),
    m_capacity(0),
    m_fraction_exponent(1.0),
    m_capacity_exponent(1.0),
    m_linear(true)
{
}


//-----------------------------------------------------------------------------------------------
// returns the current value of the cost
//-----------------------------------------------------------------------------------------------
double Cost::value() const
{
    if(m_linear)
    {
        return m_const + m_mult_frac * pow(m_fraction, m_fraction_exponent) + m_mult_cap * pow(m_capacity, m_capacity_exponent);
    }

    else
    {
        return m_const + m_mult_frac * (pow(m_fraction, m_fraction_exponent) * pow(m_capacity, m_capacity_exponent));
    }

}


//-----------------------------------------------------------------------------------------------
// overloaded < operator (checks the time only)
//-----------------------------------------------------------------------------------------------
bool Cost::operator <(const Cost &rhs) const
{
    return m_time < rhs.m_time;
}

//-----------------------------------------------------------------------------------------------
// overloaded > operator (checks the time only)
//-----------------------------------------------------------------------------------------------
bool Cost::operator >(const Cost &rhs) const
{
    return m_time > rhs.m_time;
}


} // namespace ResOpt
