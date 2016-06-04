#pragma once
#include "stdafx.h"
#include "RpcConnection.h"


namespace brpc{

/*
* ����AMQPЭ��RPCԶ�̵��û���
* @author Javeme
* @create 2014/7/13
*/
class AmqpRpcConnection : public RpcConnection
{
public:
	AmqpRpcConnection(cstring url, RpcService* dispatcher, 
		AuthChecker* authChecker, cstring serializerType,
		unsigned int timeout=0)
		: RpcConnection(dispatcher, authChecker, serializerType)
	{

	}
	virtual ~AmqpRpcConnection(){}

public:
	virtual bool checkConnected() 
	{
		throw std::exception("The method or operation is not implemented.");
	}

	virtual void onSend( const DataPackage& output ) 
	{
		throw std::exception("The method or operation is not implemented.");
	}

	virtual DataPackage onSendSynch( const DataPackage& output ) 
	{
		throw std::exception("The method or operation is not implemented.");
	}

	virtual bool onSerializeException( SerializeException& e ) 
	{
		throw std::exception("The method or operation is not implemented.");
	}

	virtual bool onReturnCallException( Exception& e ) 
	{
		throw std::exception("The method or operation is not implemented.");
	}

};


}//end of namespace brpc