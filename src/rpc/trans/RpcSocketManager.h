#pragma once
#include "Object.h"
#include "RpcSocket.h"

namespace bluemei{

/*
* RPC�������ݼ�����
* @author ����÷
* @create 2012/5/28
*/
class RpcSocketManager : public Object
{
public:
	static RpcSocket* getRpcSocket(const String& name);
};



}//end of namespace bluemei