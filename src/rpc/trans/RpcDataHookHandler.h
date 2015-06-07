#pragma once
#include "Object.h"
#include "DataPackage.h"

namespace bluemei{

class RpcReceiveListener;

/*
* RPC���������շ���������
* @author ����÷
* @create 2012/8/8
*/
class RpcDataHookHandler : public virtual Object
{
public:
	virtual void onReceived(cstring name,const DataPackage& data,long time)=0;
	virtual void onSent(cstring name,const DataPackage& data,long time)=0;
	virtual void onError(cstring name,cstring err,RpcReceiveListener* listener)=0;
};

#define HOOK_ERR_RECV_STOPED "receive-stoped"
#define HOOK_ERR_CLOSED "closed"

}//end of namespace bluemei