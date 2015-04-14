#pragma once
#include "Object.h"
#include "RpcException.h"
#include "ResourceLock.h"
#include "LinkedList.cpp"
#include "System.h"
#include "DataPackage.h"

namespace bluemei{

typedef unsigned short word;
typedef unsigned int dword;


/*
* RPC�������ݼ�����
* @author ����÷
* @create 2012/5/28
*/
class RpcReceiveListener : public virtual Object
{
public:
	virtual bool onReceive(DataPackage& input)=0;
	virtual bool onException(Exception& e)=0;
};

/*
* RPC���������շ���������
* @author ����÷
* @create 2012/8/8
*/
class RpcDataHookHandle : public virtual Object
{
public:
	virtual void onReceived(cstring name,const DataPackage& data,long time)=0;
	virtual void onSent(cstring name,const DataPackage& data,long time)=0;
};

/*
* RPC����㴦���� 
* RPC��������ӿ�(RpcConnection)
*
* @author ����÷
* @create 2012/5/28
*/
class RpcSocket : public Object
{
public:
	RpcSocket();
	virtual ~RpcSocket();
public:
	virtual void connect(const HashMap<String,String>& paras) throw(RpcException)=0;//��Map,Ҳ����stl map,��������΢�鷳
	virtual void close() throw(RpcException)=0;

	virtual void send(const DataPackage& output) throw(RpcException)=0;
	virtual void setReceiveListener(RpcReceiveListener* listener);

	virtual void setDataHookHandler(RpcDataHookHandle* hook);
	
	virtual bool isAlive()const=0;

	virtual DataPackage sendSynch(const DataPackage& output);
protected:
	virtual bool notifyReceive(DataPackage& input);
	virtual bool notifyException(Exception& e);

	virtual void notifyHookReceived(cstring name,const DataPackage& data,long time=0);
	virtual void notifyHookSent(cstring name,const DataPackage& data,long time=0);
protected:
	RpcReceiveListener* dataListener;

	RpcDataHookHandle* dataHookHandler;

	LinkedList<DataPackage> m_recvPacketList;
	bool m_bWaitData;
	String m_idWaitData;
	SyncLock m_waitLock;
};


}//end of namespace bluemei