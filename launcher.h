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


#ifndef LAUNCHER_H
#define LAUNCHER_H

#include <QObject>

namespace ResOpt
{

class Model;
class ReservoirSimulator;
class Case;


/**
 * @brief Launches the project.
 * @details This class takes care of launching the project for a set of variable values.
 *
 */
class Launcher : public QObject
{
    Q_OBJECT
private:
    Model *p_model;
    ReservoirSimulator *p_simulator;



public:
    explicit Launcher(QObject *parent = 0);

    bool initialize();

    // set functions
    void setModel(Model *m) {p_model = m;}
    void setReservoirSimulator(ReservoirSimulator *s) {p_simulator = s;}

    // get functions
    Model* model() {return p_model;}
    ReservoirSimulator* reservoirSimulator() {return p_simulator;}
    
signals:

    /**
     * @brief Emitted when evaluate() finished.
     * @details This signal is connected to the onLauncherFinished() slot in the Runner.
     *
     * @param source
     */
    void finished(Launcher *source);


public slots:

    /**
     * @brief Evaluates the project with the variable values defined in the Case c.
     * @details First the reservoir simulator is launched, then the pipe network is evaluated. After the project is evaluated, the constraints and objective are updated,
     *          and the updated values are copied to the Case c.
     *
     * @param c
     */
    void evaluate(Case *c);
    
};

} // namespace ResOpt

#endif // LAUNCHER_H
