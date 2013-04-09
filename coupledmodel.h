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


#ifndef COUPLEDMODEL_H
#define COUPLEDMODEL_H

#include "model.h"

#include "realvariable.h"
#include "binaryvariable.h"
#include "intvariable.h"
#include "constraint.h"

namespace ResOpt
{
class ProductionWell;
class MidPipe;
class Separator;
class PressureBooster;


/**
 * @brief A Model where mass balance is allways feasible.
 * @details In a coupled model, the output streams from an upstream part of the project is transfered as input streams to the downstream parts of the project.
 *          This ensures that the mass balance is always honoured.
 *
 */
class CoupledModel : public Model
{
private:

    QVector<shared_ptr<BinaryVariable> > m_vars_binary;     // vector containing all binary variables
    QVector<shared_ptr<RealVariable> > m_vars_real;         // vector containing all real variables
    QVector<shared_ptr<IntVariable> > m_vars_integer;       // vector containing all integer variables
    QVector<shared_ptr<Constraint> > m_cons;                // vector containing all the constraints

    /**
    * @brief Adds the streams flowing from this well to the upstream connected pipes
    *
    * @param w
    */
    void addStreamsUpstream(ProductionWell *w);

    /**
    * @brief Adds the streams flowing from this pipe to the upstream connected pipes (or separators)
    *
    * @param p
     */
    void addStreamsUpstream(MidPipe *p, Well *from_well, double flow_frac);


    /**
     * @brief Adds the streams flowing from this separator to the upstream connected pipe
     *
     * @param s
     */
    void addStreamsUpstream(Separator *s, Well *from_well, double flow_frac);

    /**
     * @brief Adds the streams flowing from this booster to the upstream connected pipe
     *
     * @param b
     */
    void addStreamsUpstream(PressureBooster *b, Well *from_well, double flow_frac);



public:
    CoupledModel();
    CoupledModel(const CoupledModel &m);

    virtual ~CoupledModel();

    // virtual functions
    virtual Model* clone() const {return new CoupledModel(*this);}

    virtual void initialize();
    virtual void process();

    virtual void updateStreams();
    virtual bool updateConstraints();

    virtual QVector<shared_ptr<BinaryVariable> >& binaryVariables(bool force_refresh = false);
    virtual QVector<shared_ptr<RealVariable> >& realVariables(bool force_refresh = false);
    virtual QVector<shared_ptr<IntVariable> >& integerVariables(bool force_refresh = false);
    virtual QVector<shared_ptr<Constraint> >& constraints(bool force_refresh = false);

    virtual QVector<shared_ptr<RealVariable> > realVariables(Component *c);

    virtual int numberOfRealVariables() const {return m_vars_real.size();}
    virtual int numberOfBinaryVariables() const {return m_vars_binary.size();}
    virtual int numberOfIntegerVariables() const {return m_vars_integer.size();}
    virtual int numberOfConstraints() const {return m_cons.size();}

    virtual double realVariableValue(int i) const {return m_vars_real.at(i)->value();}
    virtual double binaryVariableValue(int i) const {return m_vars_binary.at(i)->value();}
    virtual double integerVariableValue(int i) const {return m_vars_integer.at(i)->value();}
    virtual double constraintValue(int i) const {return m_cons.at(i)->value();}

};


} // namespace ResOpt

#endif // COUPLEDMODEL_H
