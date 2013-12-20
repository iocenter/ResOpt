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


#ifndef CONSOLE_H
#define CONSOLE_H

#include <QtWidgets/QDockWidget>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QProgressBar>


namespace ResOptGui
{

class Console : public QDockWidget
{
    Q_OBJECT

private:
    QProgressBar *p_progress;
    QTextEdit *p_edit;


public:
    explicit Console(QWidget *parent = 0);
    
signals:
    
public slots:
    void printMsg(QString);
    void startProgress();
    void stopProgress();

    
};

} // namespace

#endif // CONSOLE_H
