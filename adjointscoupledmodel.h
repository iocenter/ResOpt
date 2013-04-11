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

class AdjointsCoupledModel : public CoupledModel
{
private:

    // indicating the level of adjoints that should be calculated
    // 1 = one well one timestep
    // 2 = one well every timestep
    // 3 = every well every timestep
    int m_adjoint_level;

    QVector<AdjointCollection*> m_adjoint_collections;

public:
    AdjointsCoupledModel();

    // virtual functions
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


    /**
     * @brief Returns the adjoint for Stream s wrt. variable v.
     *
     * @param v
     * @param s
     * @return Adjoint
     */
    Adjoint* adjoint(shared_ptr<RealVariable> v, Stream *s);


    // set functions
    void setAdjointLevel(int i) {m_adjoint_level = i;}

    // get functions
    int adjointLevel() {return m_adjoint_level;}



};

} // namespace

#endif // ADJOINTSCOUPLEDMODEL_H
