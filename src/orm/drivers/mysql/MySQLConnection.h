#pragma once
#include "orm/drivers/mysql/include/mysql.h"
#include "SqlExpression.h"
#include "Driver.h"


namespace brpc{


//�����
class MySQLResultSet : public ResultSet
{
protected:
	MySQLResultSet();
private:
	friend class MySQLConnection;
	MySQLResultSet(MYSQL_RES *r);
public:
	virtual ~MySQLResultSet();
public:
	//ȡ��һ����¼
	virtual bool next();
	virtual bool previous();

	//��λ
	virtual bool absolute(uint64 row);
	virtual void afterLast();
	virtual void beforeFirst();

	//����������е�����
	virtual uint32 fieldsCount() const;
	//����������е�����
	virtual uint64 rowsCount() const;

	virtual uint32 columnIndex(cstring columnName) const;

	//�ر�
	virtual void close();
	virtual bool isClosed() const;

	//ȡ�ַ�������
	virtual cstring getString(uint32 columnIndex);
	virtual cstring getString(cstring columnName);

	//ȡint������
	virtual int getInt(uint32 columnIndex);
	virtual int getInt(cstring columnName);

	//ȡdouble������
	virtual double getDouble(uint32 columnIndex);
	virtual double getDouble(cstring columnName);

	//ȡ�ֽ�����
	virtual ByteArray getBytes(uint32 columnIndex);
	virtual ByteArray getBytes(cstring columnName);

	//ȡ��Object��
	virtual Object* getObject(uint32 columnIndex);
	virtual Object* getObject(cstring columnName);

	//ȡ���ݳ���
	virtual size_t getDataLength(uint32 columnIndex);
	virtual size_t getDataLength(cstring columnName);
protected:
	void checkIndex(uint32 columnIndex);
protected:
	MYSQL_RES *m_result;
	uint64 m_previousIndex;
};


//����
class MySQLConnection : public DriverConnection
{
public:
	MySQLConnection();
private:
	friend class MySQLDriver;
	MySQLConnection(MYSQL *c);
public:
	virtual void open(const String& connInfo);
	virtual void close();

	virtual void execute(cstring sql, size_t len=0);
	virtual ResultSet* executeQuery(cstring sql, size_t len=0);
	//update,delete,insert��Ӱ��ļ�¼��
	virtual int executeUpdate(cstring sql, size_t len=0);

	virtual bool usingDatabase(cstring dbName);
	virtual void setCharset(cstring charset);//"utf8"

	//��������������
	virtual void executeSQLWithBytes(cstring sql,cstring buf,size_t len);
	//�����������������
	virtual void executeSQLWithBytesArray(cstring sql,ByteArray bufs[],int num);
protected:
	MYSQL *m_mysql;
};

//����
class MySQLDriver
{
public:
	static MySQLConnection* connect(
		const char *host,
		const char *username,
		const char *password,
		const char *db,
		unsigned int port=0)
	{
		MYSQL *conn=NULL;
		conn = mysql_init(NULL);
		if (conn == NULL) {
			cstring error=mysql_error(conn);
			throw SQLException(error);
		}
		if ((mysql_real_connect(conn, host, username, password, db, port, NULL, 0)) == NULL) {
			cstring error=mysql_error(conn);
			throw SQLException(error);
		}
		return new MySQLConnection(conn);
	}
};


}//end of namespace brpc