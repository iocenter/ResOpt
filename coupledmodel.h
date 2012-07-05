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

namespace ResOpt
{
class ProductionWell;
class MidPipe;
class Separator;


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
    void addStreamsUpstream(MidPipe *p);


    /**
     * @brief Adds the streams flowing from this separator to the upstream connected pipe
     *
     * @param s
     */
    void addStreamsUpstream(Separator *s);


public:
    CoupledModel();
    CoupledModel(const CoupledModel &m);

    virtual ~CoupledModel();

    // virtual functions
    virtual Model* clone() const {return new CoupledModel(*this);}

    virtual void initialize();
    virtual void updateStreams();
    virtual bool updateConstraints();

    virtual QVector<shared_ptr<BinaryVariable> >& binaryVariables();
    virtual QVector<shared_ptr<RealVariable> >& realVariables();
    virtual QVector<shared_ptr<Constraint> >& constraints();
};


} // namespace ResOpt

#endif // COUPLEDMODEL_H
