#pragma once

#include "MArrayE.h"
#include "MSerialize.h"
#include "MSerializeBuffer.h"
#include "MFile.h"
#include "MAssert.h"
//#include "MProfile.h"
#if (! (defined(MPLATFORM_EMSCRIPTEN)))
#if (defined(MPLATFORM_LINUX_QNAP) || defined(MPLATFORM_CLR))
#else
//#include <boost/chrono.hpp>
//#include <boost/thread.hpp>
#include <chrono>
#include <thread>
#endif
#endif

extern unsigned int MUnserializeBuffer_serversion;

const float mserepsilon = 0.0001f;

class MUnserializeBuffer
{
public:
	inline MUnserializeBuffer(const MSerializeBuffer & serbuf)
	{
		p = serbuf.GetPointer();
		n = serbuf.GetN();
		pused = 0;
		powned = false;
		version = MUnserializeBuffer_serversion;
	}
	inline MUnserializeBuffer(const MSerializeBuffer * serbuf)
	{
		p = serbuf->GetPointer();
		n = serbuf->GetN();
		pused = 0;
		powned = false;
		version = MUnserializeBuffer_serversion;
	}
	inline MUnserializeBuffer(unsigned char * p, unsigned int n = -1)
	{
		this->p = p;
		this->n = n;
		pused = 0;
		powned = false;
		version = MUnserializeBuffer_serversion;
	}
#ifdef MPLATFORM_WIN32_MSVC
	inline MUnserializeBuffer(const TCHAR * filename)		// only works for files < 4gb
	{
		//MProfileDummy mprofile;
		unsigned long long filesize;
		//mprofile.Start(_T("MGetFileSize"));
		if (0 != MGetFileSize(filename, &filesize))
		{
			//mprofile.End();
			//MAssert(false);
			p = 0;
			n = 0;
			pused = 0;
			powned = false;
		}
		else
		{
			//mprofile.End();
			MAssert(filesize < 0xffffffff);

			//mprofile.Start(_T("alloc"));
			n = (unsigned int)filesize;
			p = new unsigned char[n];
			FILE * f = 0;
			//mprofile.End();
			//mprofile.Start(_T("fopenloop"));
			while (f == 0)
			{
				_tfopen_s(&f, filename, _T("rb"));
				if (f != 0)
				{
					break;
				}
				MAssert(f != 0);
				if (!MFileExistsFindFirst(filename))
				{
					MAssert(false);
					p = 0;
					n = 0;
					pused = 0;
					powned = false;
					return;
				}
				Sleep(1);
			}
			//mprofile.End();
			//mprofile.Start(_T("fread"));
			fread(p, 1, n, f);
			//mprofile.End();
			//mprofile.Start(_T("fclose"));
			fclose(f);
			//mprofile.End();
			pused = 0;
			powned = true;
		}
		version = MUnserializeBuffer_serversion;
	}
#endif
#if (! (defined(MPLATFORM_EMSCRIPTEN)))
	inline MUnserializeBuffer(const char * filename)		// only works for files < 4gb
	{
		//MProfileDummy mprofile;
		unsigned long long filesize;
		//mprofile.Start(_T("MGetFileSize"));
		if (0 != MGetFileSizeA(filename, &filesize))
		{
			//mprofile.End();
			//MAssert(false);
			p = 0;
			n = 0;
			pused = 0;
			powned = false;
		}
		else
		{
			//mprofile.End();
			MAssert(filesize < 0xffffffff);

			//mprofile.Start(_T("alloc"));
			n = (unsigned int)filesize;
			p = new unsigned char[n];
			FILE * f = 0;
			//mprofile.End();
			//mprofile.Start(_T("fopenloop"));
			while (f == 0)
			{
				f = fopen(filename, "rb");
				if (f != 0)
				{
					break;
				}
				MAssert(f != 0);
				if (!MFileExists(filename))
				{
					MAssert(false);
					p = 0;
					n = 0;
					pused = 0;
					powned = false;
					return;
				}

				//Sleep(1);
				//std::this_thread::sleep_for(std::chrono::milliseconds(1));		// c++11
#if (defined(MPLATFORM_LINUX_QNAP))
				usleep(1);
#else
#if (defined(MPLATFORM_CLR))
				Sleep(1);
#else
				//boost::this_thread::sleep_for(boost::chrono::milliseconds(1));
				std::this_thread::sleep_for(std::chrono::milliseconds(1));
#endif
#endif
			}
			//mprofile.End();
			//mprofile.Start(_T("fread"));
			fread(p, 1, n, f);
			//mprofile.End();
			//mprofile.Start(_T("fclose"));
			fclose(f);
			//mprofile.End();
			pused = 0;
			powned = true;
		}
		version = MUnserializeBuffer_serversion;
	}
#endif
#ifdef MPLATFORM_EMSCRIPTEN
	inline MUnserializeBuffer(const char * filename)		// only works for files < 4gb
	{
		unsigned long long filesize;
		if (0 != MGetFileSizeA(filename, &filesize))
		{
			p = 0;
			n = 0;
			pused = 0;
			powned = false;
		}
		else
		{
			n = (unsigned int)filesize;
			p = new unsigned char[n];
			FILE * f = 0;
			f = fopen(filename, "rb");
			if (f != 0)
			{
				fread(p, 1, n, f);
				fclose(f);
			}
			pused = 0;
			powned = true;
		}
		version = MUnserializeBuffer_serversion;
	}
#endif

