#pragma once
#include "stdafx.h"
#include "ObjectList.h"
#include "RpcInvoker.h"


namespace brpc{

/*
* RPCԶ�̵��û���
* @author ����÷
* @create 2014/7/13
*/
class RpcContext : public HashMap<String, Object*>
{
public:
	RpcContext(RpcInvoker& invoker):invoker(invoker){}
	virtual ~RpcContext(){}	
public:
	RpcInvoker& getInvoker(){ return invoker; }
protected:
	RpcInvoker& invoker;
};


}//end of namespace bluemei