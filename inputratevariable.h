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


#ifndef INPUTRATEVARIABLE_H
#define INPUTRATEVARIABLE_H

#include <tr1/memory>

using std::tr1::shared_ptr;

namespace ResOpt
{

class Pipe;
class Stream;
class RealVariable;

/**
 * @brief Connects a set of three real variables to a Stream
 * @details This class is used by the DecoupledModel to connect variables to streams in the different parts of the model.
 *
 */
class InputRateVariable
{
private:
    Pipe *p_pipe;
    Stream *p_stream;
    shared_ptr<RealVariable> p_var_oil;
    shared_ptr<RealVariable> p_var_gas;
    shared_ptr<RealVariable> p_var_water;

public:
    InputRateVariable();


    // misc functions

    /**
     * @brief Updates the stream with the current values for the varaibles.
     *
     * @return bool
     */
    bool updateStream();

    // set functions
    void setPipe(Pipe *p) {p_pipe = p;}
    void setStream(Stream *s) {p_stream = s;}
    void setOilVariable(shared_ptr<RealVariable> v) {p_var_oil = v;}
    void setGasVariable(shared_ptr<RealVariable> v) {p_var_gas = v;}
    void setWaterVariable(shared_ptr<RealVariable> v) {p_var_water = v;}

    // get functions
    Pipe* pipe() {return p_pipe;}
    Stream* stream() {return p_stream;}
    shared_ptr<RealVariable> oilVariable() {return p_var_oil;}
    shared_ptr<RealVariable> gasVariable() {return p_var_gas;}
    shared_ptr<RealVariable> waterVariable() {return p_var_water;}
};

} // namespace ResOpt

#endif // INPUTRATEVARIABLE_H
