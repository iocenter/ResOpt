#include "separator.h"

#include <iostream>
#include "cost.h"
#include "pipeconnection.h"
#include "intvariable.h"
#include "realvariable.h"
#include "stream.h"

using std::cout;
using std::endl;

namespace ResOpt
{

Separator::Separator()
    : m_type(WATER),
      p_cost(0),
      p_outlet_connection(0)
{
}

Separator::Separator(const Separator &s)
    : Pipe(s)
{
    // copying base types
    m_type = s.m_type;
    m_remaining_capacity = s.m_remaining_capacity;

    // copying pointed objects
    p_cost = new Cost(*s.p_cost);
    p_outlet_connection = new PipeConnection(*s.p_outlet_connection);

    // copying variables
    p_install_time = shared_ptr<IntVariable>(new IntVariable(*s.p_install_time));
    p_remove_fraction = shared_ptr<RealVariable>(new RealVariable(*s.p_remove_fraction));
    p_remove_capacity = shared_ptr<RealVariable>(new RealVariable(*s.p_remove_capacity));

}


Separator::~Separator()
{
    if(p_cost != 0) delete p_cost;
    if(p_outlet_connection != 0) delete p_outlet_connection;
}


//-----------------------------------------------------------------------------------------------
// initializes the separator
//-----------------------------------------------------------------------------------------------
void Separator::initialize(const QVector<double> &schedule)
{
    Pipe::initialize(schedule);

    m_remaining_capacity.fill(p_remove_capacity->value(), schedule.size());
}

//-----------------------------------------------------------------------------------------------
// empties the streams, and resets the remaining capacities
//-----------------------------------------------------------------------------------------------
void Separator::emptyStreams()
{
    Pipe::emptyStreams();

    m_remaining_capacity.fill(p_remove_capacity->value());
}


//-----------------------------------------------------------------------------------------------
// reduces the remaining capacity for time step i
//-----------------------------------------------------------------------------------------------
void Separator::reduceRemainingCapacity(int i, double q)
{
    double remaining = m_remaining_capacity.at(i);

    remaining = (q > remaining) ? 0 : (remaining - q);

    m_remaining_capacity.replace(i, remaining);
}

//-----------------------------------------------------------------------------------------------
// calculates the inlet pressure of the separator
//-----------------------------------------------------------------------------------------------
void Separator::calculateInletPressure()
{

    // checking if the outlet connection is defined
    if(p_outlet_connection == 0)
    {
        cout << endl << "### Runtime Error ###" << endl
             << "NO Outlet pipe set for SEPARATOR: " << number() << endl << endl;

        exit(1);
    }

    // checking if outlet pipe have the same number of streams

    if(numberOfStreams() != outletConnection()->pipe()->numberOfStreams())
    {

        cout << endl << "### Runtime Error ###" << endl
             << "Separator and upstream pipe do not have the same number of time steps..." << endl
             << "SEPARATOR    : " << number() << ", n = " << numberOfStreams() << endl
             << "Upstream PIPE: " << outletConnection()->pipe()->number() << ", n = " << outletConnection()->pipe()->numberOfStreams() << endl << endl;

        exit(1);

    }


    // looping through the time steps
    for(int i = 0; i < numberOfStreams(); i++)
    {
        // getting the outlet pressure
        double p_out = outletConnection()->pipe()->stream(i)->pressure(stream(i)->inputUnits());

        // setting inlet pressure = outlet pressure
        stream(i)->setPressure(p_out);
    }


}




} // namespace ResOpt
