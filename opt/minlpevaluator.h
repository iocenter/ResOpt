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


#ifndef MINLPEVALUATOR_H
#define MINLPEVALUATOR_H

#include <QList>
#include <QVector>

namespace ResOpt
{

class Case;
class Optimizer;


class MINLPEvaluator
{
private:
    Optimizer *p_optimizer;

    QList<Case*> m_results;

    QVector<double> m_best_objs;
    QVector<double> m_best_infeas;

    int m_iterations;

public:
    MINLPEvaluator(Optimizer *o);
    ~MINLPEvaluator();

    Case* solveContineousProblem(Case *discrete_vars);
    bool shouldContinue(int i, double obj, double infeas);
    Case* findResult(Case *c);

    int iterations() const {return m_iterations;}


};


} // namespace ResOpt

#endif // MINLPEVALUATOR_H
