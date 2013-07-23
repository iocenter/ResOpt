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


#ifndef INSPECTORSEPARATOR_H
#define INSPECTORSEPARATOR_H

#include <QWidget>
#include <QPushButton>

namespace ResOpt
{
class Separator;
}

using ResOpt::Separator;

namespace ResOptGui
{

class InspectorVariable;
class InspectorVariableInstall;

class InspectorSeparator : public QWidget
{
    Q_OBJECT
private:
    Separator *p_separator;

    InspectorVariableInstall *p_var_install;
    InspectorVariable *p_var_capacity;
    InspectorVariable *p_var_removefrac;

    QPushButton m_btn_close;
    QPushButton m_btn_ok;
    QPushButton m_btn_plot;

    void construct();


public:
    explicit InspectorSeparator(Separator *sep, QWidget *parent = 0);
    
signals:
    void sendMsg(QString);
    
public slots:
    void saveAndClose();
    void openPlot();
    
};

} // namespace

#endif // INSPECTORSEPARATOR_H
