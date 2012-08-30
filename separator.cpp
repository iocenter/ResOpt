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
        double p_out = outletConnection()->pipe()->stream(i)->pressure();

        // setting inlet pressure = outlet pressure
        stream(i)->setPressure(p_out);
    }


}




} // namespace ResOpt
