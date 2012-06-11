#ifndef PRODUCTIONWELL_H
#define PRODUCTIONWELL_H

#include "well.h"

#include <tr1/memory>

using std::tr1::shared_ptr;


namespace ResOpt
{

class IntVariable;
class BinaryVariable;
class PipeConnection;
class Constraint;
class Pipe;


class ProductionWell : public Well
{
private:
    shared_ptr<Constraint> p_bhp_contraint;                    // constraint used to check if the bhp pressure of the well is feasible

    QVector<PipeConnection*> m_pipe_connections;    // vector of all pipes that could be connected to the well
    shared_ptr<Constraint> p_connection_constraint;            // constraint that makes sure that the sum of flow to pipes = 1



public:
    ProductionWell();
    ~ProductionWell();

    // overloaded functions
    void setName(const QString &n);

    /**
     * @brief Checks if the inlet pressure of the connected pipe is at any time higher than the wells BHP
     * @details If the inlet pressure of the pipe is higher than the producing BHP, this is not a feasible solution,
     *          and the constraint is set out of bounds.
     *
     */
    void updateBhpConstraint();

    void updatePipeConnectionConstraint();


    /**
     * @brief Finds the fraction of the rates from this Well that flows through the pipe p
     *
     * @param p
     * @param ok false if the Pipe is not connected to this Well
     * @return double
     */
    double flowFraction(Pipe *p, bool *ok = 0);

    // virtual functions

    virtual void setAutomaticType() {setType(Well::PROD);}

    // add functions

    void addPipeConnection(PipeConnection *c) {m_pipe_connections.push_back(c);}

    // set functions

    //get functions


    /**
     * @brief Returns the Constraint on the BHP
     *
     * @return Constraint
     */
    shared_ptr<Constraint> bhpConstraint() {return p_bhp_contraint;}

    shared_ptr<Constraint> pipeConnectionConstraint() {return p_connection_constraint;}

    int numberOfPipeConnections() {return m_pipe_connections.size();}
    PipeConnection* pipeConnection(int i) {return m_pipe_connections.at(i);}



};

} // namespace ResOpt

#endif // PRODUCTIONWELL_H
