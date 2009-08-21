/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#ifndef AX_NETWORK_ALL_H
#define AX_NETWORK_ALL_H

#include "bitmsg.h"
#include "bitmsgdelta.h"
#include "peer.h"
#include "networksystem.h"

namespace Axon { namespace Network {



	using namespace Axon::Logic;
}} // namespace Axon::Network

namespace Axon {
	typedef Network::BitMsg netBitMsg;
	typedef Network::BitMsgDelta netBitMsgDelta;
	typedef Network::Peer netPeer;
}

#endif // end guardian
