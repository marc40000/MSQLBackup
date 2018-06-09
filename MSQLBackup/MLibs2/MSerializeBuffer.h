#pragma once

#include "MArrayE.h"
#include "MSerialize.h"
#ifdef WIN32
#include <TCHAR.h>
#include <cstdint>
#endif
#include <stdio.h>
#include <iostream>

class MSerializeBuffer
{
public:
	inline MSerializeBuffer() : p(1024)
	{
	}
	inline MSerializeBuffer(const unsigned int size) : p(size)
	{
	}
	virtual ~MSerializeBuffer()
	{
	}

	MArrayE < unsigned char > p;
	void * userdata;

	inline unsigned char * GetPointer() const
	{
		return p.p;
	}
	inline unsigned char * GetPointerCurrent() const
	{
		return p.p + p.GetN();
	}
	inline unsigned int GetN() const
	{
		return p.GetN();
	}

	inline void Reset()
	{
		p.SetN(0);
		p.SetSize(1024);
	}

#ifdef MPLATFORM_WIN32_MSVC
	inline int SaveToFile(const TCHAR * filename)
	{
		FILE * f;
		_tfopen_s(&f, filename, _T("wb"));
		if (f == 0)
		{
			return 1;
		}
		fwrite(GetPointer(), 1, GetN(), f);
		fclose(f);
		return 0;
	}
#endif
	inline int SaveToFile(const char * filename)
	{
		FILE * f;
		f = fopen(filename, "wb");
		if (f == 0)
		{
			return 1;
		}
		fwrite(GetPointer(), 1, GetN(), f);
		fclose(f);
		return 0;
	}

	//////////////////////////////////////////////////////////////////////

	inline void Do(const unsigned char * v, const unsigned int n)
	{
		p.AddBack(v, n);
	}

	inline void Skip(const unsigned int n)
	{
		const unsigned char t = 0;
		unsigned int i;
		for (i = 0; i < n; ++i)
		{
			p.AddBack(&t, 1);
		}
	}

	inline void operator<< (const char v)
	{
		p.AddBack((unsigned char*)&v, sizeof(v));
	}

	inline void operator<< (const unsigned char v)
	{
		p.AddBack((unsigned char*)&v, sizeof(v));
	}

	inline void operator<< (const short int v)
	{
		p.AddBack((unsigned char*)&v, sizeof(v));
	}

	inline void operator<< (const unsigned short int v)
	{
		p.AddBack((unsigned char*)&v, sizeof(v));
	}

	inline void operator<< (const int v)
	{
		p.AddBack((unsigned char*)&v, sizeof(v));
	}

	inline void operator<< (const unsigned int v)
	{
		p.AddBack((unsigned char*)&v, sizeof(v));
	}

	inline void operator<< (const long long v)
	{
		p.AddBack((unsigned char*)&v, sizeof(v));
	}

	inline void operator<< (const unsigned long long v)
	{
		p.AddBack((unsigned char*)&v, sizeof(v));
	}

	inline void operator<< (const float v)
	{
		p.AddBack((unsigned char*)&v, sizeof(v));
	}

	inline void operator<< (const double v)
	{
		p.AddBack((unsigned char*)&v, sizeof(v));
	}

	/*inline void operator<< (const bool v)
	{
		p.AddBack((unsigned char*)&v, sizeof(v));
	}*/

	inline void operator<< (const bool v)
	{
		int t = v ? -1 : 0;
		(*this) << t;
	}

	inline void boolaschar (const bool v)
	{
		char t = v ? -1 : 0;
		(*this) << t;
	}

	// Note: I changed this from int to unsigned char on 20160216
	/*inline void operator<< (const bool v)
	{
		unsigned char t = v ? -1 : 0;
		p.AddBack((unsigned char*)&t, sizeof(unsigned char));
	}*/

	/*inline void operator<< (const wchar_t * v)
	{
		unsigned int n = wcslen(v);
		p.AddBack((unsigned char*)v, sizeof(wchar_t) * n);
	}

	inline void operator<< (const char * v)
	{
		unsigned int n = strlen(v);
		p.AddBack((unsigned char*)v, sizeof(char) * n);
	}

	inline void operator<< (const TCHAR * v)
	{
		unsigned int n = _tcslen(v);
		p.AddBack((unsigned char*)v, sizeof(TCHAR) * n);
	}*/

	inline void string (const char * v)
	{
		unsigned int n = strlen(v);
		p.AddBack((unsigned char*)v, sizeof(char) * (n + 1));
	}

	inline void stringalign4 (const char * v)
	{
		unsigned int n = strlen(v);
		p.AddBack((unsigned char*)v, sizeof(char) * (n + 1));
		
		unsigned int skipn = (4 - ((n + 1) % 4)) % 4;
		Skip(skipn);
	}

#ifdef MPLATFORM_WIN32
	inline void tstring (const wchar_t * v)
	{
		unsigned int n = wcslen(v);
		p.AddBack((unsigned char*)v, sizeof(wchar_t) * (n + 1));
	}

	inline void tstringcompact (const wchar_t * v)
	{
		unsigned int n = wcslen(v);
		char t[1024];
		wcstombs(t, v, 1024);
		p.AddBack((unsigned char*)t, sizeof(char) * (n + 1));
	}
#endif

	static inline const unsigned int mstrlenu16(const char16_t * v)
	{
		unsigned int n = 0;
		const char16_t * p = v;
		while ((*p) != 0)
		{
			n++;
		}
		return n;
	}

	inline void stringu16(const char16_t * v)
	{
		unsigned int n = mstrlenu16(v);
		p.AddBack((unsigned char*)v, sizeof(char16_t) * (n + 1));
	}


	inline void operator<< (MSerialize & s)
	{
		s.Serialize(*this);
	}
	//inline void operator<< (MSerialize * s)
	//{
	//	s->Serialize(*this);
	//}


	// int8_t is defined as signed char which c++ handels differntly from char. So we have to add a method for this
	inline void operator<< (const int8_t v)
	{
		p.AddBack((unsigned char*)&v, sizeof(v));
	}



	//////////////////////////////////////////////////////////////////
	// special, not lossless, encodings

	inline void FloatAs8Bit(const float v, const float v0, const float vd)
	{
		int vv = (int)(((v - v0) / vd) * 256.0f);
		if (vv < 0)
		{
			vv = 0;
		}
		else
		if (vv > 255)
		{
			vv = 255;
		}
		unsigned char c = vv;
		(*this) << c;
	}

	inline void FloatAs16Bit(const float v, const float v0, const float vd)
	{
		int vv = (int)(((v - v0) / vd) * 65536.0f);
		if (vv < 0)
		{
			vv = 0;
		}
		else
		if (vv > 65535)
		{
			vv = 65535;
		}
		unsigned short int s = vv;
		(*this) << s;
	}



	inline unsigned int Hash32Content()
	{
		unsigned int i;
		unsigned int hash = 0;
		unsigned int n = GetN();

		// hash the state itself
		for (i = 0; i < n / 4; ++i)
		{
			hash ^= ((unsigned int*)p.p)[i];
		}
		for (i = (n / 4) * 4; i < n; ++i)
		{
			hash ^= ((unsigned char*)p.p)[i];
		}

		return hash;
	}
};
