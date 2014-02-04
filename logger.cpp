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


#include "logger.h"


#include <iostream>

using std::cout;
using std::endl;


namespace ResOpt
{

Logger::Logger(MainWindow *mw, QObject *parent) :
    QObject(parent),
    p_mw(mw)
{
    if(p_mw != 0) connect(this, SIGNAL(sendError(QString&)), p_mw, SLOT(error(QString&)));
}


//-----------------------------------------------------------------------------------------------
// set main window
//-----------------------------------------------------------------------------------------------
void Logger::setMainWindow(MainWindow *mw)
{
    p_mw = mw;
    connect(this, SIGNAL(sendError(QString&)), p_mw, SLOT(error(QString&)));
}

//-----------------------------------------------------------------------------------------------
// error
//-----------------------------------------------------------------------------------------------
void Logger::error(QString &message)
{

    if(p_mw == 0)
    {
        cout << endl
            << "### Runtime Error ###" << endl
            << message.toStdString() << endl
            << "#####################" << endl;

        exit(1);
    }

    else emit sendError(message);

}

//-----------------------------------------------------------------------------------------------
// warning
//-----------------------------------------------------------------------------------------------
void Logger::warning(QString &message)
{}


} // namespace
