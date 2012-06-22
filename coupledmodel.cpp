#include "coupledmodel.h"
#include "midpipe.h"
#include "pipeconnection.h"
#include "binaryvariable.h"
#include "stream.h"
#include "productionwell.h"

#include <iostream>

using std::cout;
using std::endl;

namespace ResOpt
{

CoupledModel::CoupledModel()
{
}

CoupledModel::CoupledModel(const CoupledModel &m)
    : Model(m)
{}

//-----------------------------------------------------------------------------------------------
// updates the rates flowing through every element in the model
//-----------------------------------------------------------------------------------------------
void CoupledModel::updateStreams()
{

    cout << "Updating the streams for the pipe system..." << endl;

    // first need to empty all the streams in all pipes
    for(int i = 0; i < numberOfPipes(); ++i) pipe(i)->emptyStreams();


    // starting with the production wells, feeding the rates to the connected pipes
    for(int i = 0; i < numberOfWells(); ++i)
    {
        // trying to cast to production well
        ProductionWell *prod_well = dynamic_cast<ProductionWell*>(well(i));

        if(prod_well != 0)  // this is a production well
        {
            // adding the streams from this well to the upstream pipes connected to it
            addStreamsUpstream(prod_well);

            // looping through the outlet connections of the well, doing the same
            for(int j = 0; j < prod_well->numberOfPipeConnections(); ++j)
            {
                // checking if it is a mid pipe
                MidPipe *p_mid = dynamic_cast<MidPipe*>(prod_well->pipeConnection(j)->pipe());
                if(p_mid != 0)
                {
                    addStreamsUpstream(p_mid);

                }

            } // pipe connection
        } // production well
    } // well
}

//-----------------------------------------------------------------------------------------------
// adds the rates from the well to the direct upstream connections
//-----------------------------------------------------------------------------------------------
void CoupledModel::addStreamsUpstream(ProductionWell *w)
{

    // looping through the pipes connected to the well
    for(int i = 0; i < w->numberOfPipeConnections(); ++i)
    {

        Pipe *p = w->pipeConnection(i)->pipe();     // pointer to the pipe

        cout << "adding streams from well " << w->name().toAscii().data() << " to Pipe #" << p->number() << endl;

        // finding the flow fraction from this well to the pipe
        double frac = w->pipeConnection(i)->variable()->value();
        cout << "frac = " << frac << endl;

        // calculating the rate from this well to the pipe vs. time
        for(int j = 0; j < w->numberOfStreams(); ++j)
        {
            Stream s = *w->stream(j) * frac;

            // adding the rate contribution from this well to what is allready going through the pipe
            p->addToStream(j, s);

        }

    }

}

//-----------------------------------------------------------------------------------------------
// adds the rates from the pipe to all upstream connections
//-----------------------------------------------------------------------------------------------
void CoupledModel::addStreamsUpstream(MidPipe *p)
{
    // looping through the pipes connected to the pipe
    for(int i = 0; i < p->numberOfOutletConnections(); ++i)
    {
        Pipe *upstream = p->outletConnection(i)->pipe();    // pointer to the upstream pipe

        cout << "adding streams from Pipe #" << p->number() << "to Pipe #" << upstream->number() << endl;

        // finding the flow fraction from this pipe to the upstream pipe
        double frac = p->outletConnection(i)->variable()->value();

        cout << "frac = " << frac << endl;

        // looping through the streams, adding the rate from this pipe
        for(int j = 0; j < p->numberOfStreams(); ++j)
        {
            Stream s = *p->stream(j) * frac;

            cout << "adding:" << endl;
            s.printToCout();

            // adding the contribution to the upstream pipe
            upstream->addToStream(j, s);

            cout << "now the total is:" << endl;
            upstream->stream(j)->printToCout();
        }

        // then checking if the outlet pipe connection is a midpipe
        MidPipe *p_mid = dynamic_cast<MidPipe*>(upstream);
        if(p_mid != 0)
        {
            // recursive call
            addStreamsUpstream(p_mid);
        }

    }
}

} // namespace ResOpt
