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

namespace ResOpt
{

Stream::Stream()
    : m_time(0.0),
      m_oil_rate(0.0),
      m_water_rate(0.0),
      m_gas_rate(0.0),
      m_pressure(0.0),
      m_input_units(Stream::FIELD)
{
}

Stream::Stream(double t, double qo, double qg, double qw, double p)
    : m_time(t),
      m_oil_rate(qo),
      m_water_rate(qw),
      m_gas_rate(qg),
      m_pressure(p),
      m_input_units(Stream::FIELD)
{}

//-----------------------------------------------------------------------------------------------
// Sets the values of this stream to the average of the vector
//-----------------------------------------------------------------------------------------------
void Stream::avg(const QVector<Stream *> &input, double t_start)
{
    // setting the time to the time of the last stream
    m_time = input.at(input.size() -1)->time();

    double ts_start = t_start;

    // calculating cumulatives
    double cum_gas = 0;
    double cum_oil = 0;
    double cum_water = 0;
    double cum_pres = 0;
    for(int i = 0; i < input.size(); ++i)
    {
        cum_gas += input.at(i)->gasRate(inputUnits()) * (input.at(i)->time() - ts_start);
        cum_oil += input.at(i)->oilRate(inputUnits()) * (input.at(i)->time() - ts_start);
        cum_water += input.at(i)->waterRate(inputUnits()) * (input.at(i)->time() - ts_start);
        cum_pres += input.at(i)->pressure(inputUnits()) * (input.at(i)->time() - ts_start);

        ts_start = input.at(i)->time();
    }

    // setting the average rates to this stream
    double time_span = m_time - t_start;
    m_gas_rate = cum_gas / time_span;
    m_oil_rate = cum_oil / time_span;
    m_water_rate = cum_water / time_span;
    m_pressure = cum_pres / time_span;

}

//-----------------------------------------------------------------------------------------------
// Prints the stream info to cout
//-----------------------------------------------------------------------------------------------
void Stream::printToCout() const
{
    cout << "Stream data:" << endl;
    cout << "TIME       = " << time() << endl;
    cout << "GAS RATE   = " << gasRate(inputUnits()) << endl;
    cout << "OIL RATE   = " << oilRate(inputUnits()) << endl;
    cout << "WATER RATE = " << waterRate(inputUnits()) << endl;
    cout << "PRESSURE   = " << pressure(inputUnits()) << endl;
    if(inputUnits() == Stream::METRIC) cout << "UNITS      = METRIC" << endl << endl;
    else cout << "UNITS      = FIELD" << endl << endl;

}

//-----------------------------------------------------------------------------------------------
// Returns the gas rate
//-----------------------------------------------------------------------------------------------
double Stream::gasRate(Stream::units u) const
{
    if(u != m_input_units)
    {
        if(u == Stream::METRIC) return m_gas_rate * 28.3168466;
        else return m_gas_rate /  28.3168466;
    }
    else return m_gas_rate;
}

double Stream::gasRate(bool input_units) const
{
    if(input_units) return m_gas_rate;
    else
    {
        if(inputUnits() == Stream::METRIC) return gasRate(Stream::FIELD);
        else return gasRate(Stream::METRIC);
    }
}

//-----------------------------------------------------------------------------------------------
// Returns the oil rate
//-----------------------------------------------------------------------------------------------
double Stream::oilRate(Stream::units u) const
{
    if(u != m_input_units)
    {
        if(u == Stream::METRIC) return m_oil_rate * 0.158987295;
        else return m_oil_rate /  0.158987295;
    }
    else return m_oil_rate;
}

double Stream::oilRate(bool input_units) const
{
    if(input_units) return m_oil_rate;
    else
    {
        if(inputUnits() == Stream::METRIC) return oilRate(Stream::FIELD);
        else return oilRate(Stream::METRIC);
    }
}


//-----------------------------------------------------------------------------------------------
// Returns the water rate
//-----------------------------------------------------------------------------------------------
double Stream::waterRate(Stream::units u) const
{
    if(u != m_input_units)
    {
        if(u == Stream::METRIC) return m_water_rate * 0.158987295;
        else return m_water_rate /  0.158987295;
    }
    else return m_water_rate;
}

double Stream::waterRate(bool input_units) const
{
    if(input_units) return m_water_rate;
    else
    {
        if(inputUnits() == Stream::METRIC) return waterRate(Stream::FIELD);
        else return waterRate(Stream::METRIC);
    }
}


//-----------------------------------------------------------------------------------------------
// Returns the pressure
//-----------------------------------------------------------------------------------------------
double Stream::pressure(Stream::units u) const
{
    if(u != m_input_units)
    {
        if(u == Stream::METRIC) return m_pressure / 14.5037738;
        else return m_pressure * 14.5037738;
    }
    else return m_pressure;
}

double Stream::pressure(bool input_units) const
{
    if(input_units) return m_pressure;
    else
    {
        if(inputUnits() == Stream::METRIC) return pressure(Stream::FIELD);
        else return pressure(Stream::METRIC);
    }
}



//-----------------------------------------------------------------------------------------------
// Assignment operator
//-----------------------------------------------------------------------------------------------
Stream& Stream::operator =(const Stream &rhs)
{
    if(this != &rhs)
    {
        setTime(rhs.time());
        setGasRate(rhs.gasRate(inputUnits()));
        setOilRate(rhs.oilRate(inputUnits()));
        setWaterRate(rhs.waterRate(inputUnits()));
        setPressure(rhs.pressure(inputUnits()));
        setInputUnits(rhs.inputUnits());
    }

    return *this;
}

//-----------------------------------------------------------------------------------------------
// Compound assignment operator
//-----------------------------------------------------------------------------------------------
Stream& Stream::operator +=(const Stream& rhs)
{

    setTime(rhs.time());
    setPressure(0.0);

    // setting the rates with the same units as the lhs
    setGasRate(gasRate(rhs.inputUnits()) + rhs.gasRate(rhs.inputUnits()));
    setOilRate(oilRate(rhs.inputUnits()) + rhs.oilRate(rhs.inputUnits()));
    setWaterRate(waterRate(rhs.inputUnits()) + rhs.waterRate(rhs.inputUnits()));

    // setting the units
    setInputUnits(rhs.inputUnits());


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
    result.setGasRate(result.gasRate(result.inputUnits())*rhs);
    result.setOilRate(result.oilRate(result.inputUnits())*rhs);
    result.setWaterRate(result.waterRate(result.inputUnits())*rhs);

    return result;
}


} // namespace ResOpt
