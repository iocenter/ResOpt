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

#ifndef RESERVOIR_H
#define RESERVOIR_H

#include <QString>

namespace ResOpt
{


/**
 * @brief A container class for Reservoirs
 * @details A Reservoir holds information on the grid and fluid properties. This information is located in a file generated
 *          by the user. The validity of the reservoir input file is never checked by ResOpt, so care should be taken when
 *          generating this file.
 *
 */
class Reservoir
{
private:
    QString m_name; /**< TODO */
    QString m_file; /**< TODO */
    QString m_matlab_path;
    QString m_mrst_path;
    QString m_mrst_script;
    bool m_use_mrst_script;
    bool m_keep_mat_file;
    double m_endtime; /**< TODO */

    bool m_gas_phase;
    bool m_oil_phase;
    bool m_wat_phase;

    double m_den_gas;
    double m_den_oil;
    double m_den_wat;

public:
/**
 * @brief
 *
 */
    Reservoir();

    // misc functions
    QString description() const;

    // set functions

    /**
     * @brief Sets the name of the Reservoir
     *
     * @param n
     */
    void setName(const QString &n) {m_name = n;}

    /**
     * @brief Sets the file name of the file containing the reservoir description
     *
     * @param f
     */
    void setFile(const QString &f) {m_file = f;}

    void setMrstPath(const QString &p) {m_mrst_path = p;}
    void setMatlabPath(const QString &p) {m_matlab_path = p;}
    void setMrstScript(const QString &s) {m_mrst_script = s; m_use_mrst_script = true;}
    void setKeepMatFile(bool b) {m_keep_mat_file = b;}

    /**
     * @brief Sets the end time of the simulation
     *
     * @param t end time in days
     */
    void setEndTime(double t) {m_endtime = t;}

    void setPhases(bool gas, bool oil, bool water)
    {
        m_gas_phase = gas;
        m_oil_phase = oil;
        m_wat_phase = water;
    }
    void setDensities(double gas, double oil, double water)
    {
        m_den_gas = gas;
        m_den_oil = oil;
        m_den_wat = water;
    }

    // get functions

    /**
     * @brief Returns the name of the Reservoir
     *
     * @return QString
     */
    QString name() const {return m_name;}

    /**
     * @brief Returns the file name of the reservoir description file
     *
     * @return QString
     */
    QString file() const {return m_file;}

    QString matlabPath() const {return m_matlab_path;}
    QString mrstPath() const {return m_mrst_path;}
    QString mrstScript() const {return m_mrst_script;}
    bool useMrstScript() const {return m_use_mrst_script;}
    bool keepMatFile() const {return m_keep_mat_file;}

    /**
     * @brief Returns the end time of the simulation
     *
     * @return End time in days
     */
    double endTime() const {return m_endtime;}

    double gasDensity() {return m_den_gas;}
    double oilDensity() {return m_den_oil;}
    double waterDensity() {return m_den_wat;}

    int numberOfPhases() {return m_gas_phase + m_oil_phase + m_wat_phase;}
    bool gasPhase() {return m_gas_phase;}
    bool oilPhase() {return m_oil_phase;}
    bool waterPhase() {return m_wat_phase;}
};

} // namespace ResOpt

#endif // RESERVOIR_H
