#pragma once

#include <atlconv.h>
#include <SqlExt.h>
#include "MSQLSQLServerRecordSet.h"
#include "MSQLInsert.h"

class MSQLSQLServer
{
public:
	char connectionstring[1024];
	char lastquery[64 * 1024];

	SQLHENV env;
	SQLHDBC dbc;
	SQLRETURN ret; /* ODBC API return status */
	SQLTCHAR connectionstringreturned[1024];
	SQLSMALLINT connectionstringreturnedlen;

	SQLTCHAR sqlstate[8];
	SQLINTEGER nativeerror;
	SQLTCHAR sqlerrormsg[16 * 1024];
	SQLSMALLINT sqlerrormsgn;

	inline char * SQLEncode(const char * s, char * o)
	{
		unsigned int i = 0, j = 0;
		while (s[i] != 0)
		{
			if (s[i] != '\'')
			{
				o[j] = s[i];
				i++;
				j++;
			}
			else
			{
				o[j] = '\'';
				j++;
				o[j] = '\'';
				j++;
				i++;
			}
		}
		o[j] = 0;
		return o;
	};

	int Init(char * connectionstring);		// connectionstring should be something like: "DSN=fred;"
	int Destroy();

	bool Execute(const char * format, ...);
	MSQLSQLServerRecordSet * Query(const char * format, ...);
	bool QueryClose(MSQLSQLServerRecordSet * rst);

	unsigned long long GetSingleValue(const char * format, ...);
	unsigned long long GetMaxID(char * tablename);
	unsigned long long GetLastInsertID();
	bool Insert(MSQLInsert & insert);
};
