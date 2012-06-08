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

#ifndef NPVOBJECTIVE_H
#define NPVOBJECTIVE_H

#include "objective.h"

/**
 * @brief Class for objectives that maximize net present value
 *
 */
class NpvObjective : public Objective
{
private:
    double m_dcf;           // discount factor /**< TODO */
    double m_price_oil;     // oil price /**< TODO */
    double m_price_gas;     // gas price /**< TODO */
    double m_price_water;   // water price /**< TODO */

public:
/**
 * @brief
 *
 */
    NpvObjective();

    // set functions

    /**
     * @brief Set the discount factor
     *
     * @param d
     */
    void setDcf(double d) {m_dcf = d;}
    /**
     * @brief Sets the oil price
     *
     * @param p
     */
    void setOilPrice(double p) {m_price_oil = p;}
    /**
     * @brief Sets the gas price
     *
     * @param p
     */
    void setGasPrice(double p) {m_price_gas = p;}
    /**
     * @brief Sets the water price
     *
     * @param p
     */
    void setWaterPrice(double p) {m_price_water = p;}

    // get functions

    /**
     * @brief Returns the discount factor
     *
     * @return double
     */
    double dcf() {return m_dcf;}
    /**
     * @brief Returns the oil price
     *
     * @return double
     */
    double oilPrice() {return m_price_oil;}
    /**
     * @brief Returns the gas price
     *
     * @return double
     */
    double gasPrice() {return m_price_gas;}
    /**
     * @brief Returns the water price
     *
     * @return double
     */
    double waterPrice() {return m_price_water;}

    // virtuals

    /**
     * @brief Calculates the NPV based on the input streams, the discount factor, and the product prices
     *
     * @param s
     */
    virtual void calculateValue(QVector<Stream*> s);




};

#endif // NPVOBJECTIVE_H
