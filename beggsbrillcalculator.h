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


#ifndef BEGGSBRILLCALCULATOR_H
#define BEGGSBRILLCALCULATOR_H

#include "pressuredropcalculator.h"


namespace ResOpt
{

/**
 * @brief Pressure drop calculations after Beggs and Brill, 1973
 *
 */
class BeggsBrillCalculator : public PressureDropCalculator
{
public:
    enum flow_regime {SEGREGATED, TRANSITION, INTERMITTENT, DISTRIBUTED, UNDEFINED};

private:
    double m_sg_gas;        // gas specific gravity
    double m_den_oil;       // oil density
    double m_den_wat;       // water density

    double m_vis_oil;       // oil viscosity
    double m_vis_wat;       // water viscosity

    double m_diameter;      // inner pipe diameter
    double m_length;        // total pipe length
    double m_angle;         // inclination of pipe
    double m_temperature;   // average temperature in pipe

    // private calculate functions
    double superficialGasVelocity(Stream *s, double p, double z);
    double superficialLiquidVelocity(Stream *s);
    double liquidDensity(Stream *s);
    double gasZFactor(double yg, double t, double p);
    double gasDensity(double t, double p, double z);
    double surfaceTension(double gas_density, double liquid_density);
    double gasViscosity(double p, double z);
    double liquidViscosity(Stream *s);





public:
    BeggsBrillCalculator();

    virtual ~BeggsBrillCalculator();

    // virtual functions

    virtual PressureDropCalculator* clone() const {return new BeggsBrillCalculator(*this);}

    /**
     * @brief Calculates the pressure drop in the pipe for a given stream and outlet pressure
     *
     * @param s The rates going through the pipe segment
     * @param p_outlet The outlet pressure of the pipe
     * @return The calculated pressure drop in the pipe
     */
    virtual double pressureDrop(Stream *s, double p_outlet, Stream::units unit);

    // set functions

    void setDiameter(double d) {m_diameter = d;}
    void setLength(double l) {m_length = l;}
    void setAngle(double a) {m_angle = a;}
    void setTemperature(double t) {m_temperature = t;}
    void setGasSpecificGravity(double s) {m_sg_gas = s;}
    void setOilDensity(double d) {m_den_oil = d;}
    void setWaterDensity(double d) {m_den_wat = d;}
    void setOilViscosity(double v) {m_vis_oil = v;}
    void setWaterViscosity(double v) {m_vis_wat = v;}

    // get functions

    double diameter() {return m_diameter;}
    double length() {return m_length;}
    double temperature() {return m_temperature;}
    double gasSpecificGravity() {return m_sg_gas;}
    double oilDensity() {return m_den_oil;}
    double waterDensity() {return m_den_wat;}
    double oilViscosity() {return m_vis_oil;}
    double waterViscosity() {return m_vis_wat;}

    /**
     * @brief Returns the inclination angle of the pipe in either degrees or radians
     *
     * @param rad true if should return angle in rads
     * @return double
     */
    double angle(bool rad = false)
    {
        if(!rad) return m_angle;
        else return 3.14159265 * m_angle / 180;
    }



};

} // namespace ResOpt

#endif // BEGGSBRILLCALCULATOR_H
