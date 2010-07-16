// Copyright 2009-2010 Sandia Corporation. Under the terms
// of Contract DE-AC04-94AL85000 with Sandia Corporation, the U.S.
// Government retains certain rights in this software.
// 
// Copyright (c) 2009-2010, Sandia Corporation
// All rights reserved.
// 
// This file is part of the SST software package. For license
// information, see the LICENSE file in the top level directory of the
// distribution.


#include "sst_config.h"
#include "sst/core/serialization/core.h"

#include <utility>

#include <sst/core/link.h>
#include <sst/core/simulation.h>
#include <sst/core/event.h>
#include <sst/core/pollingLinkQueue.h>
#include <sst/core/timeVortex.h>

namespace SST { 

Link::Link(LinkId_t id) :
    rFunctor( NULL ),
    defaultTimeBase( NULL ),
    latency(1),
    id(id)
{
    recvQueue = Simulation::getSimulation()->getTimeVortex();
}

Link::Link() :
    rFunctor( NULL ),
    defaultTimeBase( NULL ),
    latency(1),
    id(-1)
{
    recvQueue = Simulation::getSimulation()->getTimeVortex();
}

Link::~Link() {
    if ( type == POLL ) {
	delete recvQueue;
    }
}

void Link::setPolling() {
    type = POLL;
//     recvQueue = new EventQueue_t;
    recvQueue = new PollingLinkQueue();
}

    
void Link::setLatency(Cycle_t lat) {
    latency = lat;
}
    
    
void Link::Send( SimTime_t delay, TimeConverter* tc, Event* event ) {
//     _LINK_DBG("delay=%lu sendQueue=%p event=%p sFunctor=%p\n",
//               (unsigned long) delay,sendQueue,event,sFunctor);
    if ( tc == NULL ) {
	_abort(Link,"Cannot send an event on Link with NULL TimeConverter\n");
    }

    Cycle_t cycle = Simulation::getSimulation()->getCurrentSimCycle() +
                    tc->convertToCoreTime(delay) + latency;
    
    _LINK_DBG( "cycle=%lu\n", (unsigned long)cycle );

    if ( event == NULL ) {
	event = new NullEvent();
    }
    event->setDeliveryTime(cycle);
    event->setDeliveryLink(id,pair_link);
    
//     pair_link->recvQueue->insert( cycle, event );
    pair_link->recvQueue->insert( event );
}
    

Event* Link::Recv()
{
    Event* event = NULL;
    Simulation *simulation = Simulation::getSimulation();

    if ( !recvQueue->empty() ) {
	Activity* activity = recvQueue->front();
	if ( activity->getDeliveryTime() <=  simulation->getCurrentSimCycle() ) {
	    event = static_cast<Event*>(activity);
	    recvQueue->pop();
	}
    }
    return event;
} 
    
void Link::setDefaultTimeBase(TimeConverter* tc) {
    defaultTimeBase = tc;
}

TimeConverter* Link::getDefaultTimeBase() {
    return defaultTimeBase;
}


template<class Archive>
void
Link::serialize(Archive & ar, const unsigned int version)
{
    ar & BOOST_SERIALIZATION_NVP( recvQueue );
    // don't serialize rFunctor
    ar & BOOST_SERIALIZATION_NVP( defaultTimeBase );
    ar & BOOST_SERIALIZATION_NVP( latency );
    ar & BOOST_SERIALIZATION_NVP(pair_link);
    ar & BOOST_SERIALIZATION_NVP( type );
    ar & BOOST_SERIALIZATION_NVP(id);
}


template<class Archive>
void
SelfLink::serialize(Archive & ar, const unsigned int version)
{
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(Link);
}

    
} // namespace SST

SST_BOOST_SERIALIZATION_INSTANTIATE(SST::Link::serialize)
SST_BOOST_SERIALIZATION_INSTANTIATE(SST::SelfLink::serialize)

BOOST_CLASS_EXPORT(SST::Link)
BOOST_CLASS_EXPORT(SST::SelfLink)
