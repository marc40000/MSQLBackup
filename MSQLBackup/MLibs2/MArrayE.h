/*
	MArrayE Template Class

	(c) 2008 Marc Rochel

	various MArray versions:

		0. reference, no serial, all equal									MArrayE
		1. reference, flat serialization; just memcpy, all equal			MArrayEF
		2. reference, serial, all equal										MArrayES
		3. pointer, serial with builder, maybe different					MArrayPS
		4. pointer, serial, all equal										MArrayPES	* not implemented, use 3 with a MSimpleSerializeBuilder

		- pointer, no serial, maybe different => 0 can handle that
		- pointer, no serial, all equal => 0 can handle that


	Notes:
	- TYPE can be anything, pointers or objects itself.
	- Use SetN() and GetN() to set the size.
	- Use AddBack() to add elements to the back. It increases itself efficiently.
	- It shrinks itself efficiently as well when it is accessed a few times and less memory would be sufficient
	- Constructors and Destructors of the Elements don't get called. If you need that, declare init/destroy methods
	  and call them yourself or use the MArrayPS and call the constructor/destructor yourself. This is because malloc is used.


	MArray can be used as a Stack or a Queue:

	// Stack functionality
	// Push: AddBack
	// Top:  GetBack
	// Pop:  DecN

	// Queue functionality
	// Push: AddBack,   AddFront
	// Top:  GetFront,  GetBack
	// Pop:	 Remove(0), DecN

	Note that Stack is theoretically optimal while Queue isn't. Theoretically, a queue has to be implemented as a linked
	list. But practically, queues are short or may need to get elements removed or added in the middle, like build queues of units
	for games. If they are short, practically an array will be faster than a linked list. If elements can be removed or added in 
	the middle, the linked list queue gets a O(n) as well, so theoretically it's equal to this implementation then.
	So practically, this queue implementation is optimal as well.

*/

#ifndef _MARRAYE_
#define _MARRAYE_

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif
#include <stdlib.h>
#include <memory.h>
#include "MAssert.h"

// If defined, malloc is user instead of new. This has consequences on calling of the default constructor and destructor or not.
//#define MARRAYEUSEMALLOC


// Templates used for Sorting
template <class TYPE> class MArrayElementCompare
{
public:
	virtual bool IsSmaller(TYPE * a, TYPE * b) = 0;			// Note: For my QuickSort Implementation, this actually has to be IsSmaller. A test with itself has to return false!
	inline bool IsGreaterOrEqual(TYPE * a, TYPE * b)
	{
		return !IsSmaller(a, b);
	}
	inline bool IsGreater(TYPE * a, TYPE * b)
	{
		return IsSmaller(b, a);
	}
	inline bool IsSmallerOrEqual(TYPE * a, TYPE * b)
	{
		return !IsSmaller(b, a);
	}
};

template <class TYPE> class MArrayElementCompareSimple : public MArrayElementCompare < TYPE >
{
public:
	virtual bool IsSmaller(TYPE * a, TYPE * b)
	{
		return (*a) < (*b);
	}
};
typedef MArrayElementCompareSimple < int > MArrayElementCompareSimpleInt;
typedef MArrayElementCompareSimple < unsigned int > MArrayElementCompareSimpleUnsignedInt;
typedef MArrayElementCompareSimple < long long > MArrayElementCompareSimpleLongLong;
typedef MArrayElementCompareSimple < unsigned long long > MArrayElementCompareSimpleUnsignedLongLong;
typedef MArrayElementCompareSimple < float > MArrayElementCompareSimpleFloat;
typedef MArrayElementCompareSimple < double > MArrayElementCompareSimpleDouble;


