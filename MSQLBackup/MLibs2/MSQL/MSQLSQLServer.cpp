#include "stdafx.h"
#include "MSQLSQLServer.h"
#include "MSQLConfig.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>


int MSQLSQLServer::Init(char * connectionstring)
{
	strcpy(this->connectionstring, connectionstring);

	ret = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &env);
	ret = SQLSetEnvAttr(env, SQL_ATTR_ODBC_VERSION, (void *)SQL_OV_ODBC3, 0);
	ret = SQLAllocHandle(SQL_HANDLE_DBC, env, &dbc);
	ret = SQLSetConnectAttr(dbc, SQL_LOGIN_TIMEOUT, (SQLPOINTER)5, 0);		// set login timeout to 5 sec
																			//ret = SQLConnect(dbc, connectionstring, SQL_NTS, NULL, 0, NULL, 0);
																			//HWND desktopHandle = GetDesktopWindow();
	wchar_t connectionstringw[1024];
	mbstowcs(connectionstringw, connectionstring, 1024);
	ret = SQLDriverConnect(dbc, 0, connectionstringw, SQL_NTS, connectionstringreturned, 1024, &connectionstringreturnedlen, SQL_DRIVER_NOPROMPT);
	if (SQL_SUCCEEDED(ret))
	{
		MSQLLog(MSQLLogFirstParameter, "MSQLSQLServer: Connected");
		char outstra[1024];
		wcstombs(outstra, connectionstringreturned, 1024);
		MSQLLog(MSQLLogFirstParameter, "MSQLSQLServer: Returned connection string was: %s", outstra);
		//if (ret == SQL_SUCCESS_WITH_INFO)
		//{
		//_tprintf(_T("Driver reported the following diagnostics\n"));
		//}
		return 0;
	}
	else
	{
		sqlstate[0] = 0;
		sqlerrormsg[0] = 0;
		ret = SQLGetDiagRec(SQL_HANDLE_DBC, dbc, 1, sqlstate, &nativeerror, sqlerrormsg, 16 * 1024, &sqlerrormsgn);
		char sqlstatea[8];
		wcstombs(sqlstatea, sqlstate, 8);
		char sqlerrormsga[16 * 1024];
		wcstombs(sqlerrormsga, sqlerrormsg, 16 * 1024);
		MSQLLog(MSQLLogFirstParameter, "MSQLSQLServer: Failed to connect: %s, %s, %s", connectionstring, sqlstatea, sqlerrormsga);
		return 1;
	}
}

int MSQLSQLServer::Destroy()
{
	SQLDisconnect(dbc);		/* disconnect from driver */
	SQLFreeHandle(SQL_HANDLE_DBC, dbc);
	SQLFreeHandle(SQL_HANDLE_ENV, env);
	return 0;
}

bool MSQLSQLServer::Execute(const char * format, ...)
{
	va_list parg;
	va_start(parg, format);
	char query[64 * 1024];
	vsnprintf(query, 64 * 1024, format, parg);

	strcpy(lastquery, query);

	SQLWCHAR queryw[64 * 1024];
	mbstowcs(queryw, query, 64 * 1024);

	SQLHSTMT stmt;
	ret = SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);
	ret = SQLPrepare(stmt, queryw, SQL_NTS);
	ret = SQLExecute(stmt);
	int ret1 = 0;
	if (!SQL_SUCCEEDED(ret))
	{
		sqlstate[0] = 0;
		sqlerrormsg[0] = 0;
		ret = SQLGetDiagRec(SQL_HANDLE_STMT, stmt, 1, sqlstate, &nativeerror, sqlerrormsg, 16 * 1024, &sqlerrormsgn);

		char sqlstatea[8];
		wcstombs(sqlstatea, sqlstate, 8);
		char sqlerrormsga[16 * 1024];
		wcstombs(sqlerrormsga, sqlerrormsg, 16 * 1024);
		MSQLLog(MSQLLogFirstParameter, "MSQLSQLServer: Execute failed: %s, %s, %s", query, sqlstatea, sqlerrormsga);

		ret1 = 1;
	}
	else
	{
		if (ret == SQL_SUCCESS_WITH_INFO)
		{
			sqlstate[0] = 0;
			sqlerrormsg[0] = 0;
			ret = SQLGetDiagRec(SQL_HANDLE_STMT, stmt, 1, sqlstate, &nativeerror, sqlerrormsg, 16 * 1024, &sqlerrormsgn);

			char sqlstatea[8];
			wcstombs(sqlstatea, sqlstate, 8);
			char sqlerrormsga[16 * 1024];
			wcstombs(sqlerrormsga, sqlerrormsg, 16 * 1024);
			MSQLLog(MSQLLogFirstParameter, "MSQLSQLServer: Execute succeeded: %s, %s, %s", query, sqlstatea, sqlerrormsga);
		}
		else
		{
			MSQLLog(MSQLLogFirstParameter, "MSQLSQLServer: Execute succeeded: %s", query);
		}
	}
	ret = SQLFreeHandle(SQL_HANDLE_STMT, stmt);

	return ret1 == 0;
}

