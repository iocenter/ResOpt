/*
 * This file is part of the ResOpt project.
 *
 * Copyright (C) 2011-2012 Aleksander O. Juell <aleksander.juell@ntnu.no>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
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

#include <QtCore/QCoreApplication>
#include <iostream>


#include "runner.h"

using std::cout;
using std::endl;


int main(int argc, char *argv[])
{
    // QCoreApplication a(argc, argv);

    Runner *r = 0;

    if(argc == 2)
    {
        r = new Runner(argv[1]);
        r->run();
    }
    else if(argc == 1)
    {
        r = new Runner("driver.dat");
        r->run();

    }
    else
    {
        cout << "Wrong input arguments!" << endl
             << "Correct usage: .\\ResOpt driver_file" << endl;
    }

    if(r != 0) delete r;

    
    return 0;
}
