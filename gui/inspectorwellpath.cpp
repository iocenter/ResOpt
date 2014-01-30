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


#include "inspectorwellpath.h"
#include "inspectorvariable.h"
#include "inspectorheelvariable.h"

#include "intvariable.h"

#include "well.h"

#include <QLineEdit>
#include <QLabel>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QPushButton>


using ResOpt::IntVariable;
using ResOpt::Well;

namespace ResOptGui
{



InspectorWellPath::InspectorWellPath(WellPath *wp, QWidget *parent) :
    QWidget(parent),
    p_wellpath(wp)
{

    QVBoxLayout *layout = new QVBoxLayout(this);



    // toe
    box_toe = new QGroupBox("Toe", this);
    box_toe->setStyleSheet("QGroupBox{border:1px solid gray;border-radius:5px;margin-top: 1ex;} QGroupBox::title{subcontrol-origin: margin;subcontrol-position:top center;padding:0 3px;}");
    //box_toe->setFixedWidth(500);
    QVBoxLayout *layout_toe = new QVBoxLayout(box_toe);

    box_toe->setLayout(layout_toe);


    toe_i = new InspectorVariable("I", p_wellpath->toeI()->value(), p_wellpath->toeI()->max(), p_wellpath->toeI()->min(),box_toe, true);
    layout_toe->addWidget(toe_i);

    toe_j = new InspectorVariable("J", p_wellpath->toeJ()->value(), p_wellpath->toeJ()->max(), p_wellpath->toeJ()->min(),box_toe);
    layout_toe->addWidget(toe_j);

    toe_k = new InspectorVariable("K", p_wellpath->toeK()->value(), p_wellpath->toeK()->max(), p_wellpath->toeK()->min(),box_toe);
    layout_toe->addWidget(toe_k);


    layout->addWidget(box_toe);

    // heel
    box_heel = new QGroupBox("Heel", this);
    box_heel->setStyleSheet("QGroupBox{border:1px solid gray;border-radius:5px;margin-top: 1ex;} QGroupBox::title{subcontrol-origin: margin;subcontrol-position:top center;padding:0 3px;}");
    //box_heel->setFixedWidth(500);
    QVBoxLayout *layout_heel = new QVBoxLayout(box_heel);

    box_heel->setLayout(layout_heel);


    heel_i = new InspectorHeelVariable("I", p_wellpath->heelI()->value(), p_wellpath->heelI()->max(), p_wellpath->heelI()->min(), p_wellpath->heelILocked(), box_toe, true);
    layout_heel->addWidget(heel_i);

    heel_j = new InspectorHeelVariable("J", p_wellpath->heelJ()->value(), p_wellpath->heelJ()->max(), p_wellpath->heelJ()->min(), p_wellpath->heelJLocked(), box_toe);
    layout_heel->addWidget(heel_j);

    heel_k = new InspectorHeelVariable("K", p_wellpath->heelK()->value(), p_wellpath->heelK()->max(), p_wellpath->heelK()->min(), p_wellpath->heelKLocked(), box_toe);
    layout_heel->addWidget(heel_k);


    layout->addWidget(box_heel);

    // variable options
    box_opt_var = new QGroupBox("Optional Variables", this);
    box_opt_var->setStyleSheet("QGroupBox{border:1px solid gray;border-radius:5px;margin-top: 1ex;} QGroupBox::title{subcontrol-origin: margin;subcontrol-position:top center;padding:0 3px;}");
    //box_heel->setFixedWidth(500);
    QVBoxLayout *layout_opt_var = new QVBoxLayout(box_opt_var);

    box_opt_var->setLayout(layout_opt_var);

    for(int i = 0; i < p_wellpath->numberOfVariableOptions(); ++i)
    {
        shared_ptr<IntVariable> var = p_wellpath->variableOption(i);

        QString name = var->name().split(" ").at(1);

        InspectorVariable *opt_var = new InspectorVariable(name, var->value(), var->max(), var->min(), box_opt_var, i == 0);

        m_opt_var.push_back(opt_var);
        layout_opt_var->addWidget(opt_var);
    }




    layout->addWidget(box_opt_var);


    // constant options
    box_opt_con = new QGroupBox("Optional Constants", this);
    box_opt_con->setStyleSheet("QGroupBox{border:1px solid gray;border-radius:5px;margin-top: 1ex;} QGroupBox::title{subcontrol-origin: margin;subcontrol-position:top center;padding:0 3px;}");
    //box_heel->setFixedWidth(500);
    QVBoxLayout *layout_opt_con = new QVBoxLayout(box_opt_con);

    box_opt_con->setLayout(layout_opt_con);

    for(int i = 0; i < p_wellpath->numberOfConstantOptions(); ++i)
    {
        QPair<QString, double> con = p_wellpath->constantOption(i);

        InspectorConstant *opt_con = new InspectorConstant(con.first, con.second, box_opt_con, i == 0);

        m_opt_con.push_back(opt_con);
        layout_opt_con->addWidget(opt_con);
    }




    layout->addWidget(box_opt_con);





    setLayout(layout);

}

//-----------------------------------------------------------------------------------------------
// Saves the GUI values to the WellPath
//-----------------------------------------------------------------------------------------------
void InspectorWellPath::save()
{
    // toe
    p_wellpath->toeI()->setValue(toe_i->value());
    p_wellpath->toeJ()->setValue(toe_j->value());
    p_wellpath->toeK()->setValue(toe_k->value());


    // heel i
    if(heel_i->lockStateChanged())
    {
        Well *w = dynamic_cast<Well*>(p_wellpath->toeI()->parent());

        if(heel_i->locked())
        {
            p_wellpath->setHeelI(p_wellpath->toeI());
            p_wellpath->setHeelILocked(true);
        }
        else
        {
            shared_ptr<IntVariable> new_var(new IntVariable(p_wellpath->toeI()->parent()));
            new_var->setValue(heel_i->value());
            new_var->setMax(heel_i->max());
            new_var->setMin(heel_i->min());
            new_var->setName("WELLPATH HEEL I for well: " + w->name());

            p_wellpath->setHeelI(new_var);
            p_wellpath->setHeelILocked(false);
        }
    }
    else if(!heel_i->locked())
    {
        p_wellpath->heelI()->setValue(heel_i->value());
    }

    // heel j
    if(heel_j->lockStateChanged())
    {
        Well *w = dynamic_cast<Well*>(p_wellpath->toeJ()->parent());

        if(heel_j->locked())
        {
            p_wellpath->setHeelJ(p_wellpath->toeJ());
            p_wellpath->setHeelJLocked(true);
        }
        else
        {
            shared_ptr<IntVariable> new_var(new IntVariable(p_wellpath->toeJ()->parent()));
            new_var->setValue(heel_j->value());
            new_var->setMax(heel_j->max());
            new_var->setMin(heel_j->min());
            new_var->setName("WELLPATH HEEL J for well: " + w->name());

            p_wellpath->setHeelJ(new_var);
            p_wellpath->setHeelJLocked(false);
        }
    }
    else if(!heel_j->locked())
    {
        p_wellpath->heelJ()->setValue(heel_j->value());
    }

    // heel k
    if(heel_k->lockStateChanged())
    {
        Well *w = dynamic_cast<Well*>(p_wellpath->toeK()->parent());

        if(heel_k->locked())
        {
            p_wellpath->setHeelK(p_wellpath->toeK());
            p_wellpath->setHeelKLocked(true);
        }
        else
        {
            shared_ptr<IntVariable> new_var(new IntVariable(p_wellpath->toeK()->parent()));
            new_var->setValue(heel_k->value());
            new_var->setMax(heel_k->max());
            new_var->setMin(heel_k->min());
            new_var->setName("WELLPATH HEEL K for well: " + w->name());

            p_wellpath->setHeelK(new_var);
            p_wellpath->setHeelKLocked(false);
        }
    }
    else if(!heel_k->locked())
    {
        p_wellpath->heelK()->setValue(heel_k->value());
    }


    // option vars
    for(int i = 0; i < m_opt_var.size(); ++i)
    {
        p_wellpath->variableOption(i)->setValue(m_opt_var.at(i)->value());
    }

    // option cons
    for(int i = 0; i < m_opt_con.size(); ++i)
    {
        p_wellpath->updateConstantOption(i, m_opt_con.at(i)->value());


    }
}




} // namespace
