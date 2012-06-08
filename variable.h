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

#ifndef VARIABLE_H
#define VARIABLE_H

#include <QString>

/**
 * @brief Base class for all varaibles.
 *
 */
class Variable
{

private:

    QString m_name;     // identifying name for the variable

public:
    Variable();


    /**
     * @brief Checks if this is a variable.
     * @details A Variable is not considered a variable if the max and min bounds are equal to the value.
     *
     * @return bool
     */
    virtual bool isVariable() = 0;

    // set functions
    void setName(const QString &n) {m_name = n;}

    // get functions
    const QString& name() const {return m_name;}


};

#endif // VARIABLE_H
