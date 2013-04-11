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


#include "inspectorconstraint.h"

#include <QtGui/QLabel>
#include <QtGui/QGridLayout>

namespace ResOptGui
{


InspectorConstraint::InspectorConstraint(double time, double value, double max, double min, QWidget *parent, bool header):
    QWidget(parent)
{
    QGridLayout *layout = new QGridLayout(this);
    setLayout(layout);

    int row = 0;

    // creating header if specified
    if(header)
    {
        layout->addWidget(new QLabel("time", this), row, 0, Qt::AlignCenter);
        layout->addWidget(new QLabel("min", this), row, 1, Qt::AlignCenter);
        layout->addWidget(new QLabel("value", this), row, 2, Qt::AlignCenter);
        layout->addWidget(new QLabel("max", this), row, 3, Qt::AlignCenter);

        row++;
    }

    // setting up the components

    p_time = new QLabel(QString::number(time), this);
   // p_time->setFixedWidth(100);
    layout->addWidget(p_time, row, 0, Qt::AlignCenter);

    p_min = new QLabel(QString::number(min), this);
   // p_max->setFixedWidth(100);
    layout->addWidget(p_min, row, 1, Qt::AlignCenter);


    p_value = new QLabel(this);
    p_value->setTextFormat(Qt::RichText);

    if(value < min || value > max) p_value->setText("<b><font color = red>" + QString::number(value) + "</font></b>");
    else p_value->setText("<b>" + QString::number(value) + "</b>");

   // p_value->setFixedWidth(100);
    layout->addWidget(p_value, row, 2, Qt::AlignCenter);


    p_max = new QLabel(QString::number(max), this);
   // p_max->setFixedWidth(100);
    layout->addWidget(p_max, row, 3, Qt::AlignCenter);





}


} // namespace
