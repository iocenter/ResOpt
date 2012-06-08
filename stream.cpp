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


#include "stream.h"

#include <iostream>

using std::cout;
using std::endl;

Stream::Stream()
    : m_time(0.0),
      m_oil_rate(0.0),
      m_water_rate(0.0),
      m_gas_rate(0.0),
      m_pressure(0.0)
{
}


//-----------------------------------------------------------------------------------------------
// Prints the stream info to cout
//-----------------------------------------------------------------------------------------------
void Stream::printToCout()
{
    cout << "Stream data:" << endl;
    cout << "TIME       = " << time() << endl;
    cout << "GAS RATE   = " << gasRate() << endl;
    cout << "OIL RATE   = " << oilRate() << endl;
    cout << "WATER RATE = " << waterRate() << endl;
    cout << "PRESSURE   = " << pressure() << endl << endl;

}

//-----------------------------------------------------------------------------------------------
// Returns the gas rate
//-----------------------------------------------------------------------------------------------
double Stream::gasRate(Stream::units u) const
{
    if(u == Stream::FIELD) return m_gas_rate;
    else if(u == Stream::METRIC) return m_gas_rate * 28.3168466;
    else return -1.0;
}

//-----------------------------------------------------------------------------------------------
// Returns the oil rate
//-----------------------------------------------------------------------------------------------
double Stream::oilRate(Stream::units u) const
{
    if(u == Stream::FIELD) return m_oil_rate;
    else if(u == Stream::METRIC) return m_oil_rate * 0.158987295;
    else return -1.0;
}

//-----------------------------------------------------------------------------------------------
// Returns the water rate
//-----------------------------------------------------------------------------------------------
double Stream::waterRate(Stream::units u) const
{
    if(u == Stream::FIELD) return m_water_rate;
    else if(u == Stream::METRIC) return m_water_rate * 0.158987295;
    else return -1.0;
}


//-----------------------------------------------------------------------------------------------
// Assignment operator
//-----------------------------------------------------------------------------------------------
Stream& Stream::operator =(const Stream &rhs)
{
    if(this != &rhs)
    {
        setTime(rhs.time());
        setGasRate(rhs.gasRate());
        setOilRate(rhs.oilRate());
        setWaterRate(rhs.waterRate());
        setPressure(rhs.pressure());
    }

    return *this;
}

//-----------------------------------------------------------------------------------------------
// Compound assignment operator
//-----------------------------------------------------------------------------------------------
Stream& Stream::operator +=(const Stream& rhs)
{
    setTime(rhs.time());
    setGasRate(gasRate() + rhs.gasRate());
    setOilRate(oilRate() + rhs.oilRate());
    setWaterRate(waterRate() + rhs.waterRate());
    setPressure(0.0);

    return *this;
}

//-----------------------------------------------------------------------------------------------
// Plus operator
//-----------------------------------------------------------------------------------------------
const Stream Stream::operator +(const Stream &rhs) const
{
    Stream result = *this;
    result += rhs;

    return result;
}

//-----------------------------------------------------------------------------------------------
// Multiplication operator
//-----------------------------------------------------------------------------------------------
const Stream Stream::operator *(const double &rhs) const
{
    Stream result = *this;
    result.setGasRate(result.gasRate()*rhs);
    result.setOilRate(result.oilRate()*rhs);
    result.setWaterRate(result.waterRate()*rhs);

    return result;
}
