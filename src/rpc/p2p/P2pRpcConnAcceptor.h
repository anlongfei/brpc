#pragma once
#include "stdafx.h"
#include "RpcServer.h"
#include "RpcSocket.h"

namespace bluemei{
	
class P2pRpcConnAcceptor : public RpcConnAcceptor
{
public:
	P2pRpcConnAcceptor(RpcServer* server);
	virtual ~P2pRpcConnAcceptor(){}
public:
	virtual void run();
	virtual void stop();
protected:
	virtual void waitConnection();
	virtual void addConnection(RpcSocket* rpcSocket);
private:
	Url url;
	volatile bool running;
};

}//end of namespace bluemei