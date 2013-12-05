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

/**
 * @brief Model that uses adjoints from a reservoir simulator to obtain derivatives.
 * @details This model type is at the moment only used in combination with MRST. Adjoints are read from MRST, and used to calculated numerical derivatives for the production network.
 */
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
