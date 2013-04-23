#ifndef PRESSUREBOOSTER_H
#define PRESSUREBOOSTER_H

#include "pipe.h"

#include <tr1/memory>
#include <QVector>

using std::tr1::shared_ptr;

namespace ResOpt
{

class Cost;
class IntVariable;
class RealVariable;
class Constraint;
class PipeConnection;


/**
 * @brief A pressure booster that reduces the inlet pressure
 * @details
 *
 */
class PressureBooster : public Pipe
{
private:

    Cost *p_cost;                                               // the cost associated with installing the separator
    shared_ptr<IntVariable> p_install_time;                     // the installation time of the separator
    shared_ptr<RealVariable> p_pressure_change;                 // the pressure that the booster can increase
    shared_ptr<RealVariable> p_capacity;                        // the maximum capacity of the booster
    QVector<shared_ptr<Constraint> > m_capacity_constraints;    // constraint on the maximum capacity of the booster
    PipeConnection *p_outlet_connection;                        // the outlet pipe that this separator feeds




public:
    PressureBooster();
    PressureBooster(const PressureBooster &p);

    virtual ~PressureBooster();

    // virtual functions

    virtual Pipe* clone() {return new PressureBooster(*this);}

    virtual void calculateInletPressure();

    virtual QString description() const;

    // misc functions

    void setupCapacityConstraints(const QVector<double> &master_schedule);
    void updateCapacityConstraints();


    // set functions
    void setOutletConnection(PipeConnection *c) {p_outlet_connection = c;}
    void setCost(Cost *c) {p_cost = c;}
    void setInstallTime(shared_ptr<IntVariable> t) {p_install_time = t;}
    void setPressureVariable(shared_ptr<RealVariable> p) {p_pressure_change = p;}
    void setCapacityVariable(shared_ptr<RealVariable> c) {p_capacity = c;}

    // get functions
    PipeConnection* outletConnection() {return p_outlet_connection;}
    Cost* cost() {return p_cost;}
    shared_ptr<IntVariable> installTime() {return p_install_time;}
    shared_ptr<RealVariable> pressureVariable() {return p_pressure_change;}
    shared_ptr<RealVariable> capacityVariable() {return p_capacity;}
    QVector<shared_ptr<Constraint> > capacityConstraints() {return m_capacity_constraints;}

};

} // namespace ResOpt

#endif // PRESSUREBOOSTER_H