	virtual ~MUnserializeBuffer()
	{
		MAssert(Check());
		if (powned)
		{
			delete [] p;
		}
	}

private:
	unsigned char * p;
	unsigned int n;
	unsigned pused;
	bool powned;

public:
	unsigned int version;
	void * userdata;

	inline void Reset()		// aka Rewind
	{
		pused = 0;
	}

	inline unsigned char * GetPointer() const
	{
		return p;
	}
	inline unsigned char * GetPointerCurrent() const
	{
		//MAssert(pused < n);
		if (pused >= n)
		{
			//int a = 3;
		}
		return p + pused;
	}
	inline const unsigned int GetN() const
	{
		return n;
	}
	inline const unsigned int GetUsed() const
	{
		return pused;
	}
	inline const unsigned int GetLeft() const
	{
		return n - pused;
	}
	inline const bool IsValid() const
	{
		return p != 0;
	}

	inline const bool Check() const
	{
		if (pused > n)
		{
			// something is wrong
			return false;
		}
		return true;
	}



	//////////////////////////////////////////////////////////////////////

	inline void Do(unsigned char * v, const unsigned int n)
	{
		//MAssert(pused + n <= this->n);
		memcpy(v, GetPointerCurrent(), n);
		pused += n;
	}

	inline void Skip(const unsigned int n)
	{
		//MAssert(pused + n <= this->n);
		pused += n;
	}

	inline void operator<< (char & v)
	{
		//MAssert(pused + sizeof(char) <= this->n);
		v = *((char*)(GetPointerCurrent()));
		pused += sizeof(char);
	}

	inline void operator<< (unsigned char & v)
	{
		//MAssert(pused + sizeof(unsigned char) <= this->n);
		v = *((unsigned char*)(GetPointerCurrent()));
		pused += sizeof(unsigned char);
	}

	inline void operator<< (short int & v)
	{
		//MAssert(pused + sizeof(short int) <= this->n);
		v = *((memscripten_align1_short*)(GetPointerCurrent()));
		pused += sizeof(short int);
	}

	inline void operator<< (unsigned short int & v)
	{
		//MAssert(pused + sizeof(unsigned short int) <= this->n);
		v = *((memscripten_align1_ushort*)(GetPointerCurrent()));
		pused += sizeof(unsigned short int);
	}

	inline void operator<< (int & v)
	{
		//MAssert(pused + sizeof(int) <= this->n);
		v = *((memscripten_align1_int*)(GetPointerCurrent()));
		pused += sizeof(int);
	}

	inline void operator<< (unsigned int & v)
	{
		//MAssert(pused + sizeof(unsigned int) <= this->n);
		v = *((memscripten_align1_uint*)(GetPointerCurrent()));
		pused += sizeof(unsigned int);
	}

	inline void operator<< (long long & v)
	{
		//MAssert(pused + sizeof(long long) <= this->n);
		v = *((memscripten_align1_int64*)(GetPointerCurrent()));
		pused += sizeof(long long);
	}

	inline void operator<< (unsigned long long & v)
	{
		//MAssert(pused + sizeof(unsigned long long) <= this->n);
		v = *((memscripten_align1_uint64*)(GetPointerCurrent()));
		pused += sizeof(unsigned long long);
	}

	inline void operator<< (float & v)
	{
		//MAssert(pused + sizeof(float) <= this->n);
		v = *((memscripten_align1_float*)(GetPointerCurrent()));
		pused += sizeof(float);
	}

