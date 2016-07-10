// brpc.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "CheckMemLeaks.h"
CHECK_MEMORY_LEAKS

#include "RpcServer.h"
#include "RpcClient.h"
#include "DefaultAuthChecker.h"
#include "JsonSerializer.h"
#include "MyRpcService.h"
#include "ObjectMap.h"
#include "ObjectFactory.h"

using namespace brpc;

class LogErrorHandler : public bluemei::IErrorHandler
{
public:
	virtual bool handle(Exception& e)
	{
		Log* log = Log::getLogger();
		log->warn(e.toString());
		return true;
	}
};

#include "MyRpcApi.h"
void testMyRpcApi()
{
	cstring url = "http://127.0.0.1"; // jmtp://127.0.0.1
	cstring name = "test", password = "123456";
	MyRpcApi myApi(url, name, password);
	
	myApi.subscribe("nova.onclick", "callback.onclick");

	String sss = myApi.ping();

	myApi.print("test print!!!");

	TestObject* obj = new TestObject();
	obj->name = "Cat";
	obj->age = 28;
	obj->weight = 46.82f;
	obj->sex = true;
	myApi.print(obj);

	double f = myApi.sum(55, 33.8);
	bool sucess = myApi.playMusic("F:/����/��������/Beyond-�������.mp3");
	/*String result = myApi.execute("ping baidu.com");
	result = myApi.execute("notepad test-notepad.txt");*/
	
	MySubRpcApi* sub = (MySubRpcApi*)myApi.getSubService("sub");
	int len = sub->print("hello, sub rpc-api");

	getchar();
}

int run(int argc, char* argv[])
{
	Log* logger = Log::getLogger();
	cstring usage = "usage: brpc server|client [options...]\n";
	
	if (argc > 1)
	{
		String arg1 = argv[1];		
		if (arg1 == "server")
		{
			logger->info("brpc server starting...");

			MyRpcService dispatcher("nova");
			DefaultAuthChecker checker("test", "123456");
			//RpcServer server("http://0.0.0.0", &checker, "text/json");
			RpcServer server("amqp://guest:guest@127.0.0.1:5672/?self=node-1",
				&checker, "text/json");			
			printf("server start...\n");
			server.start(&dispatcher);
			server.wait();
		}
		else if(arg1 == "client")
		{
			logger->info("brpc client starting...");

			//cstring url = "http://127.0.0.1";//http://192.168.1.131
			cstring url = "amqp://guest:guest@127.0.0.1:5672/"\
				"?self=node-1&destination=node-1&topic=rpc";
			RpcService dispatcher;
			DefaultAuthChecker checker("", "");
			//"text/xml", "text/json", "application/brpc.bin"
			RpcClient client(url, &dispatcher, &checker,"text/json", 1000*3);

			printf("client start...\n");
			//such as: echo "hello rpc"
			if (argc > 2)
			{
				String method = argv[2];
				ObjectList args;
				for(int i=3; i<argc; i++){
					String arg = argv[i];
					char t = arg.charAt(0);
					if (t == '"')
						args.addValue(arg.substring(1, arg.length()-2).c_str());
					else if (t == '\'')
						args.addValue(arg.substring(1, arg.length()-2).c_str());
					else if (isdigit(t)){
						if (arg.find(".") > 0)
							args.addValue(CodeUtil::str2Float(arg.c_str()));
						else
							args.addValue(CodeUtil::str2Int(arg.c_str()));
					}
					else if (t == 't' && arg == "true")
						args.addValue(true);
					else if (t == 'f' && arg == "false")
						args.addValue(false);
					else
						printf("invalid para: %s\n", argv[i]);
				}

				ObjectList loginArgs;
				loginArgs.addValue("test");
				loginArgs.addValue("123456");
				String msg = client.login(loginArgs);
								
				Object* result = client.call(method, args);
				String rs = String::format(">>>>> call method %s(%s): \n"
					"=====\n"
					"%s\n"
					"=====\n", 
					method.toString().c_str(),
					args.toString().c_str(),
					result ? result->toString().c_str() : "<null>");
				printf(rs.c_str());
				logger->info(rs);
				delete result;

				msg = client.logout();
			}
			else
			{
				printf(usage);	
				printf("please input a method name.\n");		
			}
		}
		else if(arg1 == "api-test")
		{
			logger->info("brpc api-test starting...");
			testMyRpcApi();
		}
		else
			printf(usage);
	}
	else
		printf(usage);
	return 0;
}

int main(int argc, char* argv[])
{
	//_CrtSetBreakAlloc(9373);

	BRpcUtil::setBrpcDebug(true);
	//main2(argc, argv);
	
	String name = "brpc";
	if (argc > 1){
		name += "-";
		name += argv[1];
	}

	try{
		String file = String::format("/var/log/%s.log", name.c_str());
		LogManager::instance().initLogger(name, file, Log::LOG_DEBUG);
	}catch (Exception& e){
		e.printStackTrace();
	}

	Log* log = Log::getLogger(name);
	log->info("brpc starting...");
	log->updateFormatter("$time [Thread-$thread] [$level] $message");

	LogErrorHandler logHandler;
	ErrorHandler::setHandler(&logHandler);

	SocketTools::initSocketContext();

	try
	{
		log->trace("before running");
		run(argc, argv);
		log->trace("after running");
	}catch (Exception& e){
		e.printStackTrace();
		log->error("exit with exception: " + e.toString());
	}

	SocketTools::cleanUpSocketContext();

	log->info("brpc end");
	System::instance().destroy();

	system("pause");
	return 0;
}
