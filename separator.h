#ifndef SEPARATOR_H
#define SEPARATOR_H

#include "pipe.h"

#include <tr1/memory>

using std::tr1::shared_ptr;

namespace ResOpt
{

class Cost;
class IntVariable;
class PipeConnection;


/**
 * @brief A separator that can remove phases
 * @details The separator can be fed by any number of pipes, and feeds one upstream pipe. The total rates going through the separator is calculated from the feed pipes.
 *          An installation time is set for the separator. From the installation time and on, any of the phases flowing through the separator can be removed.
 *
 */
class Separator : public Pipe
{
private:

    bool m_remove_water;        // true if the water should be discarded
    bool m_remove_oil;          // true if the oil should be discarded
    bool m_remove_gas;          // true if the gas should be discarded

    Cost *p_cost;                               // the cost associated with installing the separator
    shared_ptr<IntVariable> p_install_time;     // the installation time of the separator
    PipeConnection *p_outlet_connection;        // the outlet pipe that this separator feeds




public:
    Separator();
    Separator(const Separator &s);

    virtual ~Separator();

    // virtual functions

    virtual Pipe* clone() {return new Separator(*this);}

    virtual void calculateInletPressure();

    // set functions
    void setRemoveWater(bool b) {m_remove_water = b;}
    void setRemoveOil(bool b) {m_remove_oil = b;}
    void setRemoveGas(bool b) {m_remove_gas = b;}

    void setOutletConnection(PipeConnection *c) {p_outlet_connection = c;}
    void setCost(Cost *c) {p_cost = c;}
    void setInstallTime(shared_ptr<IntVariable> t) {p_install_time = t;}

    // get functions
    bool removeWater() const {return m_remove_water;}
    bool removeOil() const {return m_remove_oil;}
    bool removeGas() const {return m_remove_gas;}

    PipeConnection* outletConnection() {return p_outlet_connection;}
    Cost* cost() {return p_cost;}
    shared_ptr<IntVariable> installTime() {return p_install_time;}
};

} // namespace ResOpt

#endif // SEPARATOR_H
