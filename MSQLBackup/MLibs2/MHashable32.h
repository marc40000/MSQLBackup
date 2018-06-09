#pragma once

class MHashable32
{
public:
	inline MHashable32()
	{
	}
	virtual ~MHashable32()
	{
	}
	virtual const unsigned int Hash32() const = 0;
};



// full specialized template for MHashable32Helper
// suggested by http://stackoverflow.com/questions/16779296/template-specialization-of-a-single-method-from-templated-class-with-multiple-te
// that way, you can create a MHashable32Helper and always Hash your object correctly, no matter if it is a class that has a Hash32() method or
// if it is an int, char etc. This is important if you don't want your T to have virtual methods. Otherwise, you could let them inherit from MHashable32

template < class T > class MHashable32Helper
{
public:
	inline const unsigned int HashFunction32(const T & v) const
	{
		return v.Hash32();
	}
};

#ifdef MPLATFORM_WIN32
template < > class MHashable32Helper < LPVOID >
{
public:
	inline const unsigned int HashFunction32(const LPVOID & v) const
	{
		if (sizeof(LPVOID) == 4)
		{
			// 32 bit system
			return (unsigned int)v;
		}
		else
		{
			// 64 bit system
			return (((unsigned int)v) ^ (((unsigned long long)v) >> 32));
		}
	}
};
#endif

template < > class MHashable32Helper < char >
{
public:
	inline const unsigned int HashFunction32(const char & v) const
	{
		return (unsigned int)v;
	}
};

template < > class MHashable32Helper < unsigned char >
{
public:
	inline const unsigned int HashFunction32(const unsigned char & v) const
	{
		return (unsigned int)v;
	}
};

template < > class MHashable32Helper < short int >
{
public:
	inline const unsigned int HashFunction32(const short int & v) const
	{
		return (unsigned int)v;
	}
};

template < > class MHashable32Helper < unsigned short int >
{
public:
	inline const unsigned int HashFunction32(const unsigned short int & v) const
	{
		return (unsigned int)v;
	}
};

template < > class MHashable32Helper < int >
{
public:
	inline const unsigned int HashFunction32(const int & v) const
	{
		return (unsigned int)v;
	}
};

template < > class MHashable32Helper < unsigned int >
{
public:
	inline const unsigned int HashFunction32(const unsigned int & v) const
	{
		return v;
	}
};

template < > class MHashable32Helper < long long >
{
public:
	inline const unsigned int HashFunction32(const long long & v) const
	{
		return (unsigned int)(v ^ (v >> 32));
	}
};

template < > class MHashable32Helper < unsigned long long >
{
public:
	inline const unsigned int HashFunction32(const unsigned long long & v) const
	{
		return (unsigned int)(v ^ (v >> 32));
	}
};

template < > class MHashable32Helper < float >
{
public:
	inline const unsigned int HashFunction32(const float & v) const
	{
		return *((unsigned int*)(&v));
	}
};

template < > class MHashable32Helper < double >
{
public:
	inline const unsigned int HashFunction32(const double & v) const
	{
		return (((unsigned int*)(&v))[0]) ^ (((unsigned int*)(&v))[1]);
	}
};


template < > class MHashable32Helper < long >
{
public:
	inline const unsigned int HashFunction32(const long & v) const
	{
		return (unsigned int)v;
	}
};

template < > class MHashable32Helper < unsigned long >
{
public:
	inline const unsigned int HashFunction32(const unsigned long & v) const
	{
		return (unsigned int)v;
	}
};




/*	template < class KEY, class TYPE > inline const unsigned int MHashE < KEY, TYPE >::HashFunction32 (const KEY & key) const
	{
		return key.Hash32();
	}

	template <> inline const unsigned int MHashE < LPVOID, int >::HashFunction32 (const LPVOID & key) const
	{
		return (unsigned int)key;
	}

	template < class TYPE > inline const unsigned int MHashE < LPVOID, TYPE >::HashFunction32 (const LPVOID & key) const
	{
		return (unsigned int)key;
	}
	template <> inline const unsigned int HashFunction32 < char > (const char key) const
	{
		return (unsigned int)key;
	}
	template <> inline const unsigned int HashFunction32 < unsigned char > (const unsigned char key) const
	{
		return (unsigned int)key;
	}
	template <> inline const unsigned int HashFunction32 < short int > (const short int key) const
	{
		return (unsigned int)key;
	}
	template <> inline const unsigned int HashFunction32 < unsigned short int > (const unsigned short int key) const
	{
		return (unsigned int)key;
	}
	template <> inline const unsigned int HashFunction32 < int > (const int key) const
	{
		return (unsigned int)key;
	}
	template <> inline const unsigned int HashFunction32 < unsigned int > (const unsigned int key) const
	{
		return (unsigned int)key;
	}
	template <> inline const unsigned int HashFunction32 < long long > (const long long key) const
	{
		return (unsigned int)(key ^ (key >> 32));
	}
	template <> inline const unsigned int HashFunction32 < unsigned long long > (const unsigned long long key) const
	{
		return (unsigned int)(key ^ (key >> 32));
	}
	template <> inline const unsigned int HashFunction32 < float > (const float key) const
	{
		return *((unsigned int*)(&key));
	}
	template <> inline const unsigned int HashFunction32 < double > (const double key) const
	{
		return (((unsigned int*)(&key))[0]) ^ (((unsigned int*)(&key))[1]);
	}
	inline const unsigned int HashFunction32 < MHashable32 > (const MHashable32 & key) const
	{
		return key.Hash32();
	}*/


