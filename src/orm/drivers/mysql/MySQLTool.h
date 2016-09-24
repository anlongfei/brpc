#pragma once
#include <shellapi.h>
#include <time.h>
#include "orm/drivers/mysql/MySQLConnection.h"

#define DEFAULT_TIME_FORMAT "%Y-%m-%d %H:%M:%S"


namespace brpc{

class MySQLTool : public Object
{
public:
	static string getCurrentDir()
	{
		char buf[MAX_PATH];
		memset(buf,0,MAX_PATH);
		GetCurrentDirectoryA(MAX_PATH,buf);
		return buf;
	}

	static string getCurrentTime(cstring format=DEFAULT_TIME_FORMAT)
	{
		time_t t = time(0);
		char buf[64];
		memset(buf,0,sizeof(buf));
		strftime(buf, sizeof(buf), format, localtime(&t));
		return buf;
	}

	static bool startMySQL()
	{
		//"start bin/mysqld-nt.exe --no-defaults --skip-bdb --skip-innodb --log=log.txt"
		string path=getCurrentDir();
		path+="/mysql5.0/bin/mysqld-nt.exe";//D:/c++/mfc/DiagClientMdi/MySQLConnector
		cstring parameters=" --no-defaults --skip-bdb --skip-innodb --log=log.txt";
		HINSTANCE h=ShellExecuteA(NULL, "open", path.c_str(), parameters, NULL, SW_HIDE);//SW_HIDE
		if((int)h<=32)
		{
			/*
			0 ����ϵͳ�ڴ��������ϵͳ��Դ����
			ERROR_FILE_NOT_FOUND ָ�����ļ�û���ҵ�
			ERROR_PATH_NOT_FOUND ָ���ĵ�ַû���ҵ�
			ERROR_BAD_FORMAT EXE�ļ���һ����Ч��PE�ļ���ʽ������EXE�ļ�����
			SE_ERR_ACCESSDENIED ��Ȩ���ʸ��ļ�
			SE_ERR_ASSOCINCOMPLETE �ļ�������Ч
			SE_ERR_DDEBUSY DDE�����޷������Ӧ����ΪDDE�������ڱ�����
			SE_ERR_DDEFAIL DDE����ʧ�ܡ�
			SE_ERR_DDETIMEOUT DDE�����޷������Ӧ����Ϊ����ʱ
			SE_ERR_DLLNOTFOUND ָ����DLL�ļ�û�ҵ�
			SE_ERR_FNF ָ���ļ�δ�ҵ�
			SE_ERR_NOASSOC û�й�������
			SE_ERR_OOM �ڴ治�����޷������Ӧ


			SE_ERR_PNF ָ���ĵ�ַδ�ҵ�

			SE_ERR_SHARE ����Խ���쳣*/
			printf("ShellExecute error:%d\n",(int)h);
			return false;
		}
		WaitForSingleObject(h,1000*1);
		return true;
	}

	static bool stopMySQL(const string& passw)
	{
		/*string path=getCurrentDir();
		path+="/mysql5.0/mysql_stop.bat";
		HINSTANCE h=ShellExecuteA(NULL,  "open", path.c_str(), NULL, NULL, SW_SHOWNORMAL);//SW_HIDE
		WaitForSingleObject(h,1000*30);*/

		//bin\mysqladmin --user=root --password= shutdown
		string path=getCurrentDir();
		path+="/mysql5.0/bin/mysqladmin.exe";
		string parameters="  --user=root --password="+passw+" shutdown";
		HINSTANCE h=ShellExecuteA(NULL, "open", path.c_str(), parameters.c_str(), NULL, SW_HIDE);
		return (int)h>32;
	}

	static void printResult(ResultSet* rs)
	{
		printf("--------------------------------------------------------\n");
		int fieldNum=rs->fieldsCount();
		while(rs->next())
		{
			for (int i=0; i<fieldNum; i++) {
				if(i>0)
					printf(" | ");
				printf("%s",rs->getString(i));
			}
			printf("\n--------------------------------------------------------\n");
		}
		rs->beforeFirst();

		/*rs->afterLast();
		while(rs->previous())
		{
			for (int i=0; i<fieldNum; i++) {
				if(i>0)
					printf(" | ");
				printf("%s",rs->getString(i));
			}
			printf("\n--------------------------------------------------------\n");
		}
		rs->afterLast();*/
	}
};


}//end of namespace brpc