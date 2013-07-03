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


#include "inspectorwellconnectionvariable.h"

#include "inspectorvariable.h"

#include <QtWidgets/QLineEdit>
#include <QtWidgets/QLabel>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QComboBox>
#include <QDoubleValidator>



namespace ResOptGui
{


InspectorWellConnectionVariable::InspectorWellConnectionVariable(int i, int i_max, int i_min, int j, int j_max, int j_min, int k1, int k2, int wi, QWidget *parent) :
    QWidget(parent)
{
    QGridLayout *layout = new QGridLayout(this);
    setLayout(layout);

    // setting up the i variable
    p_var_i = new InspectorVariable("i-direction: ", i, i_max, i_min, this, true);
    layout->addWidget(p_var_i, 0, 0);

    // setting up the j variable
    p_var_j = new InspectorVariable("j-direction: ", j, j_max, j_min, this, false);
    layout->addWidget(p_var_j, 1, 0);


}

//-----------------------------------------------------------------------------------------------
// Returns the current value
//-----------------------------------------------------------------------------------------------




} // namespace
