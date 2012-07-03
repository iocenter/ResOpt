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


#ifndef PIPECONNECTION_H
#define PIPECONNECTION_H

#include <tr1/memory>


using std::tr1::shared_ptr;

namespace ResOpt
{
class Pipe;
class BinaryVariable;



/**
 * @brief Defines the connection from a Well or Pipe to a Pipe.
 *
 */
class PipeConnection
{
private:
    int m_pipe_number;
    Pipe *p_pipe;
    shared_ptr<BinaryVariable> p_routing_variable;


public:
    PipeConnection();
    PipeConnection(const PipeConnection &p);
    ~PipeConnection();

    // set functions
    void setPipeNumber(int i) {m_pipe_number = i;}
    void setPipe(Pipe *p) {p_pipe = p;}
    void setVariable(shared_ptr<BinaryVariable> v) {p_routing_variable = v;}

    // get functions
    int pipeNumber() const {return m_pipe_number;}
    Pipe* pipe() {return p_pipe;}
    shared_ptr<BinaryVariable> variable() {return p_routing_variable;}

};

} // namespace ResOpt

#endif // PIPECONNECTION_H
