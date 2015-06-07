#pragma once
#include "stdafx.h"
#include "Object.h"
#include "RpcInvoker.h"
#include "RpcService.h"
#include "AuthChecker.h"

namespace bluemei{
	
/*
* RPC Client
* @author ����÷
* @create 2014/7/13
*/
class RpcClient : public RpcInvoker
{
public:
	RpcClient(cstring url, RpcService* dispatcher, AuthChecker* authChecker, 
		cstring serializerType="text/json", unsigned int timeout=0);
	virtual ~RpcClient();

public:
	virtual Object* call( cstring name, const ObjectList& args );
	virtual Object* call( cstring obj, cstring name, const ObjectList& args );
	virtual void cast( cstring name, const ObjectList& args );
	virtual void cast( cstring obj, cstring name, const ObjectList& args );
	virtual bool subscribe( cstring event, cstring method );
	virtual bool unsubscribe( cstring event, cstring method );
	virtual String login( const ObjectList& args );
	virtual String logout();

	virtual bool invoke( const Headers& headers, const InputStream& input );
	virtual void notifyEvent( cstring event, Object* sender, const ObjectList& args );

	virtual String toString() const;

private:
	RpcInvoker* rpcInvoker;//implement
};


}//end of namespace bluemei