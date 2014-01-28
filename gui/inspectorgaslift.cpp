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


#include "inspectorgaslift.h"


#include <QtWidgets/QLineEdit>
#include <QtWidgets/QLabel>
#include <QtWidgets/QGridLayout>
#include <QDoubleValidator>

#include <QSpacerItem>

namespace ResOptGui
{


InspectorGasLift::InspectorGasLift(double time, double value, double max, double min, QWidget *parent, bool header):
    QWidget(parent)
{
    QGridLayout *layout = new QGridLayout(this);
    setLayout(layout);

    int row = 0;

    // creating header if specified
    if(header)
    {
        QLabel *time_header = new QLabel("time", this);
        time_header->setMinimumWidth(80);
        layout->addWidget(time_header, row, 0, Qt::AlignCenter);
        layout->addWidget(new QLabel("value", this), row, 1, Qt::AlignCenter);
        layout->addWidget(new QLabel("max", this), row, 2, Qt::AlignCenter);
        layout->addWidget(new QLabel("min", this), row, 3, Qt::AlignCenter);

        layout->setColumnMinimumWidth(4, 60);


        row++;
    }

    // setting up the components

    p_time = new QLabel(QString::number(time), this);
    p_time->setMinimumWidth(80);
    layout->addWidget(p_time, row, 0, Qt::AlignCenter);

    p_value = new QLineEdit(QString::number(value), this);
    p_value->setFixedWidth(100);
    p_value->setValidator(new QDoubleValidator(this));
    layout->addWidget(p_value, row, 1);

    p_max = new QLineEdit(QString::number(max), this);
    p_max->setFixedWidth(100);
    p_max->setValidator(new QDoubleValidator(this));
    layout->addWidget(p_max, row, 2);

    p_min = new QLineEdit(QString::number(min), this);
    p_min->setFixedWidth(100);
    p_min->setValidator(new QDoubleValidator(this));
    layout->addWidget(p_min, row, 3);



}

//-----------------------------------------------------------------------------------------------
// Returns the current value
//-----------------------------------------------------------------------------------------------
double InspectorGasLift::value()
{
    return p_value->text().toDouble();
}

//-----------------------------------------------------------------------------------------------
// Returns the current max
//-----------------------------------------------------------------------------------------------
double InspectorGasLift::max()
{
    return p_max->text().toDouble();
}

//-----------------------------------------------------------------------------------------------
// Returns the current min
//-----------------------------------------------------------------------------------------------
double InspectorGasLift::min()
{
    return p_min->text().toDouble();
}



} // namespace
