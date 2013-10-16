/*
 * This file is part of the ResOpt project.
 *
 * Copyright (C) 2011-2013 Aleksander O. Juell <aleksander.juell@ntnu.no>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
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


#ifndef ADJOINTSCOUPLEDMODEL_H
#define ADJOINTSCOUPLEDMODEL_H

#include "coupledmodel.h"

#include <QVector>

namespace ResOpt
{

class AdjointCollection;
class Adjoint;
class Stream;
class Case;

class AdjointsCoupledModel : public CoupledModel
{
private:

    double m_perturbation;
    Case *p_results;

    QVector<AdjointCollection*> m_adjoint_collections;


    Case* processPerturbation(shared_ptr<RealVariable> v);
    Case* processBaseCase();


public:
    AdjointsCoupledModel();
    AdjointsCoupledModel(const AdjointsCoupledModel &m);
    ~AdjointsCoupledModel();

    // virtual functions
    virtual Model* clone() const {return new AdjointsCoupledModel(*this);}
    virtual void initialize();
    virtual void process();

/*
    virtual QVector<shared_ptr<BinaryVariable> >& binaryVariables(bool force_refresh = false) {return CoupledModel::binaryVariables(force_refresh);}
    virtual QVector<shared_ptr<RealVariable> >& realVariables(bool force_refresh = false) {return CoupledModel::realVariables(force_refresh);}
    virtual QVector<shared_ptr<IntVariable> >& integerVariables(bool force_refresh = false) {return CoupledModel::integerVariables(force_refresh);}
    virtual QVector<shared_ptr<Constraint> >& constraints(bool force_refresh = false) {return CoupledModel::constraints(force_refresh);}

    virtual QVector<shared_ptr<RealVariable> > realVariables(Component *c) {return CoupledModel::realVariables(c);}

    virtual int numberOfRealVariables() const {CoupledModel::numberOfRealVariables();}
    virtual int numberOfBinaryVariables() const {return CoupledModel::numberOfBinaryVariables();}
    virtual int numberOfIntegerVariables() const {return CoupledModel::numberOfIntegerVariables();}
    virtual int numberOfConstraints() const {return CoupledModel::numberOfConstraints();}

    virtual double realVariableValue(int i) const {return CoupledModel::realVariableValue(i);}
    virtual double binaryVariableValue(int i) const {return CoupledModel::binaryVariableValue(i);}
    virtual double integerVariableValue(int i) const {return CoupledModel::integerVariableValue(i);}
    virtual double constraintValue(int i) const {return CoupledModel::constraintValue(i);}

    */
    // misc functions

    /**
     * @brief Returns the Adjoints asociated with variable v
     *
     * @param shared_ptr<RealVariable>
     * @return Adjoint
     */
    AdjointCollection* adjointCollection(shared_ptr<RealVariable> v);



    // set functions
    void setPerturbationSize(double eps) {m_perturbation = eps;}

    // get functions
    double perturbationSize() {return m_perturbation;}
    Case* results() {return p_results;}




};

} // namespace

#endif // ADJOINTSCOUPLEDMODEL_H
