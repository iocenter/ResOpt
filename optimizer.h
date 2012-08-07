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

#ifndef OPTIMIZER_H
#define OPTIMIZER_H

#include <QObject>


namespace ResOpt
{


class Runner;
class CaseQueue;
class Case;
class Component;



/**
 * @brief Abstract base class for optimization algorithms.
 *
 */
class Optimizer : public QObject
{
    Q_OBJECT
private:
    Runner *p_runner;

    int m_max_iter;
    int m_parallel_runs;
    double m_perturbation_size;



    bool m_initialized;

public:
    explicit Optimizer(Runner *r);
    virtual ~Optimizer();

    virtual void initialize() = 0;

    virtual void start() = 0;


    void sendBestCaseToRunner(Case *c);

    Runner* runner() {return p_runner;}

    // set functions
    void setMaxIterations(int i) {m_max_iter = i;} 
    void setParallelRuns(int i) {m_parallel_runs = i;}
    void setPerturbationSize(double p) {m_perturbation_size = p;}
    void setInitialized(bool i) {m_initialized = i;}

    // get functions
    int maxIterations() const {return m_max_iter;}
    int parallelRuns() const {return m_parallel_runs;}
    double pertrurbationSize() const {return m_perturbation_size;}
    bool isInitialized() const {return m_initialized;}

signals:

    /**
     * @brief Signal that should be emitted when the optimizer finishes.
     *
     */
    void finished();


    /**
     * @brief Sends the best case (final solution) to the runner for printing.
     *
     * @param c
     */
    void bestCase(Case *c);

public slots:

    /**
     * @brief Sends a list of cases to the Runner for evaluation.
     * @details The cases are populated with results (constraint and objective values) by the runner, and can be processed by the optimizer
     *
     * @param cases
     */
    void runCases(CaseQueue *cases, Component *comp = 0);


    /**
     * @brief Overloaded function.
     * @details This function creates a CaseQueue consisting of a single Case, c, and sends it to runCases().
     *
     * @param c
     */
    void runCase(Case *c, Component *comp = 0);



};

} // namespace ResOpt


#endif // OPTIMIZER_H
