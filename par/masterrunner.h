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


#ifndef MASTERRUNNER_H
#define MASTERRUNNER_H

#include <QObject>
#include <QList>
#include <QString>

namespace ResOpt
{

class Runner;
class Case;


class MasterRunner : public QObject
{
    Q_OBJECT
private:
    QString m_driver_file;
    int m_parallel_runs;

    QString m_path;

    QList<Runner*> m_runners;

public:
    explicit MasterRunner(const QString &driver_file, int parallel_runs, QObject *parent = 0);

    ~MasterRunner();

    bool initialize();



    
signals:
    void optimizationFinished();

    
public slots:
    void run();
    void onRunnerFinished(Runner *r, Case *c);
    
};

} //namespace

#endif // MASTERRUNNER_H
