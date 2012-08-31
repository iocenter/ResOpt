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

#ifndef WELL_H
#define WELL_H

#include <QString>
#include <tr1/memory>

#include "component.h"
#include "wellcontrol.h"

using std::tr1::shared_ptr;


namespace ResOpt
{
class WellConnection;
class Stream;
class Cost;
class IntVariable;


/**
 * @brief Base class for representations of wells.
 *
 */
class Well : public Component
{
public:
    /**
     * @brief
     *
     */
    enum well_type {PROD, INJ};

private:
    Well::well_type m_type;                     // type of well (production or injection) /**< TODO */
    QString m_name;                             // well name /**< TODO */
    QString m_group;                            // group the well belongs to /**< TODO */
    double m_bhp_limit;                         // the bhp limit when the well is on rate control
    WellControl::contol_type m_bhp_inj;         // the phase injected when injector is on BHP control


    QVector<WellConnection*> m_connections;     // the perforations of the well
    QVector<WellControl*> m_schedule;           // the control schedule of the well /**< TODO */

    Cost *p_cost;                               // the cost associated with installing the well (only used if the install time is set
    shared_ptr<IntVariable> p_install_time;     // the installation time of the separator




public:
/**
 * @brief
 *
 */
    Well();
    Well(const Well &w);

    virtual ~Well();

    // misc functions

    /**
     * @brief Initializes all the streams
     *
     */
    void initialize();

    bool isInstalled(int i);
    bool hasInstallTime() {return p_install_time != 0;}
    bool hasCost() {return p_cost != 0;}

    // virtual functions

    virtual Well* clone() const = 0;
    virtual void setAutomaticType() = 0;




    // add functions

    /**
     * @brief Adds a new connection (perforation) to the well.
     *
     * @param c
     */
    void addConnection(WellConnection *c) {m_connections.push_back(c);}

    /**
     * @brief Adds a control element to the schedule of the well.
     *
     * @param c The control element to be added
     */
    void addControl(WellControl *c);



    // set functions


    /**
     * @brief Sets the type of well, either injector or producer
     *
     * @param t
     */
    void setType(Well::well_type t) {m_type = t;}

    /**
     * @brief Sets the well name
     *
     * @param n
     */
    void setName(const QString &n) {m_name = n;}

    /**
     * @brief Sets the group that the well belongs to
     *
     * @param g
     */
    void setGroup(const QString &g) {m_group = g;}


    /**
     * @brief Sets the BHP limit for the well
     * @details The BHP limit is the minimum allowed pressure for producers, and the maximum allowes pressure for
     *          injectors. The limit is only used when the well is on rate control, and is not considered a constraint
     *          by the optimizer, only by the simulator.
     *
     * @param p
     */
    void setBhpLimit(double p) {m_bhp_limit = p;}


    /**
     * @brief Sets the default injection phase when injection well is on BHP control
     *
     * @param t
     */
    void setBhpInj(WellControl::contol_type t) {m_bhp_inj = t;}

    void setCost(Cost *c) {p_cost = c;}
    void setInstallTime(shared_ptr<IntVariable> t) {p_install_time = t;}




    // get functions

    /**
     * @brief Returns the well type, either injector or producer
     *
     * @return Well::well_type
     */
    Well::well_type type() const {return m_type;}


    /**
     * @brief Returns the well type as a string. "INJ" for injectors, "PROD" for producers
     *
     * @return QString
     */
    QString typeAsString() const
    {
        if(m_type == Well::PROD) return "P";
        else return "I";
    }

    /**
     * @brief Rewturns the well name
     *
     * @return QString
     */
    const QString& name() const {return m_name;}

    /**
     * @brief Returns the group that the well belongs to
     *
     * @return QString
     */
    const QString& group() const {return m_group;}

    /**
     * @brief Returns the BHP limit of the well
     *
     * @return double
     */
    double bhpLimit() const {return m_bhp_limit;}


    /**
     * @brief Returns the injected phase when on BHP control
     *
     * @return WellControl::contol_type
     */
    WellControl::contol_type bhpInj() const {return m_bhp_inj;}

    /**
     * @brief Returns the number of cells that the well is connected to (perforations)
     *
     * @return int
     */
    int numberOfConnections() const {return m_connections.size();}

    /**
     * @brief Returns the cell number of connection i
     *
     * @param i
     * @return Connection number i
     */
    WellConnection* connection(int i) {return m_connections.at(i);}

    /**
     * @brief Returns the number of well control set points in the shcedule of the well.
     *
     * @return int
     */
    int numberOfControls() const {return m_schedule.size();}

    /**
     * @brief Returns WellControl number i in the schedule
     *
     * @param i
     * @return WellControl
     */
    WellControl* control(int i) {return m_schedule.at(i);}

    Cost* cost() {return p_cost;}
    shared_ptr<IntVariable> installTime() {return p_install_time;}









};

} // namespace ResOpt

#endif // WELL_H
