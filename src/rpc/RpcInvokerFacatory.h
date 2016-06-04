#pragma once
#include "stdafx.h"
#include "AuthChecker.h"
#include "RpcService.h"

namespace brpc{

class RpcServer;
class RpcConnAcceptor;

/*
* RPCԶ�̵���������
* @author Javeme
* @create 2014/7/13
*/
class RpcInvokerFacatory : public Object
{
public:
	static RpcInvoker* loadRpcInvoker(cstring url, RpcService* dispatcher, 
		AuthChecker* authChecker, cstring serializerType, unsigned int timeout=0);
	static RpcConnAcceptor* loadRpcAcceptor(cstring url, RpcServer* server);
};

}//end of namespace brpc