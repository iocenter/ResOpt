/*
 * This file is part of the ResOpt project.
 *
 * Copyright (C) 2011-2014 Aleksander O. Juell <aleksander.juell@ntnu.no>
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


#ifndef WELLPATH_H
#define WELLPATH_H

#include <tr1/memory>
#include <QVector>
#include <QPair>
#include <QString>

using std::tr1::shared_ptr;



namespace ResOpt
{


class IntVariable;


class WellPath
{
private:


    shared_ptr<IntVariable> p_toe_i;
    shared_ptr<IntVariable> p_toe_j;
    shared_ptr<IntVariable> p_toe_k;

    shared_ptr<IntVariable> p_heel_i;
    shared_ptr<IntVariable> p_heel_j;
    shared_ptr<IntVariable> p_heel_k;

    bool m_lock_heel_i;
    bool m_lock_heel_j;
    bool m_lock_heel_k;

    QVector<shared_ptr<IntVariable> > m_options_var;
    QVector<QPair<QString, double> > m_options_const;




public:
    WellPath();
    ~WellPath();
    WellPath(const WellPath &wp);

    // misc functions
    bool initialize();

    void updateConstantOption(int i, double value);

    // set functions
    void setToeI(shared_ptr<IntVariable> i) {p_toe_i = i;}
    void setToeJ(shared_ptr<IntVariable> j) {p_toe_j = j;}
    void setToeK(shared_ptr<IntVariable> k) {p_toe_k = k;}

    void setHeelI(shared_ptr<IntVariable> i) {p_heel_i = i;}
    void setHeelJ(shared_ptr<IntVariable> j) {p_heel_j = j;}
    void setHeelK(shared_ptr<IntVariable> k) {p_heel_k = k;}

    void setHeelILocked(bool b) {m_lock_heel_i = b;}
    void setHeelJLocked(bool b) {m_lock_heel_j = b;}
    void setHeelKLocked(bool b) {m_lock_heel_k = b;}

    // get functions

    QVector<shared_ptr<IntVariable> > variables();

    shared_ptr<IntVariable> toeI() {return p_toe_i;}
    shared_ptr<IntVariable> toeJ() {return p_toe_j;}
    shared_ptr<IntVariable> toeK() {return p_toe_k;}

    shared_ptr<IntVariable> heelI() {return p_heel_i;}
    shared_ptr<IntVariable> heelJ() {return p_heel_j;}
    shared_ptr<IntVariable> heelK() {return p_heel_k;}

    bool heelILocked() {return m_lock_heel_i;}
    bool heelJLocked() {return m_lock_heel_j;}
    bool heelKLocked() {return m_lock_heel_k;}

    int numberOfVariableOptions() {return m_options_var.size();}
    shared_ptr<IntVariable> variableOption(int i) {return m_options_var.at(i);}

    int numberOfConstantOptions() {return m_options_const.size();}
    QPair<QString, double> constantOption(int i) {return m_options_const.at(i);}



    //shared_ptr<IntVariable> iVariable() {return p_var_i;}
    //shared_ptr<IntVariable> jVariable() {return p_var_j;}

    // add functions
    void addOptionConstant(const QString &name, double value) {m_options_const.push_back(QPair<QString, double>(name, value));}
    void addOptionVariable(shared_ptr<IntVariable> v) {m_options_var.push_back(v);}

};


} // namespace

#endif // WELLPATH_H
