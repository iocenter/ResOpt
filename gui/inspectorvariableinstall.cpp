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


#include "inspectorvariableinstall.h"

#include <QtGui/QComboBox>
#include <QtGui/QLabel>
#include <QtGui/QGridLayout>
#include <QDoubleValidator>
#include <QtGui/QLineEdit>

namespace ResOptGui
{


InspectorVariableInstall::InspectorVariableInstall(int value, int max, int min, QVector<double> schedule, QWidget *parent, bool header):
    QWidget(parent)
{
    QGridLayout *layout = new QGridLayout(this);
    setLayout(layout);

    int row = 0;

    // creating header if specified
    if(header)
    {
        layout->addWidget(new QLabel("value", this), row, 1, Qt::AlignCenter);
        layout->addWidget(new QLabel("max", this), row, 2, Qt::AlignCenter);
        layout->addWidget(new QLabel("min", this), row, 3, Qt::AlignCenter);

        row++;
    }

    // setting up the components

    p_name = new QLabel("Install Time:", this);
    p_name->setFixedWidth(150);
    layout->addWidget(p_name, row, 0);

    // the current value
    p_value = new QComboBox(this);
    p_max = new QComboBox(this);
    p_min = new QComboBox(this);


    p_value->addItem("0 days", 0);
    p_max->addItem("0 days", 0);
    p_min->addItem("0 days", 0);

    for(int i = 0; i < schedule.size(); ++i)
    {
        QString s = QString::number(schedule.at(i)) + " days";

        p_value->addItem(s, i+1);
        p_max->addItem(s, i+1);
        p_min->addItem(s, i+1);
    }

    p_value->setCurrentIndex(value);
    p_max->setCurrentIndex(max);
    p_min->setCurrentIndex(min);

    p_value->setFixedWidth(100);
    p_max->setFixedWidth(100);
    p_min->setFixedWidth(100);

    layout->addWidget(p_value, row, 1);
    layout->addWidget(p_max, row, 2);
    layout->addWidget(p_min, row, 3);



}

//-----------------------------------------------------------------------------------------------
// Returns the current value
//-----------------------------------------------------------------------------------------------
int InspectorVariableInstall::value()
{
    return p_value->currentIndex();
}

//-----------------------------------------------------------------------------------------------
// Returns the current max
//-----------------------------------------------------------------------------------------------
int InspectorVariableInstall::max()
{
    return p_max->currentIndex();
}

//-----------------------------------------------------------------------------------------------
// Returns the current min
//-----------------------------------------------------------------------------------------------
int InspectorVariableInstall::min()
{
    return p_min->currentIndex();
}


} // namespace
