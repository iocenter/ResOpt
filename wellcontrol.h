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

#ifndef WELLCONTROL_H
#define WELLCONTROL_H

#include <tr1/memory>




using std::tr1::shared_ptr;

namespace ResOpt
{

class RealVariable;


/**
 * @brief Class containing information on one schedule entry for a Well
 * @details This class holds information on one set point in the schedule for a Well, and is used to generate the input
 *          schedule for the reservoir simulator. The time, type, and set point (rate or pressure) is stored.
 *
 */
class WellControl
{
public:
    /**
     * @brief The different types of control that can be used on a well
     *
     */
    enum contol_type {BHP, QOIL, QGAS, QWAT};

private:
    shared_ptr<RealVariable> p_control_var;        // variable containing current set point /**< TODO */
    WellControl::contol_type m_type;    // type of well control /**< TODO */
    double m_end_time;                      // stop time /**< TODO */


public:
/**
 * @brief
 *
 */
    WellControl();

    WellControl(const WellControl &c);

    ~WellControl();

    // set functions

    /**
     * @brief Sets the Variable that is going to be used for this shcedule entry
     *
     * @param v
     */
    void setControlVar(shared_ptr<RealVariable> v) {p_control_var = v;}

    /**
     * @brief Set the type of control to be used for this Well at this time
     *
     * @param t
     */
    void setType(WellControl::contol_type t) {m_type = t;}

    /**
     * @brief Sets the end time of the shcedule entry
     *
     * @param t
     */
    void setEndTime(double t) {m_end_time = t;}


    // get functions

    /**
     * @brief Returns the Variable used for the set point in this schedule entry
     *
     * @return RealVariable
     */
    shared_ptr<RealVariable> controlVar() {return p_control_var;}

    /**
     * @brief Returns the type of control
     *
     * @return WellControl::contol_type
     */
    WellControl::contol_type type() const {return m_type;}

    /**
     * @brief Returns the end time
     *
     * @return double
     */
    double endTime() const {return m_end_time;}


};

} // namespace ResOpt

#endif // WELLCONTROL_H
