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


#ifndef BINARYVARIABLE_H
#define BINARYVARIABLE_H

#include "variable.h"

namespace ResOpt
{

class BinaryVariable : public Variable
{
private:
    double m_value;
    bool m_is_variable;

public:
    BinaryVariable();

    virtual bool isVariable() {return m_is_variable;}

    // set functions
    void setValue(double v) {m_value = v;}
    void setIsVariable(bool b) {m_is_variable = b;}

    // get functions
    double max() {return 1.0;}
    double min() {return 0.0;}
    double value() {return m_value;}
};

} // namespace ResOpt


#endif // BINARYVARIABLE_H
