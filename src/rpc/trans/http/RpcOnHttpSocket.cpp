#include "StdAfx.h"
#include "RpcOnHttpSocket.h"
#include "LambdaThread.h"
#include "ServerSocket.h"
#include "Exceptions.h"
#include "CodeUtil.h"
#include "HttpHeader.h"
#include "HttpParser.h"
#include "Log.h"

namespace bluemei{
	
#define PATH_RPC_SERVICE "/rpcservice"

RpcOnHttpSocket::RpcOnHttpSocket(void)
{
	m_pSocket=null;
	m_pRecvThread=null;
	m_bRecving=false;
}


RpcOnHttpSocket::~RpcOnHttpSocket(void)
{
	try{
		close();
	}catch(Exception& e){			
		System::debugInfo("%s\n",e.toString().c_str());
	}
	delete m_pRecvThread;
	delete m_pSocket;
}

void RpcOnHttpSocket::connect( const HashMap<String,String>& paras ) throw(IOException)
{
	//�ͷ�ԭ����Socket�ڴ�
	if(m_pSocket!=null)
	{
		delete m_pSocket;
	}
	//�ж��Ƿ�Ϊ�����
	String server;
	paras.get("server",server);
	if (server == "true"){
		//�����
		//�����µ�����
		String port;
		paras.get("port",port);
		ServerSocket serverSocket(CodeUtil::str2Int(port));
		m_pSocket=serverSocket.accept();
	}
	else{
		//�ͻ���
		String ip,port;
		paras.get("ip",ip);
		paras.get("port",port);
		m_pSocket=new ClientSocket();
		m_pSocket->connect(ip,CodeUtil::str2Int(port));
	}

	String timeout;
	paras.get("timeout",timeout);
	m_pSocket->setTimeout(CodeUtil::str2Int(timeout));//1000*4

	String noDelay;
	paras.get("noDelay",noDelay);
	m_pSocket->setNoDelay(noDelay=="true");

	startReceiveThread();
}

void RpcOnHttpSocket::close() throw(IOException)
{
	if (m_pSocket==null)
	{
		throwpe(Exception("null socket"));
	}
	/*stopReceiveThread();
	m_pSocket->close();*/
	
	m_pSocket->close();
	stopReceiveThread();
}

//send: header data tail
void RpcOnHttpSocket::send(const DataPackage& package) throw(IOException)
{
	m_sendLock.getLock();

	if (m_pSocket==null){
		throwpe(Exception("null socket"));
	}

	const ByteBuffer& output = package.body;
	unsigned int len=output.size();
	
	//����ͷ��
	HttpHeader* header = null;
	if (package.headers.getDefault(KEY_RESPONSE, "false") == "true"){
		header = new HttpResponse(package.headers);
		//header->addCookie("sessionId", "abcd-2234-dddd-cccc-bbbb-aaaa");		
	}
	else{
		HttpRequest* request = new HttpRequest(package.headers);
		request->setRequestType(HttpRequest::HTTP_POST);
		request->setRequestUrl(PATH_RPC_SERVICE);
		header = request; 
	}

	if(!package.headers.contain(KEY_CONTENT_LEN))
		header->setContentLength(len);
	ByteBuffer headerBuf(header->getEntrySize()*20);
	header->writeTo(headerBuf);
	m_pSocket->writeEnoughBytes((const char*)headerBuf.array(),headerBuf.size());//Header
	delete header;
	header = null;

	//��������
	m_pSocket->writeEnoughBytes((const char*)output.array(),output.size());//Data
	//֪ͨ���ݹ���
	notifyHookSent(m_pSocket->getPeerHost(),package);
		
	m_sendLock.releaseLock();
}

//�������������߳�
void RpcOnHttpSocket::startReceiveThread()
{
	if(m_pRecvThread!=null || m_bRecving)
		return;

	m_pRecvThread=new LambdaThread([&](void* pUserParameter){
		m_bRecving=true;
		while(m_bRecving){
			try{
				if(m_pSocket==null)
				{
					throwpe(IOException("null socket"));
				}
				//��ͷ��	
				LinkedList<String> lines;
				String line;
				while((line = m_pSocket->readLineByGbk()).length() > 0){
					lines.addToLast(line);
					//printf("%s\n",line.c_str());
				}
				HttpParser parser;
				HttpHeader* header = parser.parse(lines);
				word len = header->getContentLength();
				DataPackage package;
				package.headers = header->getHeaders();
				package.headers.put(KEY_CONTENT_TYPE, header->getContentType());
				package.headers.put(KEY_CHARSET, header->getCharset());
				delete header;
				header = null;
				
				//��Data
				ByteBuffer& input = package.body;
				for (int i=0; i<len; i++)
				{
					input.put(m_pSocket->readByte());
				}
				
				Log::getLogger()->debug(String::format(
					">>>>>>>>>>received: headers====%s, body====%s",
					package.headers.toString().c_str(), 
					String((cstring)package.body.array(), package.body.size()).c_str()));
				
				//֪ͨ���ݹ���
				notifyHookReceived(m_pSocket->getPeerHost(),package);
				//֪ͨ�յ�����
				notifyReceive(package);

			}catch(TimeoutException& e){
				notifyException(e);
			}catch(Exception& e){
				//֪ͨ�쳣
				m_bRecving&=notifyException(e);
			}
		}//end while
		/*try{
			m_pSocket->close();
		}catch(Exception& e){			
			System::debugInfo("%s\n",e.toString().c_str());
		}*/
		//end of thread
	},nullptr);
	m_pRecvThread->setAutoDestroy(false);
	m_pRecvThread->start();
}

void RpcOnHttpSocket::stopReceiveThread()
{
	if(m_bRecving && m_pRecvThread!=null)
	{
		m_bRecving=false;
		m_pRecvThread->wait();
	}
}

bool RpcOnHttpSocket::isRecving() const
{
	return m_bRecving;
}

bool RpcOnHttpSocket::isAlive() const
{
	return isRecving();
}


}//end of namespace bluemei