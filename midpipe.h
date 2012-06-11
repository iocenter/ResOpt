#ifndef MIDPIPE_H
#define MIDPIPE_H

#include "pipe.h"
#include <tr1/memory>




using std::tr1::shared_ptr;

namespace ResOpt
{

class IntVariable;
class PipeConnection;
class Constraint;



class MidPipe : public Pipe
{
private:

    QVector<PipeConnection*> m_outlet_connections;
    shared_ptr<Constraint> p_connection_constraint;            // constraint that makes sure that the sum of flow to pipes = 1


public:
    MidPipe();
    ~MidPipe();

    // virtual functions

    virtual void calculateInletPressure();

    // overloaded functions
    void setNumber(int n);

    // misc functions

    void updateOutletConnectionConstraint();

    // add functions

    void addOutletConnection(PipeConnection *c) {m_outlet_connections.push_back(c);}


    // set functions


    // get functions


    int numberOfOutletConnections() {return m_outlet_connections.size();}
    PipeConnection* outletConnection(int i) {return m_outlet_connections.at(i);}


    /**
     * @brief Finds the fraction of the rates from this Pipe that go through upstream_pipe
     *
     * @param upstream_pipe
     * @param ok false if upstream_pipe is not connected to this Pipe
     * @return double
     */
    double flowFraction(Pipe *upstream_pipe, bool *ok = 0);

    shared_ptr<Constraint> outletConnectionConstraint() {return p_connection_constraint;}
};

} // namespace ResOpt


#endif // MIDPIPE_H
