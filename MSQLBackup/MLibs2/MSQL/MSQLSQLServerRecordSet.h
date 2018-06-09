#pragma once

class MSQLSQLServerRecordSet
{
public:
	SQLHSTMT stmt;
	
	// if a Get results in a return of false, you can investigate these values to get more details about it
	SQLLEN indicator;
	SQLRETURN sqlret;

	int indexcounter;

	inline bool Fetch()
	{
		indexcounter = 0;
		return SQL_SUCCEEDED(SQLFetch(stmt));
	}

	/////////////////////////////////////////////////////////////////////////////////////////////
	// flexible column access

	inline bool Get(const int index, char * s, const unsigned int sn)
	{
		sqlret = SQLGetData(stmt, index + 1, SQL_C_CHAR, s, sn, &indicator);
		return SQL_SUCCEEDED(sqlret);
	}

	inline bool Get(const int index, bool & v)
	{
		sqlret = SQLGetData(stmt, index + 1, SQL_C_BIT, &v, sizeof(v), &indicator);
		return SQL_SUCCEEDED(sqlret);
	}

	inline bool Get(const int index, char & v)
	{
		int t;
		sqlret = SQLGetData(stmt, index + 1, SQL_C_LONG, &t, sizeof(v), &indicator);
		v = t;
		return SQL_SUCCEEDED(sqlret);
	}

	inline bool Get(const int index, unsigned char & v)
	{
		unsigned int t;
		sqlret = SQLGetData(stmt, index + 1, SQL_C_ULONG, &t, sizeof(v), &indicator);
		v = t;
		return SQL_SUCCEEDED(sqlret);
	}

	inline bool Get(const int index, short int & v)
	{
		sqlret = SQLGetData(stmt, index + 1, SQL_C_SHORT, &v, sizeof(v), &indicator);
		return SQL_SUCCEEDED(sqlret);
	}

	inline bool Get(const int index, unsigned short int & v)
	{
		sqlret = SQLGetData(stmt, index + 1, SQL_C_USHORT, &v, sizeof(v), &indicator);
		return SQL_SUCCEEDED(sqlret);
	}

	inline bool Get(const int index, int & v)
	{
		sqlret = SQLGetData(stmt, index + 1, SQL_C_LONG, &v, sizeof(v), &indicator);
		return SQL_SUCCEEDED(sqlret);
	}

	inline bool Get(const int index, unsigned int & v)
	{
		sqlret = SQLGetData(stmt, index + 1, SQL_C_ULONG, &v, sizeof(v), &indicator);
		return SQL_SUCCEEDED(sqlret);
	}

	inline bool Get(const int index, long long & v)
	{
		sqlret = SQLGetData(stmt, index + 1, SQL_C_SBIGINT, &v, sizeof(v), &indicator);
		return SQL_SUCCEEDED(sqlret);
	}

	inline bool Get(const int index, unsigned long long & v)
	{
		sqlret = SQLGetData(stmt, index + 1, SQL_C_UBIGINT, &v, sizeof(v), &indicator);
		return SQL_SUCCEEDED(sqlret);
	}

	inline bool Get(const int index, float & v)
	{
		sqlret = SQLGetData(stmt, index + 1, SQL_C_FLOAT, &v, sizeof(v), &indicator);
		return SQL_SUCCEEDED(sqlret);
	}

	inline bool Get(const int index, double & v)
	{
		sqlret = SQLGetData(stmt, index + 1, SQL_C_DOUBLE, &v, sizeof(v), &indicator);
		return SQL_SUCCEEDED(sqlret);
	}

	
	/////////////////////////////////////////////////////////////////////////////////////////////
	// iterative column access

	inline bool Get(char * s, const unsigned int sn)
	{
		return Get(indexcounter++, s, sn);
	}

	inline bool Get(bool & v)
	{
		return Get(indexcounter++, v);
	}

	inline bool Get(char & v)
	{
		return Get(indexcounter++, v);
	}

	inline bool Get(unsigned char & v)
	{
		return Get(indexcounter++, v);
	}

	inline bool Get(short int & v)
	{
		return Get(indexcounter++, v);
	}

	inline bool Get(unsigned short int & v)
	{
		return Get(indexcounter++, v);
	}

	inline bool Get(int & v)
	{
		return Get(indexcounter++, v);
	}

	inline bool Get(unsigned int & v)
	{
		return Get(indexcounter++, v);
	}

	inline bool Get(long long & v)
	{
		return Get(indexcounter++, v);
	}

	inline bool Get(unsigned long long & v)
	{
		return Get(indexcounter++, v);
	}

	inline bool Get(float & v)
	{
		return Get(indexcounter++, v);
	}
	
	inline bool Get(double & v)
	{
		return Get(indexcounter++, v);
	}


	/////////////////////////////////////////////////////////////////////////////////////////////
	// operator iterative column access

	inline MSQLSQLServerRecordSet & GetString(char * s, const unsigned int sn)
	{
		Get(indexcounter++, s, sn);
		return (*this);
	}

	// convenient, but requires char[1024]
	inline MSQLSQLServerRecordSet & operator<<(char * v)
	{
		Get(v, 1024);
		return (*this);
	}

	inline MSQLSQLServerRecordSet & operator<<(bool & v)
	{
		Get(v);
		return (*this);
	}

	inline MSQLSQLServerRecordSet & operator<<(char & v)
	{
		Get(v);
		return (*this);
	}

	inline MSQLSQLServerRecordSet & operator<<(unsigned char & v)
	{
		Get(v);
		return (*this);
	}

	inline MSQLSQLServerRecordSet & operator<<(short int & v)
	{
		Get(v);
		return (*this);
	}

	inline MSQLSQLServerRecordSet & operator<<(unsigned short int & v)
	{
		Get(v);
		return (*this);
	}

	inline MSQLSQLServerRecordSet & operator<<(int & v)
	{
		Get(v);
		return (*this);
	}

	inline MSQLSQLServerRecordSet & operator<<(unsigned int & v)
	{
		Get(v);
		return (*this);
	}

	inline MSQLSQLServerRecordSet & operator<<(long long & v)
	{
		Get(v);
		return (*this);
	}
	
	inline MSQLSQLServerRecordSet & operator<<(unsigned long long & v)
	{
		Get(v);
		return (*this);
	}
	
	inline MSQLSQLServerRecordSet & operator<<(float & v)
	{
		Get(v);
		return (*this);
	}
	
	inline MSQLSQLServerRecordSet & operator<<(double & v)
	{
		Get(v);
		return (*this);
	}
};
