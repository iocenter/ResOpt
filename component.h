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



#ifndef COMPONENT_H
#define COMPONENT_H

#include <QVector>

namespace ResOpt
{

class Stream;

/**
 * @brief Mother class of all components in the model (wells, pipes)
 * @details Every part of the Model that have streams of oil, gas, or water going through it should inheret from this class.
 *
 */
class Component
{
private:
    QVector<Stream*> m_streams;         // the streams going through the component

public:
    Component();
    Component(const Component &c);

    virtual ~Component();

    // virtual functions

    // misc functions
    void clearStreams() {m_streams.clear();}

    // add functions
    void addStream(Stream *s) {m_streams.push_back(s);}



    // set functions
    bool setStream(int i, Stream *s);


    // get functions
    int numberOfStreams() const {return m_streams.size();}
    Stream* stream(int i) {return m_streams.at(i);}



};

} // namespace ResOpt
#endif // COMPONENT_H
