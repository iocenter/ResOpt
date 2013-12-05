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


#ifndef ADJOINTCOLLECTION_H
#define ADJOINTCOLLECTION_H

#include <QVector>
#include <tr1/memory>

using std::tr1::shared_ptr;

namespace ResOpt
{

class Adjoint;
class RealVariable;
class Well;
class Stream;

/**
 * @brief A collection of all the Adjoints for a given variable
 */
class AdjointCollection
{
private:
    QVector<Adjoint*> m_adjoints;
    shared_ptr<RealVariable> p_var;

public:
    AdjointCollection();
    AdjointCollection(const AdjointCollection &ac);

    // misc functions
    /**
     * @brief Perturbs all Streams asociated with this adjoint collection.
     * @details Perturbs all Streams asociated with variable(). See Adjoint::perturbStream(double).
     * @param eps_x The size of the perturbation
     * @return
     */
    bool perturbStreams(double eps_x);


    // add functions
    void addAdjoint(Adjoint *a) {m_adjoints.push_back(a);}

    // set functions
    void setVariable(shared_ptr<RealVariable> v) {p_var = v;}

    // get functions
    shared_ptr<RealVariable> variable() {return p_var;}
    Adjoint* adjoint(int i) {return m_adjoints.at(i);}
    /**
     * @brief Returns the Adjoint for the Stream from a given Well at the specified timestep.
     * @param w
     * @param time
     * @return The adjoints for the Stream from Well w at timestep time, with respect to the variable variable()
     */
    Adjoint* adjoint(Well *w, int time);

    /**
     * @brief Returns the numer of adjoints in this collection.
     * @return
     */
    int numberOfAdjoints() {return m_adjoints.size();}


};


} // namespace

#endif // ADJOINTCOLLECTION_H
