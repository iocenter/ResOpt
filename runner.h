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


#ifndef RUNNER_H
#define RUNNER_H


/**
 * @brief Class that controls the main execution of ResOpt
 *
 */

#include <tr1/memory>
#include <time.h>

#include <QString>
#include <QFile>
#include <QVector>
#include <QObject>

class QThread;

using std::tr1::dynamic_pointer_cast;
using std::tr1::shared_ptr;


namespace ResOpt
{

class Model;
class ReservoirSimulator;
class ModelReader;
class Optimizer;
class Launcher;
class CaseQueue;
class Case;
class Component;


/**
 * @brief Main execution class.
 *
 */
class Runner : public QObject
{
    Q_OBJECT
private:
    QVector<Launcher*> m_launchers;
    QVector<bool> m_launcher_running;
    QVector<QThread*> m_threads;

    ModelReader *p_reader;
    Model *p_model;
    ReservoirSimulator *p_simulator;
    Optimizer *p_optimizer;

    QFile *p_summary;
    QFile *p_debug;
    int m_number_of_runs;
    int m_number_of_res_sim_runs;
    time_t m_start_time;

    CaseQueue *p_cases;
    Launcher *p_last_run_launcher;
    bool m_paused;




    /**
     * @brief Writes the problem definition to the summary file
     * @details A list of all the variables and constraints are printed to the summary file.
     */
    void writeProblemDefToSummary();


    /**
     * @brief Writes the results from all the cases to the summary file
     * @details The variable, constraints and objective values are printed as a new line to the summary file.
     */
    void writeCasesToSummary();






public:
    explicit Runner(const QString &driver_file, QObject *parent = 0);
    ~Runner();


    /**
     * @brief Initializes the model and optimizer.
     * @details The Model is first read from the driver file. Then the ReservoirSimulator and Optimizer are initialized.
     *
     */
    void initialize();

    void initializeLaunchers();


    void printDebug(Launcher *l);

    void transferModelStateFromLauncher();

    /**
     * @brief Checks if any of the constraints are broken. Returns true if the solution is feasible.
     *
     * @param c
     * @return bool
     */
    bool isFeasible(Case *c);


    // set functions

    void setSummaryFile(const QString &f);
    void setDebugFile(const QString &f);

    void setOptimizer(Optimizer *o) {p_optimizer = o;}
    void setReservoirSimulator(ReservoirSimulator *s) {p_simulator = s;}

    // get functions
    Model* model() {return p_model;}
    Optimizer* optimizer() {return p_optimizer;}
    ReservoirSimulator* reservoirSimulator() {return p_simulator;}



public slots:

    /**
     * @brief This slot is called whenever a Launcher finishes with its model evaluation.
     * @details If there are more cases to run in the queue, the launcher is sent a new case from the queue to evaluate. If there are no more cases to run, this function checks if all the other
     *          launchers have finished. If all the launchers have finished, casesFinished() is emitted, and the results are written to the summary file.
     *
     * @param l
     */
    void onLauncherFinished(Launcher *l, Component *comp, Case *finished_case);

    /**
     * @brief Starts the Optimizer
     * @details The function first checks if the Model has been initialized. If not initialize() is called. Then the Optimizer is started
     *          through Optimizer::start().
     *
     */
    void run();


    /**
     * @brief Evaluates a list of cases.
     * @details This function is called by an Optimizer, or another class that needs to evaluate the Model.
     *          The cases in the queue is distrubuted among the Launchers for model evaluation. When a Launcher finishes the evaluation,
     *          it emits a signal that is connected to the onLauncherFinished() slot. Further distrubution of cases is take from there. When
     *          calling this function, it should be done within an event loop. The event loop should wait for the casesFinished() signal before
     *          proceeding.
     *
     * @param cases A list of the cases that should be run
     * @param comp A pointer to the component of the Model that should be evaluated. If this is a null pointer, the entire Model is evaluated.
     */
    void evaluate(CaseQueue *cases, Component *comp);




    void writeBestCaseToSummary(Case *c);


    /**
     * @brief Increases the number of simulator runs by 1
     *
     */
    void incrementReservoirSimRuns() {++m_number_of_res_sim_runs;}

    void setPaused(bool paused);




signals:
    void optimizationFinished();
    void casesFinished();
    void sendCase(Case *c, Component *comp);
    void newCaseFinished(Case *c);
    void resumePaused();

};

} // namespace ResOpt

#endif // RUNNER_H