MSQLSQLServerRecordSet * MSQLSQLServer::Query(const char * format, ...)
{
	va_list parg;
	va_start(parg, format);
	char query[64 * 1024];
	vsnprintf(query, 64 * 1024, format, parg);

	strcpy(lastquery, query);

	SQLWCHAR queryw[64 * 1024];
	mbstowcs(queryw, query, 64 * 1024);

	MSQLSQLServerRecordSet * rst = new MSQLSQLServerRecordSet();
	ret = SQLAllocHandle(SQL_HANDLE_STMT, dbc, &rst->stmt);
	ret = SQLPrepare(rst->stmt, queryw, SQL_NTS);
	ret = SQLExecute(rst->stmt);
	if (!SQL_SUCCEEDED(ret))
	{
		sqlstate[0] = 0;
		sqlerrormsg[0] = 0;
		ret = SQLGetDiagRec(SQL_HANDLE_STMT, rst->stmt, 1, sqlstate, &nativeerror, sqlerrormsg, 16 * 1024, &sqlerrormsgn);

		char sqlstatea[8];
		wcstombs(sqlstatea, sqlstate, 8);
		char sqlerrormsga[16 * 1024];
		wcstombs(sqlerrormsga, sqlerrormsg, 16 * 1024);
		MSQLLog(MSQLLogFirstParameter, "MSQLSQLServer: Query failed: %s, %s, %s", query, sqlstatea, sqlerrormsga);

		ret = SQLFreeHandle(SQL_HANDLE_STMT, rst->stmt);

		delete rst;
		rst = 0;
	}
	else
	{
		if (ret == SQL_SUCCESS_WITH_INFO)
		{
			sqlstate[0] = 0;
			sqlerrormsg[0] = 0;
			ret = SQLGetDiagRec(SQL_HANDLE_STMT, rst->stmt, 1, sqlstate, &nativeerror, sqlerrormsg, 16 * 1024, &sqlerrormsgn);

			char sqlstatea[8];
			wcstombs(sqlstatea, sqlstate, 8);
			char sqlerrormsga[16 * 1024];
			wcstombs(sqlerrormsga, sqlerrormsg, 16 * 1024);
			MSQLLog(MSQLLogFirstParameter, "MSQLSQLServer: Query succeeded: %s, %s, %s", query, sqlstatea, sqlerrormsga);
		}
		else
		{
			MSQLLog(MSQLLogFirstParameter, "MSQLSQLServer: Query succeeded: %s", query);
		}
	}

	return rst;
}

bool MSQLSQLServer::QueryClose(MSQLSQLServerRecordSet * rst)
{
	ret = SQLFreeHandle(SQL_HANDLE_STMT, rst->stmt);
	delete rst;
	return SQL_SUCCEEDED(ret);
}


unsigned long long MSQLSQLServer::GetSingleValue(const char * format, ...)
{
	va_list parg;
	va_start(parg, format);
	char query[64 * 1024];
	vsnprintf(query, 64 * 1024, format, parg);

	unsigned long long value = 0;
	if (auto rst = Query(query))
	{
		if (rst->Fetch())
		{
			*rst << value;
		}
		else
		{
			// not existing
		}
		QueryClose(rst);
	}
	else
	{
	}
	return value;
}

unsigned long long MSQLSQLServer::GetMaxID(char * tablename)
{
	return GetSingleValue("select max(id) as idmax from %s", tablename);
}

unsigned long long MSQLSQLServer::GetLastInsertID()
{
	return GetSingleValue("select @@Identity");
}

bool MSQLSQLServer::Insert(MSQLInsert & insert)
{
	char query[64 * 1024];
	char * p = query;

	p += sprintf(p, "insert into %s (", insert.tablename);

	unsigned int i;
	for (i = 0; i < insert.itemsn; i++)
	{
		MSQLInsertItem * item = insert.items + i;
		p += sprintf(p, "%s, ", item->name);
	}
	// remove last ,
	if (insert.itemsn != 0)
	{
		p -= 2;
	}

	p += sprintf(p, ") values (");

	for (i = 0; i < insert.itemsn; i++)
	{
		MSQLInsertItem * item = insert.items + i;
		strcat(p, "'");
		p++;
		SQLEncode(item->value, p);
		p += strlen(p);
		strcat(p, "', ");
		p += 3;
	}
	// remove last ,
	if (insert.itemsn != 0)
	{
		p -= 2;
	}

	p += sprintf(p, ")");

	return Execute(query);
}
