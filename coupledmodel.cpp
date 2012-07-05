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


#include "coupledmodel.h"
#include "midpipe.h"
#include "pipeconnection.h"
#include "binaryvariable.h"
#include "intvariable.h"
#include "stream.h"
#include "productionwell.h"
#include "separator.h"

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
                // checking if it is a midpipe or separator
                MidPipe *p_mid = dynamic_cast<MidPipe*>(prod_well->pipeConnection(j)->pipe());
                Separator *p_sep = dynamic_cast<Separator*>(prod_well->pipeConnection(j)->pipe());
                if(p_mid != 0) addStreamsUpstream(p_mid);
                else if(p_sep != 0) addStreamsUpstream(p_sep);


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


        // finding the flow fraction from this well to the pipe
        double frac = w->pipeConnection(i)->variable()->value();

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


        // finding the flow fraction from this pipe to the upstream pipe
        double frac = p->outletConnection(i)->variable()->value();


        // looping through the streams, adding the rate from this pipe
        for(int j = 0; j < p->numberOfStreams(); ++j)
        {
            Stream s = *p->stream(j) * frac;


            // adding the contribution to the upstream pipe
            upstream->addToStream(j, s);

        }

        // then checking if the outlet pipe connection is a midpipe or separator
        MidPipe *p_mid = dynamic_cast<MidPipe*>(upstream);
        Separator *p_sep = dynamic_cast<Separator*>(upstream);

        if(p_mid != 0) addStreamsUpstream(p_mid);
        else if(p_sep != 0) addStreamsUpstream(p_sep);

    }
}


//-----------------------------------------------------------------------------------------------
// adds the rates from the pipe to all upstream connections
//-----------------------------------------------------------------------------------------------
void CoupledModel::addStreamsUpstream(Separator *s)
{
    // pointer to the upstream connected pipe
    Pipe *upstream = s->outletConnection()->pipe();

    // looping through the streams, adding the contribution from the separator
    for(int i = 0; i < s->numberOfStreams(); ++i)
    {
        Stream str = *s->stream(i);

        // checking if the separator is installed
        if(i >= s->installTime()->value())
        {
            // checking if any of the phases should be removed
            if(s->removeGas()) str.setGasRate(0.0);
            if(s->removeOil()) str.setOilRate(0.0);
            if(s->removeWater()) str.setWaterRate(0.0);
        }

        // adding the stream to the upstream pipe
        upstream->addToStream(i, str);
    }

    // then checking if the upstream pipe is a midpipe or separator
    MidPipe *p_mid = dynamic_cast<MidPipe*>(upstream);
    Separator *p_sep = dynamic_cast<Separator*>(upstream);

    if(p_mid != 0) addStreamsUpstream(p_mid);
    else if(p_sep != 0) addStreamsUpstream(p_sep);

}


} // namespace ResOpt
