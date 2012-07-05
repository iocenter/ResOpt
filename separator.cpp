#include "separator.h"

#include <iostream>
#include "cost.h"
#include "pipeconnection.h"
#include "intvariable.h"
#include "stream.h"

using std::cout;
using std::endl;

namespace ResOpt
{

Separator::Separator()
    : m_remove_water(false),
      m_remove_oil(false),
      m_remove_gas(false),
      p_cost(0),
      p_outlet_connection(0)
{
}

Separator::Separator(const Separator &s)
    : Pipe(s)
{
    // basic types
    m_remove_water = s.m_remove_water;
    m_remove_oil = s.m_remove_oil;
    m_remove_gas = s.m_remove_gas;

    // copying pointed objects
    p_cost = new Cost(*s.p_cost);
    p_outlet_connection = new PipeConnection(*s.p_outlet_connection);

    // copying variable
    p_install_time = shared_ptr<IntVariable>(new IntVariable(*s.p_install_time));

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
    cout << "Calculating inlet pressures for SEPARATOR: " << number() << endl;

    // TODO: should calculate the average of the outlet pressures

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
