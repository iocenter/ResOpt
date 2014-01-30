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


#ifndef INSPECTORWELLPATH_H
#define INSPECTORWELLPATH_H

#include <QWidget>
#include <QList>

#include "wellpath.h"

class QLineEdit;
class QLabel;
class QGroupBox;
class QPushButton;





using ResOpt::WellPath;
#include "inspectorvariable.h"
#include "inspectorconstant.h"
#include <QtWidgets/QLineEdit>

namespace ResOptGui
{
class InspectorHeelVariable;

class InspectorWellPath : public QWidget
{
    Q_OBJECT
public:
    explicit InspectorWellPath(WellPath *wp ,  QWidget *parent = 0);

    void save();


signals:

public slots:

private:
    WellPath *p_wellpath;

    QGroupBox *box_toe;
    QGroupBox *box_heel;
    QGroupBox *box_opt_var;
    QGroupBox *box_opt_con;


    QList<InspectorVariable*> m_opt_var;
    QList<InspectorConstant*> m_opt_con;

    InspectorVariable *toe_i;
    InspectorVariable *toe_j;
    InspectorVariable *toe_k;
    InspectorHeelVariable *heel_i;
    InspectorHeelVariable *heel_j;
    InspectorHeelVariable *heel_k;

    //QLineEdit *p_led_k1;
    //QLineEdit *p_led_k2;
    //QLineEdit *p_led_wi;


};

} // namespace

#endif // INSPECTORWELLPATH_H
