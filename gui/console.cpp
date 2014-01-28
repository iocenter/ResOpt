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


#include "console.h"

#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QLabel>

namespace ResOptGui
{

Console::Console(QWidget *parent) :
    QDockWidget(parent)
{
    QWidget *widget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(widget);


    // loader animation
    QLabel *gif_anim = new QLabel(widget);
    p_loader = new QMovie(":new/images/loader");
    gif_anim->setMovie(p_loader);
    p_loader->start();
    p_loader->stop();



    p_edit = new QTextEdit(widget);
    p_edit->setReadOnly(true);



    layout->addWidget(p_edit);

    layout->addWidget(gif_anim);



    setWidget(widget);




}

//-----------------------------------------------------------------------------------------------
// prints a message to the console
//-----------------------------------------------------------------------------------------------
void Console::printMsg(QString msg)
{
    p_edit->append(msg);
}


//-----------------------------------------------------------------------------------------------
// starts the progress bar
//-----------------------------------------------------------------------------------------------
void Console::startProgress()
{
    p_loader->start();

}

//-----------------------------------------------------------------------------------------------
// stops the progress bar
//-----------------------------------------------------------------------------------------------
void Console::stopProgress()
{
    p_loader->stop();


}


} // namespace
