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

#include <QString>
#include <QVector>
#include <tr1/memory>

using std::tr1::shared_ptr;




namespace ResOpt
{

class Well;
class Reservoir;
class Objective;
class Pipe;
class Capacity;
class IntVariable;
class RealVariable;
class BinaryVariable;
class Constraint;
class Component;
class UserConstraint;
class Cost;


/**
 * @brief Abstract base class for Models.
 * @details The Model stores all the sub-parts of the problem (wells, pipes, separators, etc).
 *
 */
class Model
{
private:
    Reservoir *p_reservoir; /**< TODO */
    QVector<Well*> m_wells; /**< TODO */
    QVector<Pipe*> m_pipes;
    QVector<Capacity*> m_capacities;
    Objective *p_obj;
    QVector<double> m_master_schedule;

    QVector<UserConstraint*> m_user_constraints;

    bool m_up_to_date;      // true if the model has been evaluated

    QString m_driver_path;





    /**
     * @brief Updates the capacity constraints for all the Capacities in the model.
     * @details This function calls in turn Capacity::updateConstraints() on all the capacities.
     *
     * @return bool
     */
    bool updateCapacityConstraints();



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


    bool updateBoosterConstraints();


    /**
     * @brief Updates the user defined constraints
     *
     * @return bool
     */
    bool updateUserDefinedConstraints();

    QVector<Cost*> sortCosts(QVector<Cost*> c);



public:
    Model();
    Model(const Model &m);
    virtual ~Model();

    // virtual functions
    virtual QString description() const = 0;


    /**
     * @brief Calcuates the entire upstream part of the model
     * @details This function updates the streams in the pipe network, calculates pressures,
     *          updates constraint values and objective.
     *
     */
    virtual void process() = 0;


    /**
     * @brief Updates the streams in the pipe network.
     *
     */
    virtual void updateStreams() = 0;


    /**
     * @brief Returns a clone of this model.
     *
     * @return Model
     */
    virtual Model* clone() const = 0;


    /**
     * @brief Returns a vector containing all integer variables defined within the model.
     * @details These are all the routing variables for the model.
     *
     * @return QVector<BinaryVariable *>
     */
    virtual QVector<shared_ptr<BinaryVariable> >& binaryVariables(bool force_refresh = false) = 0;

    /**
     * @brief Returns a vector containing all the real variables defined within the model.
     * @details These are all the well control variables.
     *
     * @return QVector<RealVariable *>
     */
    virtual QVector<shared_ptr<RealVariable> >& realVariables(bool force_refresh = false) = 0;



    /**
     * @brief Returns a vector containing all the integer variables.
     * @details These are the install time variables for the separators.
     *
     * @return QVector<shared_ptr<IntVariable> >
     */
    virtual QVector<shared_ptr<IntVariable> >& integerVariables(bool force_refresh = false) = 0;

    /**
     * @brief Returns a vector containing all the constraints defined within the model.
     * @details The constraints include well BHP constraints and capasity constraints
     *
     * @return QVector<Constraint *>
     */
    virtual QVector<shared_ptr<Constraint> >& constraints(bool force_refresh = false) = 0;


    /**
     * @brief Returns a vector of the real variables associated with the Component c.
     *
     * @param c
     * @return QVector<shared_ptr<RealVariable> >
     */
    virtual QVector<shared_ptr<RealVariable> > realVariables(Component *c) = 0;


    /**
     * @brief Initializes the model.
     * @detials This function sets up the constraints associated with production wells and separators, and connects the pipe network
     *          to the wells and separators. The function must be called before the model is used for anything.
     *
     */
    virtual void initialize() = 0;



    /**
     * @brief Updates the value of all constraints in the model.
     *
     * @return bool
     */
    virtual bool updateConstraints() = 0;


    virtual int numberOfRealVariables() const = 0;
    virtual int numberOfBinaryVariables() const = 0;
    virtual int numberOfIntegerVariables() const = 0;
    virtual int numberOfConstraints() const = 0;

    virtual double realVariableValue(int i) const = 0;
    virtual double binaryVariableValue(int i) const = 0;
    virtual double integerVariableValue(int i) const = 0;
    virtual double constraintValue(int i) const = 0;

