#include "endpipe.h"

#include <iostream>
#include "pressuredropcalculator.h"
#include "stream.h"

using std::cout;
using std::endl;

EndPipe::EndPipe()
    : m_outletpressure(-1.0)
{
}


//-----------------------------------------------------------------------------------------------
// calculates the inlet pressure of the pipe
//-----------------------------------------------------------------------------------------------
void EndPipe::calculateInletPressure()
{
    cout << "Calculating inlet pressures for PIPE: " << number() << endl;

    // calculating the total rate going through the pipe
    aggregateStreams();

    // looping through the time steps
    for(int i = 0; i < numberOfStreams(); i++)
    {
        double dp = calculator()->pressureDrop(stream(i), outletPressure());    // the pressure drop in the pipe

        stream(i)->setPressure(dp + outletPressure());      // setting the inlet pressure for the time step
    }


}
