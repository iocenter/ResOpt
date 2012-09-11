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
class Component;
class Pipe;
class Well;

/**
 * @brief Launches the project.
 * @details This class takes care of launching the project for a set of variable values. The launcher holds a copy of the Model and the ReservorSimulator. Each Launcher lives in a separate
 *          thread. Multiple launchers may be run in parallel to exploit the available cores on the computer.
 *
 */
class Launcher : public QObject
{
    Q_OBJECT
private:
    Model *p_model;
    ReservoirSimulator *p_simulator;

    int m_number_of_runs;


    /**
     * @brief Checks if the reservoir simulator must be rerun for the Case.
     * @details This function checks if any of the real variable values in the Case that corresponds to well control variables have changed
     *          from what is currently in the model. The function also checks if the simulator has been launched.
     *
     * @param c
     * @return bool
     */
    bool rerunReservoirSimulator(Case *c);

    void evaluateEntireModel(Case *c);
    void evaluatePipe(Case *c, Pipe *p);
    void evaluateWell(Case *c, Well *w);



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
    void finished(Launcher *source, Component *comp);

    void runningReservoirSimulator();


public slots:

    /**
     * @brief Evaluates the project with the variable values defined in the Case c.
     * @details First the reservoir simulator is launched, then the pipe network is evaluated. After the project is evaluated, the constraints and objective are updated,
     *          and the updated values are copied to the Case c.
     *
     * @param c
     */
    void evaluate(Case *c, Component *comp);


};

} // namespace ResOpt

#endif // LAUNCHER_H
