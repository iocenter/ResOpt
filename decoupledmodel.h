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


/**
 * @brief Model where input rates to all parts of the system are treated as variables.
 * @details In this type of Model there is no automatic link between the output of a upstream part and the input to a downstream part. The input rates to every pipe segment in the Model
 *          is included as a variable. Constraints are included to honour the mass balance in the system (c = q_in - q_out = 0).
 *
 */
class DecoupledModel : public Model
{
public:
    DecoupledModel();

    DecoupledModel(const DecoupledModel &m);

    // virtual functions
    virtual void updateStreams();
    virtual Model* clone() const {return new DecoupledModel(*this);}
};


} // namespace ResOpt

#endif // DECOUPLEDMODEL_H
