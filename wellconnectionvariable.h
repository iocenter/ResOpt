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


#ifndef WELLCONNECTIONVARIABLE_H
#define WELLCONNECTIONVARIABLE_H

#include <tr1/memory>

using std::tr1::shared_ptr;

#include "wellconnection.h"

namespace ResOpt
{


class IntVariable;


class WellConnectionVariable
{
private:
    WellConnection *p_well_con;
    shared_ptr<IntVariable> p_var_i;
    shared_ptr<IntVariable> p_var_j;

public:
    WellConnectionVariable();
    ~WellConnectionVariable();
    WellConnectionVariable(const WellConnectionVariable &wcv);


    // set functions
    void setIVariable(shared_ptr<IntVariable> i) {p_var_i = i;}
    void setJVariable(shared_ptr<IntVariable> j) {p_var_j = j;}
    void setK1(int k1) {p_well_con->setK1(k1);}
    void setK2(int k2) {p_well_con->setK2(k2);}
    void setWellIndex(double wi) {p_well_con->setWellIndex(wi);}

    // get functions
    WellConnection* wellConnection();
    shared_ptr<IntVariable> iVariable() {return p_var_i;}
    shared_ptr<IntVariable> jVariable() {return p_var_j;}
};


} // namespace

#endif // WELLCONNECTIONVARIABLE_H