template <class TYPE> class MArrayE
{
public:
	TYPE * p;
	unsigned int n;		// size of the array for the user
	unsigned int size;	// size of the underlying allocated structure
	
	unsigned int shrinkcount;
	unsigned int shrinkcountlimit;

	inline MArrayE()
	{
		n=0;
		size=1;
#ifdef MARRAYEUSEMALLOC
		p=(TYPE*)malloc(size*sizeof(TYPE));
#else
		p = new TYPE[size];
#endif
		shrinkcount = 0;
		shrinkcountlimit = 16;
	}
	inline MArrayE(const unsigned int size)
	{
		n=0;
		this->size=size;
#ifdef MARRAYEUSEMALLOC
		p=(TYPE*)malloc(size*sizeof(TYPE));
#else
		p = new TYPE[size];
#endif
		shrinkcount = 0;
		shrinkcountlimit = 16;
	}
	virtual ~MArrayE()
	{
#ifdef MARRAYEUSEMALLOC
		free(p);
#else
		delete [] p;
#endif
	}

	inline void SetShrinkCountLimit(const unsigned int shrinkcountlimitnew)
	{
		shrinkcountlimit = shrinkcountlimitnew;
	}
	inline void GetShrinkCountLimit() const
	{
		return shrinkcountlimit;
	}

// internal
	inline void SetSize(const unsigned int newsize)
	{
		if (newsize!=0)
		{
#ifdef MARRAYEUSEMALLOC
			p=(TYPE*)realloc(p,newsize*sizeof(TYPE));
#else
			TYPE * newp = new TYPE[newsize];
			// don't use memcpy here but the = operator instead
			unsigned int i, m = n < newsize ? n : newsize;
			for (i = 0; i < m; ++i)
			{
				newp[i] = p[i];			
			}
			delete [] p;
			p = newp;
#endif
			size=newsize;
		}
		else
		{
			// force size to 1
#ifdef MARRAYEUSEMALLOC
			p=(TYPE*)realloc(p,sizeof(TYPE));
#else
			delete [] p;
			p = new TYPE[1];
			// don't use memcpy here but the = operator instead
			//if (n != 0)
			//{
			//	newp[0] = p[0];
			//}
#endif
			size=1;
		}
		if (n>=size)
		{
			n=size;
		}
	}
// internal
	// Note: SetSizeDestructive() is a bit faster than SetSize(), but the elements are uninitialized instead of copied
	inline void SetSizeDestructive(const unsigned int newsize)
	{
		if (newsize!=0)
		{
#ifdef MARRAYEUSEMALLOC
			p=(TYPE*)realloc(p,newsize*sizeof(TYPE));
#else
			delete [] p;
			p = new TYPE[newsize];
#endif
			size=newsize;
		}
		else
		{
			// force size to 1
#ifdef MARRAYEUSEMALLOC
			p=(TYPE*)realloc(p,sizeof(TYPE));
#else
			// don't use memcpy here but the = operator instead
			delete [] p;
			//TYPE * newp = new TYPE[1];
			p = new TYPE[1];
#endif
			size=1;
		}
		if (n>=size)
		{
			n=size;
		}
	}

	inline const unsigned int GetSize() const
	{
		return size;
	}

	inline void RemoveAll()
	{
		SetN(0);
	}

	inline void RemoveFront()
	{
		Remove(0);
	}
	
	inline void RemoveBack()
	{
		DecN();
	}

	inline void DeleteAll()
	{
		unsigned int i;
		for (i = 0; i < GetN(); ++i)
		{
			delete ((*this)[i]);
		}
	}

	inline void DeleteIfNotNullAll()
	{
		unsigned int i;
		for (i = 0; i < GetN(); ++i)
		{
			if (((*this)[i]) != 0)
			{
				delete ((*this)[i]);
			}
		}
	}

	inline void DeleteAndRemoveAll()
	{
		DeleteAll();
		RemoveAll();
	}

	inline void DeleteIfNotNullAndRemoveAll()
	{
		DeleteIfNotNullAll();
		RemoveAll();
	}

// internal helper functions
	inline void SetDoubleSize()
	{
		SetSize(size * 2);
	}
	inline void SetHalfSize()
	{
		SetSize(size / 2);
	}
	inline void IncN()
	{
		if (n >= size)
		{
			SetDoubleSize();
			shrinkcount = 0;
		}
		n++;

		Optimize();
	}
	inline void DecN()
	{
		n--;

		Optimize();
	}
	/*inline void Optimize()
	{
		if (size / 2 > 0)
		{
			if (shrinkcount == shrinkcountlimit)
			{
				SetHalfSize();
				shrinkcount = 0;
			}
			if (n < size / 2)
			{
				++shrinkcount;
			}
		}
	}*/
	inline void Optimize()		// I changed this on 2012-06-27. I think the previous version has a bug that is occuring very not often. Besides, this is a little bit faster as well.
	{
		if (size / 2 > 0)
		{
			if (n <= size / 2)
			{
				++shrinkcount;

				if (shrinkcount == shrinkcountlimit)
				{
					SetHalfSize();
					shrinkcount = 0;
				}
			}
		}
	}
	inline void ShiftR1(const unsigned int pos)
	{
		IncN();
		unsigned int i;
		for (i = n - 1; i > pos; i--)
		{
			p[i] = p[i - 1];
		}
	}
	inline void ShiftL1(const unsigned int pos)
	{
		unsigned int i;
		for (i = pos; i < n - 1; i++)
		{
			p[i] = p[i + 1];
		}
		DecN();
	}
	inline void ShiftRNNoResize(const unsigned int pos, const unsigned int amount)
	{
		unsigned int i;
		for (i = n - 1; i >= pos + amount; i--)
		{
			p[i] = p[i - amount];
		}
	}
	inline void ShiftLNNoResize(const unsigned int pos, const unsigned int amount)
	{
		unsigned int i;
		for (i = pos; i < n - amount; i++)
		{
			p[i] = p[i + amount];
		}
	}

// user interface
	inline void AddBack(const TYPE & v)
	{
		IncN();
		p[n-1]=v;
		//Optimize();	commented out in 2011-02-17, IncN already calls Optimize()
	}
	// adds multiple flat elements, usefull for MArrays of unsigned char etc.
	inline void AddBack(const TYPE * v, const unsigned int vn)
	{
		while (n + vn > size)
		{
			SetDoubleSize();
			shrinkcount = 0;
		}
#ifdef MARRAYEUSEMALLOC
		memcpy(p + n, v, vn * sizeof(TYPE));
#else
		// use = operator instead of memcpy
		unsigned int i;
		for (i = 0; i < vn; ++i)
		{
			p[n + i] = v[i];
		}
#endif
		n += vn;
	}
	inline void AddBack(const MArrayE < TYPE > & v)
	{
		AddBack(v.GetPointer(), v.GetN());
	}
	inline void AddFront(const TYPE & v)
	{
		ShiftR1(0);
		p[0]=v;
	}
	inline void AddAt(const unsigned int pos, const TYPE & v)
	{
		MAssert(pos <= n);
		ShiftR1(pos);
		p[pos]=v;
	}
	inline void SetAt(const unsigned int pos, const TYPE & v)
	{
		p[pos]=v;
	}
	inline TYPE& operator[](const unsigned int pos)
	{
		//MAssert(pos < n);
		//if (pos >= n)
		//{
		//	int a = 3;
		//}
		return p[pos];
	}
	inline const TYPE& operator[](const unsigned int pos) const
	{
		//MAssert(pos < n);
		return p[pos];
	}
	inline void Remove(const unsigned int pos)
	{
		ShiftL1(pos);
	}
	inline TYPE * GetPointer() const
	{
		return p;
	}
	inline const unsigned int GetN() const
	{
		return n;
	}
	inline void SetN(const unsigned int newn)
	{
/*		SetSize(newn);
		n=newn;*/
		if (newn>size)
		{
			SetSize(newn);
			shrinkcount = 0;
		}
		n=newn;
	}
	// Note: There is DeleteAll which calls delete on every entry, this does not do that.
	//       If there are pointers in the array, the data they point to is still there afterwards!
	inline void SetNDestructive(const unsigned int newn)
	{
/*		SetSize(newn);
		n=newn;*/
		if (newn>size)
		{
			SetSizeDestructive(newn);
			shrinkcount = 0;
		}
		n=newn;
	}
	inline void Truncate(const unsigned int truncn)
	{
		if (truncn < n)
		{
			n = truncn;
		}
	}

	inline TYPE& GetBack()
	{
		return p[n-1];
	}
	inline const TYPE& GetBack() const
	{
		return p[n-1];
	}
	inline TYPE& GetFront()
	{
		return p[0];
	}
	inline const TYPE& GetFront() const
	{
		return p[0];
	}

	inline const bool IsEmpty() const
	{
		return GetN() == 0;
	}


	///////////////////////////////////////////////////////
	// Stack Interface

	inline void Stack_Push(const TYPE & v)
	{
		AddBack(v);
	}
	inline TYPE& Stack_Top()
	{
		return GetBack();
	}
	inline const TYPE& Stack_Top() const
	{
		return GetBack();
	}
	inline void Stack_Pop()
	{
		DecN();
	}


	///////////////////////////////////////////////////////
	// Queue Interface

	inline void Queue_Push(const TYPE & v)
	{
		AddBack(v);
	}
	inline TYPE& Queue_Top()
	{
		return GetFront();
	}
	inline const TYPE& Queue_Top() const
	{
		return GetFront();
	}
	inline void Queue_Pop()
	{
		Remove(0);
	}


	///////////////////////////////////////////////////////
	// Sort
	// all these functions sort ascending

	inline int BubbleSort(MArrayElementCompare < TYPE > * f)
	{
		unsigned int i, j;
		for (i = 0; i < GetN(); ++i)
		{
			for (j = i + 1; j < GetN(); ++j)
			{
				if (!f->IsSmaller(&((*this)[i]), &((*this)[j])))
				{
					TYPE t = (*this)[i];
					(*this)[i] = (*this)[j];
					(*this)[j] = t;
				}
			}
		}
		return 0;
	}


	// see qsort.pas in meinedat/schule/informatik11c or 12c
	inline void QuickSort(MArrayElementCompare < TYPE > * f, const int l, const int r)
	{
		//static int recur = 0;
		//printf("recur=%i   l=%i r=%i   ", recur, l, r);
		//PrintArray(*this);
		//if (l < r)
		//{
			int pivoti = (l + r) / 2;
			TYPE pivot = (*this)[pivoti];
			int li = l;
			int ri = r;
			while (li < ri)
			{
				//while ((f->IsSmaller(&((*this)[li]), &pivot)) && (li <= ri))
				while (f->IsSmaller(&((*this)[li]), &pivot))
				{
					++li;
				}
				//while ((f->IsSmaller(&pivot, &((*this)[ri]))) && (li <= ri))
				while (f->IsSmaller(&pivot, &((*this)[ri])))
				//while ((f->IsSmallerOrEqual(&pivot, &((*this)[ri]))) && (li < ri))
				{
					--ri;
				}
				if (li <= ri)
				{
					// xchange
					TYPE t = (*this)[li];
					(*this)[li] = (*this)[ri];
					(*this)[ri] = t;

					++li;
					--ri;
				}
			}
			//recur++;
			//printf("pivot=%i   ", pivot);
			//PrintArray(*this);
			//QuickSort(f, l, ri - 1);
			//QuickSort(f, li + 1, r);
			if (l < ri)
			{
				QuickSort(f, l, ri);
			}
			if (li < r)
			{
				QuickSort(f, li, r);
			}
			//recur--;
		//}
	}
	inline int QuickSort(MArrayElementCompare < TYPE > * f)
	{
		unsigned int n = GetN();
		if (n > 1)
		{
			QuickSort(f, 0, n - 1);
		}
		return 0;
	}

	inline int HeapSort(MArrayElementCompare < TYPE > * f)
	{
		return QuickSort(f);
	}

	inline int MergeSort(MArrayElementCompare < TYPE > * f)
	{
		return QuickSort(f);
	}

	// just uses some default sort
	inline int Sort(MArrayElementCompare < TYPE > * f)
	{
		return QuickSort(f);
	}

	/*inline void QuickSort(MArrayElementCompare < TYPE > * f)
	{
		unsigned int ipivot;
		ipivot = GetN() / 2;
		TYPE * pivot = (*this)[ipivot];

		unsigned int ia, ib;
		ia = 0;
		ib = GetN() - 1;

		TYPE * a = (*this)[ia];
		TYPE * b = (*this)[ib];

		while (ia < jb)
		{
			if (f->IsSmaller(&((*this)[ia]), &((*this)[j])))
			{
				TYPE t = (*this)[i];
				(*this)[i] = (*this)[j];
				(*this)[j] = t;
			}
		}
	}*/


	///////////////////////////////////////////////////////

	inline void Median(MArrayElementCompare < TYPE > * f, const int l, const int r)
	{
		int pivoti = (l + r) / 2;
		TYPE pivot = (*this)[pivoti];
		int li = l;
		int ri = r;
		while (li < ri)
		{
			while (f->IsSmaller(&((*this)[li]), &pivot))
			{
				++li;
			}
			while (f->IsSmaller(&pivot, &((*this)[ri])))
			{
				--ri;
			}
			if (li <= ri)
			{
				// xchange
				TYPE t = (*this)[li];
				(*this)[li] = (*this)[ri];
				(*this)[ri] = t;

				++li;
				--ri;
			}
		}

		if (ri >= GetN() / 2)		// these conditions might be optimizable so that there can be an else between them. 
		{
			if (l < ri)
			{
				Median(f, l, ri);
			}
		}
		if (li <= GetN() / 2)		// these conditions might be optimizable so that there can be an else between them.
		{
			if (li < r)
			{
				Median(f, li, r);
			}
		}
	}
	inline int Median(MArrayElementCompare < TYPE > * f)
	{
		unsigned int n = GetN();
		if (n > 1)
		{
			Median(f, 0, n - 1);
		}
		return 0;
	}


	///////////////////////////////////////////////////////
	// Misc

	inline void Reverse()
	{
		unsigned int i;
		for (i = 0; i < GetN() / 2; i++)
		{
			TYPE t = p[i];
			p[i] = p[GetN() - 1 - i];
			p[GetN() - 1 - i] = t;
		}
	}

	inline const bool Contains(const TYPE & v) const
	{
		unsigned int i;
		for (i = 0; i < GetN(); i++)
		{
			if ((*this)[i] == v)
			{
				return true;
			}
		}
		return false;
	}
};

#endif
