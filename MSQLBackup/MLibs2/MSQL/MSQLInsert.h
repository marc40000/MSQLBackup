#pragma once

#include <string.h>
#include <stdio.h>

class MSQLInsertItem
{
public:
	char name[128];
	char value[64 * 1024];
	bool quotes;
};

const unsigned int MSQLInsertItemsMax = 128;

class MSQLInsert
{
public:
	char tablename[1024];
	MSQLInsertItem * items;
	unsigned int itemsn;

	inline MSQLInsert()
	{
		items = new MSQLInsertItem[MSQLInsertItemsMax];
		Reset();
	}

	inline MSQLInsert(const char * tablename)
	{
		items = new MSQLInsertItem[MSQLInsertItemsMax];
		Reset();
		SetTableName(tablename);
	}

	inline ~MSQLInsert()
	{
		delete[] items;
	}

	inline void Reset()
	{
		itemsn = 0;
	}

	inline void SetTableName(const char * tablename)
	{
		strcpy(this->tablename, tablename);
	}

	inline void Add(const char * name, const char * value, const bool quotes = true)
	{
		MSQLInsertItem * item = items + itemsn;
		strcpy(item->name, name);
		strcpy(item->value, value);
		item->quotes = quotes;
		itemsn++;
	}

	inline void Add(const char * name, const int value)
	{
		char s[32];
		sprintf(s, "%i", value);
		Add(name, s, false);
	}

	inline void Add(const char * name, const unsigned int value)
	{
		char s[32];
		sprintf(s, "%u", value);
		Add(name, s, false);
	}

	inline void Add(const char * name, const long long value)
	{
		char s[64];
		sprintf(s, "%lli", value);
		Add(name, s, false);
	}

	inline void Add(const char * name, const unsigned long long value)
	{
		char s[64];
		sprintf(s, "%llu", value);
		Add(name, s, false);
	}

	inline void Add(const char * name, const float value)
	{
		char s[64];
		sprintf(s, "%f", value);
		Add(name, s, false);
	}

	inline void Add(const char * name, const double value)
	{
		char s[64];
		sprintf(s, "%lf", value);
		Add(name, s, false);
	}

	inline void Add(const char * name, const bool value)
	{
		Add(name, value ? "1" : "0", false);
	}
};
