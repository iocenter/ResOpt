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


#include "inspectorcapacity.h"
#include "plotstreams.h"
#include "inspectorconstraint.h"

#include "capacity.h"
#include "constraint.h"

using ResOpt::Constraint;

#include <QtGui/QGridLayout>
#include <QDoubleValidator>
#include <QtGui/QGroupBox>
#include <tr1/memory>

using std::tr1::shared_ptr;



namespace ResOptGui
{

InspectorCapacity::InspectorCapacity(Capacity *cap, QWidget *parent) :
    QWidget(parent),
    p_capacity(cap),
    m_lbl_oil("Oil Capacity", this),
    m_chk_oil(this),
    m_led_oil(this),
    m_lbl_gas("Gas Capacity", this),
    m_chk_gas(this),
    m_led_gas(this),
    m_lbl_water("Water Capacity", this),
    m_chk_water(this),
    m_led_water(this),
    m_lbl_liquid("Liquid Capacity", this),
    m_chk_liquid(this),
    m_led_liquid(this),
    m_btn_close("Close", this),
    m_btn_ok("Ok", this),
    m_changed(false)
{

    setAttribute(Qt::WA_DeleteOnClose);


    construct();

    show();
}


//-----------------------------------------------------------------------------------------------
// constructs the view
//-----------------------------------------------------------------------------------------------
void InspectorCapacity::construct()
{
    setWindowTitle("Capacity " + p_capacity->name() + " Properties");

    QGridLayout *layout = new QGridLayout(this);

    setLayout(layout);


    // oil
    layout->addWidget(&m_lbl_oil, 0, 0);
    layout->addWidget(&m_chk_oil, 0, 1);
    layout->addWidget(&m_led_oil, 0, 2);

    connect(&m_chk_oil, SIGNAL(clicked(bool)), &m_led_oil, SLOT(setEnabled(bool)));

    if(p_capacity->maxOil() >= 0) m_led_oil.setText(QString::number(p_capacity->maxOil()));
    else m_led_oil.setDisabled(true);
    m_chk_oil.setChecked(p_capacity->maxOil() >= 0);
    m_led_oil.setValidator(new QDoubleValidator(this));


    // gas
    layout->addWidget(&m_lbl_gas, 1, 0);
    layout->addWidget(&m_chk_gas, 1, 1);
    layout->addWidget(&m_led_gas, 1, 2);

    connect(&m_chk_gas, SIGNAL(clicked(bool)), &m_led_gas, SLOT(setEnabled(bool)));

    if(p_capacity->maxGas() >= 0) m_led_gas.setText(QString::number(p_capacity->maxGas()));
    else m_led_gas.setDisabled(true);
    m_chk_gas.setChecked(p_capacity->maxGas() >= 0);
    m_led_gas.setValidator(new QDoubleValidator(this));

    // water
    layout->addWidget(&m_lbl_water, 2, 0);
    layout->addWidget(&m_chk_water, 2, 1);
    layout->addWidget(&m_led_water, 2, 2);

    connect(&m_chk_water, SIGNAL(clicked(bool)), &m_led_water, SLOT(setEnabled(bool)));

    if(p_capacity->maxWater() >= 0) m_led_water.setText(QString::number(p_capacity->maxWater()));
    else m_led_water.setDisabled(true);
    m_chk_water.setChecked(p_capacity->maxWater() >= 0);
    m_led_water.setValidator(new QDoubleValidator(this));

    // liquid
    layout->addWidget(&m_lbl_liquid, 3, 0);
    layout->addWidget(&m_chk_liquid, 3, 1);
    layout->addWidget(&m_led_liquid, 3, 2);

    connect(&m_chk_liquid, SIGNAL(clicked(bool)), &m_led_liquid, SLOT(setEnabled(bool)));

    if(p_capacity->maxLiquid() >= 0) m_led_liquid.setText(QString::number(p_capacity->maxLiquid()));
    else m_led_liquid.setDisabled(true);
    m_chk_liquid.setChecked(p_capacity->maxLiquid() >= 0);
    m_led_liquid.setValidator(new QDoubleValidator(this));


    int current_row = 4;

    // water constraint values
    if(p_capacity->waterConstraints().size() != 0)
    {

        QGroupBox *box_wat_con = new QGroupBox("Water Constraints", this);
        box_wat_con->setStyleSheet("QGroupBox{border:2px solid gray;border-radius:5px;margin-top: 1ex;} QGroupBox::title{subcontrol-origin: margin;subcontrol-position:top center;padding:0 3px;}");

        QVBoxLayout *layout_wat_con = new QVBoxLayout(box_wat_con);
        box_wat_con->setLayout(layout_wat_con);

        int i = 0;
        foreach (shared_ptr<Constraint> c, p_capacity->waterConstraints())
        {

          InspectorConstraint *ic = new InspectorConstraint(p_capacity->schedule().at(i), c->value(), c->max(), c->min(), this, i == 0);
          ++i;
          layout_wat_con->addWidget(ic);
        }


        layout->addWidget(box_wat_con, current_row++, 0, 1, 3);


    }

    // setting up the buttons
    layout->addWidget(&m_btn_ok, current_row, 0);
    connect(&m_btn_ok, SIGNAL(clicked()), this, SLOT(saveAndClose()));


    layout->addWidget(&m_btn_close, current_row, 2);
    connect(&m_btn_close, SIGNAL(clicked()), this, SLOT(close()));


    // connecting signals to setChanged




}


//-----------------------------------------------------------------------------------------------
// Saves the current values to the model, and closes the window
//-----------------------------------------------------------------------------------------------
void InspectorCapacity::saveAndClose()
{

    emit sendMsg("Saving variable values for Capacity " + p_capacity->name() + " to model...");

    // saving
    if(m_chk_oil.isChecked()) p_capacity->setMaxOil(m_led_oil.text().toDouble());
    else p_capacity->setMaxOil(-1);

    if(m_chk_gas.isChecked()) p_capacity->setMaxGas(m_led_gas.text().toDouble());
    else p_capacity->setMaxGas(-1);

    if(m_chk_water.isChecked()) p_capacity->setMaxWater(m_led_water.text().toDouble());
    else p_capacity->setMaxWater(-1);

    if(m_chk_liquid.isChecked()) p_capacity->setMaxLiquid(m_led_liquid.text().toDouble());
    else p_capacity->setMaxLiquid(-1);

    // updating the constraints in the capacity
    p_capacity->setupConstraints(p_capacity->schedule());



    close();
}


//-----------------------------------------------------------------------------------------------
// sets the state of the inspector to changed
//-----------------------------------------------------------------------------------------------
void InspectorCapacity::setChanged()
{
    m_changed = true;
}


} // namespace