    // misc functions



    /**
     * @brief Makes sure that the Model is set up propperly.
     * @details This function should be called before the Model is used for anything. It checks that all the sub parts of the Model
     *          are defined, that the master schedule corresponds to the schedule of all the wells, etc.
     *
     * @return bool
     */
    bool validate();



    /**
     * @brief Checks the current routing of wells and pipes, and connects them correctly.
     * @details Wells and Pipes may have an OUTLETPIPE defined. This is either a fixed number, or a routing variable.
     *          This function tries to resolve the current value of the OUTLETPIPE to a NUMBER assigned to a Pipe.
     *
     * @return true if successful, false if not resolved
     */
    bool resolvePipeRouting();


    /**
     * @brief Translates the input PIPE numbers given in the driver file to pointers to Pipes.
     * @details This functions should only be called once, before the optimization starts. The input pipes to a given Capacity are fixed.
     *
     * @return bool
     */
    bool resolveCapacityConnections();








    /**
     * @brief Calculates the pressure drops in all the pipes
     *
     * @return bool
     */
    bool calculatePipePressures();





    /**
     * @brief Reads the pipe pressure drop definition files for all pipes in the model.
     * @details These are the files speficied with the FILE keyword in the main driver file.
     *
     */
    void readPipeFiles();




    /**
     * @brief Updates the value of the Objective from the current Streams of all the wells in the model.
     *
     */
    void updateObjectiveValue();


    /**
     * @brief Updates the value of the constraints that are common for all model types
     *
     */
    bool updateCommonConstraints();





    // set functions

    void setMasterSchedule(const QVector<double> &schedule) {m_master_schedule = schedule;}

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

    void setUpToDate(bool b) {m_up_to_date = b;}

    void setDriverPath(const QString &path) {m_driver_path = path;}

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
     * @brief Adds a Capacity to the model
     *
     * @param s
     */
    void addCapacity(Capacity *s) {m_capacities.push_back(s);}


    /**
     * @brief Adds a user defined constraint to the model
     *
     * @param uc
     */
    void addUserDefinedConstraint(UserConstraint *uc) {m_user_constraints.push_back(uc);}


    // get functions

    const QString& driverPath() {return m_driver_path;}

    int numberOfMasterScheduleTimes() const {return m_master_schedule.size();}
    double masterScheduleTime(int i) const {return m_master_schedule.at(i);}
    QVector<double>& masterSchedule() {return m_master_schedule;}

    /**
     * @brief Returns the Reservoir
     *
     * @return Reservoir
     */
    Reservoir* reservoir() const {return p_reservoir;}

    /**
     * @brief Returns well number i
     *
     * @param i
     * @return Well
     */
    Well* well(int i) const {return m_wells.at(i);}

    /**
     * @brief Returns the total number of wells defined in the model
     *
     * @return int
     */
    int numberOfWells() const {return m_wells.size();}


    /**
     * @brief Returns the well with the specified id, 0 if not found
     *
     * @param id
     * @return Well
     */
    Well* wellById(int comp_id);
    Well* wellByName(const QString &name);

    /**
     * @brief Returns Pipe number i
     *
     * @param i
     * @return Pipe
     */
    Pipe* pipe(int i) const {return m_pipes.at(i);}

    /**
     * @brief Returns the total number of pipes defined in the model
     *
     * @return int
     */
    int numberOfPipes() const {return m_pipes.size();}


    /**
     * @brief Returns Capacity number i
     *
     * @param i
     * @return Capacity
     */
    Capacity* capacity(int i) {return m_capacities.at(i);}

    /**
     * @brief Returns the total number of capacities defined in the model
     *
     * @return int
     */
    int numberOfCapacities() const {return m_capacities.size();}


    UserConstraint* userDefinedConstraint(int i) {return m_user_constraints.at(i);}
    int numberOfUserDefinedConstraints() {return m_user_constraints.size();}


    /**
     * @brief Returns the Objective
     *
     * @return Objective
     */
    Objective* objective() {return p_obj;}

    bool isUpToDate() const {return m_up_to_date;}

    Model& operator=(const Model &rhs);
};

} // namespace ResOpt

#endif // MODEL_H
