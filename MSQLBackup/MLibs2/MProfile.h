#pragma once

#include "MTime.h"

class MProfile
{
public:
	char name[64];
	unsigned long long t0;
	unsigned long long t1;
	unsigned long long td;
	unsigned long long tbetweenlast;
	
	inline const unsigned long long GetDelta()
	{
		t1 = MTimeYS();
		return t1 - t0;
	}

	inline void Start(const char * name)
	{
		strcpy(this->name, name);
		printf("MProfile Start: %s\n", this->name);
		t0 = MTimeYS();
		tbetweenlast = t0;
	}

	inline void Between()
	{
		t1 = MTimeYS();
		td = t1 - t0;
		unsigned long long tdbetweenlast = t1 - tbetweenlast;
		printf("MProfile Between: %s    took %llu ys    since last time %llu ys\n", this->name, td, tdbetweenlast);
		tbetweenlast = t1;
	}

	inline void End()
	{
		t1 = MTimeYS();
		td = t1 - t0;
		printf("MProfile End: %s    took %llu ys\n", this->name, td);
	}
};

class MProfileDummy
{
public:
	inline void Start(const char * name) const
	{
	}

	inline void Between() const
	{
	}

	inline void End() const
	{
	}
};
