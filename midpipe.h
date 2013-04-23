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



/**
 * @brief A pipe that both have input and output connections.
 *
 */
class MidPipe : public Pipe
{
private:

    QVector<PipeConnection*> m_outlet_connections;
    shared_ptr<Constraint> p_connection_constraint;            // constraint that makes sure that the sum of flow to pipes = 1


public:
    MidPipe();
    MidPipe(const MidPipe &p);

    virtual ~MidPipe();

    // virtual functions

    virtual Pipe* clone() {return new MidPipe(*this);}

    virtual void calculateInletPressure();

    virtual QString description() const;

    // overloaded functions
    void setNumber(int n);

    // misc functions

    void updateOutletConnectionConstraint();


    /**
     * @brief Checks if any of the outlet connections are MidPipe.
     *
     * @return bool
     */
    bool midpipeConnectedUpstream();

    // add functions

    void addOutletConnection(PipeConnection *c) {m_outlet_connections.push_back(c);}


    // set functions


    // get functions


    int numberOfOutletConnections() const {return m_outlet_connections.size();}
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
