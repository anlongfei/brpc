#pragma once
#include "stdafx.h"
#include "Object.h"
#include "AbstructRpcContext.h"

namespace bluemei{


/*
* ����AMQPЭ��RPCԶ�̵��û���
* @author ����÷
* @create 2014/7/13
*/
class AmqpRpcContext : public AbstructRpcContext
{
public:
	AmqpRpcContext(){}
	virtual ~AmqpRpcContext(){}
};


}//end of namespace bluemei