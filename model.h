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

#ifndef MODEL_H
#define MODEL_H

#include <QVector>
#include <tr1/memory>

using std::tr1::shared_ptr;




namespace ResOpt
{

class Well;
class Reservoir;
class Objective;
class Pipe;
class Separator;
class IntVariable;
class RealVariable;
class BinaryVariable;
class Constraint;


/**
 * @brief Class that keeps track of all the parts of the model structure
 *
 */
class Model
{
private:
    Reservoir *p_reservoir; /**< TODO */
    QVector<Well*> m_wells; /**< TODO */
    QVector<Pipe*> m_pipes;
    QVector<Separator*> m_separators;
    Objective *p_obj; /**< TODO */

    QVector<shared_ptr<RealVariable> > m_vars_real;     // vector containing all real variables
    QVector<shared_ptr<BinaryVariable> > m_vars_binary;
    QVector<shared_ptr<Constraint> > m_cons;


    /**
     * @brief Updates the separator constraints for all the separators in the model.
     * @details This function calls in turn Separator::updateConstraints() on all the separators.
     *
     * @return bool
     */
    bool updateSeparatorConstraints();



    /**
     * @brief Updates the BHP and connection constraints on the Wells
     *
     * @return bool
     */
    bool updateWellConstaints();


    /**
     * @brief Updates the pipe connnection constraints
     *
     * @return bool
     */
    bool updatePipeConstraints();



public:
/**
 * @brief
 *
 */
    Model();
    /**
     * @brief
     *
     */
    ~Model();


    /**
     * @brief Checks the current routing of wells and pipes, and connects them correctly.
     * @details Wells and Pipes may have an OUTLETPIPE defined. This is either a fixed number, or a routing variable.
     *          This function tries to resolve the current value of the OUTLETPIPE to a NUMBER assigned to a Pipe.
     *
     * @return true if successful, false if not resolved
     */
    bool resolvePipeRouting();


    /**
     * @brief Calculates the pressure drops in all the pipes
     *
     * @return bool
     */
    bool calculatePipePressures();


    /**
     * @brief Translates the input PIPE numbers given in the driver file to pointers to Pipes.
     * @details This functions should only be called once, before the optimization starts. The input pipes to a given separator are fixed.
     *
     * @return bool
     */
    bool resolveSeparatorConnections();




    /**
     * @brief Reads the pipe pressure drop definition files for all pipes in the model.
     * @details These are the files speficied with the FILE keyword in the main driver file.
     *
     */
    void readPipeFiles();




    /**
     * @brief Returns a vector containing all integer variables defined within the model.
     * @details These are all the routing variables for the model.
     *
     * @return QVector<BinaryVariable *>
     */
    QVector<shared_ptr<BinaryVariable> > binaryVariables();

    /**
     * @brief Returns a vector containing all the real variables defined within the model.
     * @details These are all the well control variables.
     *
     * @return QVector<RealVariable *>
     */
    QVector<shared_ptr<RealVariable> > realVariables();


    /**
     * @brief Returns a vector containing all the constraints defined within the model.
     * @details The constraints include well BHP constraints and separator capasity constraints
     *
     * @return QVector<Constraint *>
     */
    QVector<shared_ptr<Constraint> > constraints();


    /**
     * @brief Updates the value of the Objective from the current Streams of all the wells in the model.
     *
     */
    void updateObjectiveValue();


    /**
     * @brief Updates the value of all the constraints in the model
     *
     */
    bool updateConstraints();


    // set functions
    /**
     * @brief Sets the Reservoir
     *
     * @param r
     */
    void setReservoir(Reservoir *r) {p_reservoir = r;}
    /**
     * @brief Sets the Objective
     *
     * @param o
     */
    void setObjective(Objective *o) {p_obj = o;}

    // add functions
    /**
     * @brief Adds a Well to the model
     *
     * @param w
     */
    void addWell(Well *w) {m_wells.push_back(w);}


    /**
     * @brief Adds a Pipe to the model
     *
     * @param p
     */
    void addPipe(Pipe *p) {m_pipes.push_back(p);}


    /**
     * @brief Adds a separator to the model
     *
     * @param s
     */
    void addSeparator(Separator *s) {m_separators.push_back(s);}

    // get functions
    /**
     * @brief Returns the Reservoir
     *
     * @return Reservoir
     */
    Reservoir* reservoir() {return p_reservoir;}

    /**
     * @brief Returns well number i
     *
     * @param i
     * @return Well
     */
    Well* well(int i) {return m_wells.at(i);}

    /**
     * @brief Returns the total number of wells defined in the model
     *
     * @return int
     */
    int numberOfWells() const {return m_wells.size();}

    /**
     * @brief Returns Pipe number i
     *
     * @param i
     * @return Pipe
     */
    Pipe* pipe(int i) {return m_pipes.at(i);}

    /**
     * @brief Returns the total number of pipes defined in the model
     *
     * @return int
     */
    int numberOfPipes() const {return m_pipes.size();}


    /**
     * @brief Returns Separator number i
     *
     * @param i
     * @return Separator
     */
    Separator* separator(int i) {return m_separators.at(i);}

    /**
     * @brief Returns the total number of separators defined in the model
     *
     * @return int
     */
    int numberOfSeparators() const {return m_separators.size();}


    /**
     * @brief Returns the Objective
     *
     * @return Objective
     */
    Objective* objective() {return p_obj;}
};

} // namespace ResOpt

#endif // MODEL_H
