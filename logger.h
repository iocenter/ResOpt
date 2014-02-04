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


#ifndef LOGGER_H
#define LOGGER_H

#include <QObject>
#include <QString>

#include "gui/mainwindow.h"

using ResOptGui::MainWindow;

namespace ResOpt
{



class Logger : public QObject
{
    Q_OBJECT
public:
    explicit Logger(MainWindow *mw = 0, QObject *parent = 0);

    void setMainWindow(MainWindow *mw);

signals:
    void sendError(QString &message);

public slots:
    void error(QString &message);
    void warning(QString &message);


private:
    MainWindow *p_mw;

};


} // namespace

#endif // LOGGER_H
