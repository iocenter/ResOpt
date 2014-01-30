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


#ifndef MODELREADER_H
#define MODELREADER_H

#include <QString>
#include <QStringList>
#include <QFile>
#include <QVector>

namespace ResOpt
{
class Runner;
class Model;
class Well;
class ProductionWell;
class Reservoir;
class Pipe;
class MidPipe;
class Objective;
class Capacity;
class WellPath;


/**
 * @brief Reads the driver file, and creates a Model.
 *
 */
class ModelReader
{
private:

    QFile m_driver_file;
    QString m_path;

    QStringList processLine(const QString& line);
    bool isEmpty(const QStringList &list);


    /**
     * @brief Reads the MASTERSCHEDULE part of the driver file
     *
     * @return QVector<double>
     */
    QVector<double> readMasterSchedule();

    /**
     * @brief Reads a RESERVOIR section of the driver file.
     * @details Generates a Reservoir object based on the information in the driver file.
     *  This object should be sent to a Model for storage.
     *
     * @return Reservoir Pointer to the generated Reservoir
     */
    Reservoir* readReservoir();



    /**
     * @brief  Reads a WELL section of the driver file
     *
     * @return Well
     */
    Well* readWell();


    /**
     * @brief Reads the SCHEDULE part of a well definition.
     *
     * @param w pointer to the well where the schedule should be added
     * @return true if everything went ok, false if errors were enocuntered
     */
    bool readWellSchedule(Well *w);

    /**
     * @brief Reads the GASLIFT part of a well definition.
     *
     * @param w pointer to the well where the gas lift schedule should be added
     * @return true if everything went ok, false if errors were enocuntered
     */
    bool readGasLiftSchedule(ProductionWell *w);


    /**
     * @brief Reads the CONNECTIONS part of a well definition
     *
     * @param w pointer to the well where the connections should be added
     * @return true if everything went ok, false if errors were enocuntered
     */
    bool readWellConnections(Well *w);

    bool readVariableWellConnections(Well *w);

    bool readWellPath(Well *w);
    bool readWellPathHeel(WellPath *wp, Well *w);
    bool readWellPathToe(WellPath *wp, Well *w);
    bool readWellPathOptions(WellPath *wp, Well *w);


    /**
     * @brief Reads the OUTLETPIPES part of a well definition
     *
     * @param w
     * @return bool
     */
    bool readPipeConnections(ProductionWell *w);


    /**
     * @brief Reads the OUTLETPIPES part of a pipe definition
     *
     * @param p
     * @return bool
     */
    bool readPipeConnections(MidPipe *p);


    /**
     * @brief Reads the objective definition in the driver file
     *
     * @return Objective
     */
    Objective* readObjective();


    /**
     * @brief Reads a PIPE definition from the driver file
     *
     * @return Pipe
     */
    Pipe* readPipe();


    /**
     * @brief Reads a CAPACITY definition from the driver file
     *
     * @return Capacity
     */
    Capacity* readCapacity();



    /**
     * @brief Reads a SEPARATOR definition from the driver file
     *
     * @return Separator
     */
    Pipe* readSeparator();


    /**
     * @brief Reads a BOOSTER definition from the driver file
     *
     * @return PressureBooster
     */
    Pipe* readPressureBooster();



    /**
     * @brief Reads the optimizer section of the driver file, and sets the optimizer for the runner.
     *
     * @param r
     */
    void readOptimizer(Runner *r);


    /**
     * @brief Reads the CONSTRAINTS section of the driver file
     *
     * @param m
     */
    void readUserDefinedConstraints(Model *m);

public:

    ModelReader(const QString &driver);

    /**
     * @brief Reads the driver file, and sets up all the data structures
     *
     */
    Model* readDriverFile(Runner *r);

    void setDriverFilePath(const QString &path) {m_path = path;}

    const QString& driverFilePath() {return m_path;}


};


} // namespace ResOpt

#endif // MODELREADER_H
