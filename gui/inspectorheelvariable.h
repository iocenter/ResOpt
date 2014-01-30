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


#ifndef INSPECTORHEELVARIABLE_H
#define INSPECTORHEELVARIABLE_H

#include <QWidget>

class QLineEdit;
class QLabel;
class QCheckBox;

namespace ResOptGui
{

class InspectorHeelVariable : public QWidget
{
    Q_OBJECT
public:
    explicit InspectorHeelVariable(const QString &name, double value, double max, double min, bool locked, QWidget *parent = 0, bool header = false);

    double value();
    double max();
    double min();
    bool locked();
    bool lockStateChanged();

signals:

public slots:
    void onLockChanged(bool checked);

private:

    QLineEdit *p_max;
    QLineEdit *p_min;
    QLineEdit *p_value;
    QCheckBox *p_locked;

    QLabel *p_name;
    QLabel *p_type;

    bool m_original_lock_state;


};

} // namespace

#endif // INSPECTORHEELVARIABLE_H