	inline void operator<< (double & v)
	{
		//MAssert(pused + sizeof(double) <= this->n);
		v = *((memscripten_align1_double*)(GetPointerCurrent()));
		pused += sizeof(double);
	}
	
	/*inline void operator<< (bool & v)
	{
		//MAssert(pused + sizeof(bool) <= this->n);
		v = *((bool*)(GetPointerCurrent()));
		pused += sizeof(bool);
	}*/

	inline void operator<< (bool & v)
	{
		//MAssert(pused + sizeof(bool) <= this->n);
		int t;
		(*this) << t;
		v = (t != 0);
	}

	inline void boolaschar (bool & v)
	{
		//MAssert(pused + sizeof(bool) <= this->n);
		char t;
		(*this) << t;
		v = (t != 0);
	}

	/*inline void operator<< (wchar_t * v)
	{
		unsigned int n = wcslen((wchar_t *)GetPointerCurrent());
		wcscpy(v, (wchar_t *)GetPointerCurrent());
		pused += sizeof(wchar_t) * n;
	}

	inline void operator<< (char * v)
	{
		unsigned int n = strlen((char *)GetPointerCurrent());
		strcpy(v, (char *)GetPointerCurrent());
		pused += sizeof(char) * n;
	}

	inline void operator<< (TCHAR * v)
	{
		unsigned int n = _tcslen((TCHAR *)GetPointerCurrent());
		_tcscpy(v, (TCHAR *)GetPointer());
		pused += sizeof(TCHAR) * n;
	}*/

	inline void string (char * v)
	{
		unsigned int n = strlen((char *)GetPointerCurrent());
		//MAssert(pused + sizeof(TCHAR) * (n + 1) <= this->n);
		strcpy(v, (char *)GetPointerCurrent());
		pused += sizeof(char) * (n + 1);
	}

	inline void stringalign4 (char * v)
	{
		unsigned int n = strlen((char *)GetPointerCurrent());
		//MAssert(pused + sizeof(TCHAR) * (n + 1) <= this->n);
		strcpy(v, (char *)GetPointerCurrent());
		pused += sizeof(char) * (n + 1);
		
		unsigned int skipn = (4 - ((n + 1) % 4)) % 4;
		Skip(skipn);
	}

#ifdef WIN32
	inline void tstring (wchar_t * v)
	{
		unsigned int n = wcslen((wchar_t *)GetPointerCurrent());
		//MAssert(pused + sizeof(TCHAR) * (n + 1) <= this->n);
		wcscpy(v, (wchar_t *)GetPointerCurrent());
		pused += sizeof(wchar_t) * (n + 1);
	}

	inline void tstringcompact (wchar_t * v)
	{
		unsigned int n = strlen((char *)GetPointerCurrent());
		//MAssert(pused + sizeof(TCHAR) * (n + 1) <= this->n);
		mbstowcs(v, (char *)GetPointerCurrent(), 0xfffffff);
		pused += sizeof(char) * (n + 1);
	}
#endif

	inline void operator<< (MSerialize & s)
	{
		// not optimal, but size of s is actually unknown at this time.
		// Anyways, it's call the elementary methods from above and we should notice any anormaly
		//MAssert(pused <= this->n);

		s.Unserialize(*this);
	}
	//inline void operator<< (MSerialize * s)
	//{
	//	s->Unserialize(*this);
	//}


	// int8_t is defined as signed char which c++ handels differntly from char. So we have to add a method for this
	inline void operator<< (int8_t & v)
	{
		//MAssert(pused + sizeof(char) <= this->n);
		v = *((int8_t*)(GetPointerCurrent()));
		pused += sizeof(int8_t);
	}

	
	//////////////////////////////////////////////////////////////////
	// special, not lossless, encodings

	inline void FloatAs8Bit(float &v, const float v0, const float vd)
	{
		unsigned char c;
		(*this) << c;
		float vv = c;
		v = v0 + (vv / 256.0f) * vd;
		if (v < v0)
		{
			v = v0;
		}
		else
		//if (v >= v0 + vd)
		if (v > v0 + vd - mserepsilon)
		{
			v = v0 + vd - mserepsilon;
		}
	}

	inline void FloatAs16Bit(float &v, const float v0, const float vd)
	{
		unsigned short int s;
		(*this) << s;
		float vv = s;
		v = v0 + (vv / 65536.0f) * vd;
		if (v < v0)
		{
			v = v0;
		}
		else
		//if (v >= v0 + vd)
		if (v > v0 + vd - mserepsilon)
		{
			v = v0 + vd - mserepsilon;
		}
	}
};
