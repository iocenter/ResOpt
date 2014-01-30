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


#include "inspectorheelvariable.h"

#include <QtWidgets/QLineEdit>
#include <QtWidgets/QLabel>
#include <QCheckBox>
#include <QtWidgets/QGridLayout>
#include <QDoubleValidator>

namespace ResOptGui
{


InspectorHeelVariable::InspectorHeelVariable(const QString &name, double value, double max, double min, bool locked, QWidget *parent, bool header):
    QWidget(parent),
    m_original_lock_state(locked)
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
        layout->addWidget(new QLabel("locked", this), row, 4, Qt::AlignLeft);


        row++;
    }

    // setting up the components

    p_name = new QLabel(name, this);
    p_name->setFixedWidth(150);
    layout->addWidget(p_name, row, 0);

    p_value = new QLineEdit(QString::number(value), this);
    p_value->setFixedWidth(80);
    p_value->setValidator(new QDoubleValidator(this));
    layout->addWidget(p_value, row, 1);

    p_max = new QLineEdit(QString::number(max), this);
    p_max->setFixedWidth(80);
    p_max->setValidator(new QDoubleValidator(this));
    layout->addWidget(p_max, row, 2);

    p_min = new QLineEdit(QString::number(min), this);
    p_min->setFixedWidth(80);
    p_min->setValidator(new QDoubleValidator(this));
    layout->addWidget(p_min, row, 3);

    p_locked = new QCheckBox(this);
    p_locked->setChecked(locked);
    p_locked->setFixedWidth(50);

    connect(p_locked, SIGNAL(clicked(bool)), this, SLOT(onLockChanged(bool)));

    layout->addWidget(p_locked, row, 4, Qt::AlignCenter);

    if(locked)
    {
        p_value->setDisabled(true);
        p_max->setDisabled(true);
        p_min->setDisabled(true);
    }


}

//-----------------------------------------------------------------------------------------------
// Returns the current value
//-----------------------------------------------------------------------------------------------
double InspectorHeelVariable::value()
{
    return p_value->text().replace(",",".").toDouble();
}

//-----------------------------------------------------------------------------------------------
// Returns the current max
//-----------------------------------------------------------------------------------------------
double InspectorHeelVariable::max()
{
    return p_max->text().replace(",",".").toDouble();
}

//-----------------------------------------------------------------------------------------------
// Returns the current min
//-----------------------------------------------------------------------------------------------
double InspectorHeelVariable::min()
{
    return p_min->text().replace(",",".").toDouble();
}


//-----------------------------------------------------------------------------------------------
// Returns if the var is locked to the toe
//-----------------------------------------------------------------------------------------------
bool InspectorHeelVariable::locked()
{
    return p_locked->isChecked();
}

//-----------------------------------------------------------------------------------------------
// Returns if the locked state has changed
//-----------------------------------------------------------------------------------------------
bool InspectorHeelVariable::lockStateChanged()
{
    return m_original_lock_state != locked();
}

//-----------------------------------------------------------------------------------------------
// slot for changed check state of p_locked
//-----------------------------------------------------------------------------------------------
void InspectorHeelVariable::onLockChanged(bool checked)
{
    p_value->setDisabled(checked);
    p_max->setDisabled(checked);
    p_min->setDisabled(checked);
}

} // namespace
