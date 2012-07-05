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


#ifndef DECOUPLEDMODEL_H
#define DECOUPLEDMODEL_H

#include "model.h"

namespace ResOpt
{

class InputRateVariable;
class MaterialBalanceConstraint;
class ProductionWell;
class MidPipe;
class Separator;
class Stream;


/**
 * @brief Model where input rates to all parts of the system are treated as variables.
 * @details In this type of Model there is no automatic link between the output of a upstream part and the input to a downstream part. The input rates to every pipe segment in the Model
 *          is included as a variable. Constraints are included to honour the mass balance in the system (c = q_in - q_out = 0).
 *
 */
class DecoupledModel : public Model
{
private:
    QVector<shared_ptr<BinaryVariable> > m_vars_binary;     // vector containing all binary variables
    QVector<shared_ptr<RealVariable> > m_vars_real;         // vector containing all real variables
    QVector<shared_ptr<Constraint> > m_cons;                // vector containing all the constraints

    QVector<InputRateVariable*> m_rate_vars;            // all the varaibles for rate input to the different parts of the model
    QVector<MaterialBalanceConstraint*> m_mb_cons;      // constraints associated with the input rate variables for mass balance feasibility

    void initializeVarsAndCons();

    /**
    * @brief Adds the streams flowing from this well to the streams in the asociated material balance constraints
    *
    * @param w
    */
    void addToMaterialBalanceStreamsUpstream(ProductionWell *w);

    /**
    * @brief Adds the streams flowing from this pipe to the streams in the asociated material balance constraints
    *
    * @param p
     */
    void addToMaterialBalanceStreamsUpstream(MidPipe *p);


    /**
     * @brief Adds the streams flowing from this separator to the streams in the asociated material balance constraints
     *
     * @param s
     */
    void addToMaterialBalanceStreamsUpstream(Separator *s);


    /**
     * @brief Updates all the streams in the material balance constraints
     *
     */
    void updateMaterialBalanceStreams();

    MaterialBalanceConstraint* find(Stream *s);


public:
    DecoupledModel();

    DecoupledModel(const DecoupledModel &m);

    virtual ~DecoupledModel();

    // virtual functions
    virtual Model* clone() const {return new DecoupledModel(*this);}

    virtual void initialize();
    virtual void updateStreams();
    virtual bool updateConstraints();

    virtual QVector<shared_ptr<BinaryVariable> >& binaryVariables();
    virtual QVector<shared_ptr<RealVariable> >& realVariables();
    virtual QVector<shared_ptr<Constraint> >& constraints();
};


} // namespace ResOpt

#endif // DECOUPLEDMODEL_H
