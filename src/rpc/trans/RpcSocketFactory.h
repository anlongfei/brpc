#pragma once
#include "Object.h"
#include "RpcSocket.h"

namespace bluemei{

/*
* RPC socket ����
* @author ����÷
* @create 2012/5/28
*/
class RpcSocketFactory : public Object
{
public:
	static RpcSocket* getRpcSocket(const String& name);
};



}//end of namespace bluemei