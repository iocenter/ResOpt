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


#ifndef CASEQUEUE_H
#define CASEQUEUE_H

#include <QVector>

namespace ResOpt
{

class Case;


/**
 * @brief A vector of Cases.
 * @details This class inherets from QVector. The next() function is used to get the next Case in the queue.
 *
 */
class CaseQueue : public QVector<Case*>
{
private:
    int m_next_i;

public:
    CaseQueue();


    /**
     * @brief Returns the next Case in the queue
     * @details If there are no more cases in the queue, a null pointer is returned.
     *
     * @return Case
     */
    Case* next();
};

} // namespace ResOpt

#endif // CASEQUEUE_H
