/*
		MHashE

		(c) 2008 by Marc Rochel

		0. reference, no serial, all equal									MHashE
		1. reference, flat serialization; just memcpy, all equal			MHashEF
		2. reference, serial, all equal										MHashES
		3. pointer, serial with builder, maybe different					MHashPS
		4. pointer, serial, all equal										MHashPES	* not implemented, use 3 with a MSimpleSerializeBuilder

		- pointer, no serial, maybe different => 0 can handle that
		- pointer, no serial, all equal => 0 can handle that

		Key Features:
		- resizeable
		- key can be a simple datatype or MHashable32
		- iterators don't get invalid when elements get added or removed 
		- deterministic (un)serializations

		Notes:
		- For strings as keys, use MString, which derives from MHashable32
		- If you plan to put significantly more than 3707 elements into the MHash, add primes to the MHashE_primes array
		  accordingly. Every prime is roughly the double from the previous prime. The last value is a 0 to indicate the end
		  of the primes list.
		- Constructors and Destructors of the Elements don't get called. If you need that, declare init/destroy methods
		  and call them yourself or use the MArrayPS and call the constructor/destructor yourself. This is because malloc is used.
*/

#ifndef _MHASHE_
#define _MHASHE_

#include "MArrayE.h"
#include "MHashable32.h"
//#include "MString.h"

const unsigned int MHashE_primes[] = {7, 17, 37, 79, 163, 331, 661, 1321, 2647, 5297, 10601, 21211, 42403, 84809, 170003, 340007, 680003, 1360027, //0,	// these are primes
	2700023, 5400001, 10000019, 20000003, 40000003, 80000023, 160481183, 314606869, 633910111, 982451653, 0					// these are primes as well
	//2720053, 5440105, 10880209, 21760417, 43520833, 87041665, 174083329, 348166657, 696333313, 1392666625, 2785333249U,	// these should be primes but aren't. I need a list of primes in that range ...
	//0		// 0 at the end
	};



template <class KEY, class TYPE> class MHashE
{
public:
#ifdef MARRAYEUSEMALLOC
	struct MHashNode
	{
		KEY key;
		TYPE v;
	};
#else
	class MHashNode
	{
	public:
		KEY key;
		TYPE v;
		inline MHashNode()
		{
		}
		inline MHashNode(const MHashNode & o)
		{
			key = o.key;
			v = o.v;
		}
		inline MHashNode & operator= (const MHashNode & o)
		{
			key = o.key;
			v = o.v;
			return *this;
		}
		inline ~MHashNode()
		{
		}
	};
#endif
	typedef MArrayE<MHashNode> MArrayEHashNode;
	
	MArrayEHashNode * p;
	unsigned int n;
	unsigned int m;
	unsigned int primei;
	TYPE typenull;
	KEY keynull;

	inline MHashE()
	{
		p=0;
		Reset();
	}
	virtual ~MHashE()
	{
		if (p!=0)
		{
			//RemoveAll();		// no use if the all get deleted afterwards, commented out 2009-02-12
			delete [] p;
		}
	}

// internal functions
	inline void SetHashTableN(const unsigned int newn)
	{
		// don't use realloc here because we need the constructors and destructors of MArrayE to be called
		if (p!=0)
		{
			//RemoveAll();		// no use if they all get deleted afterwards
			delete [] p;
		}
		m=0;
		p=new MArrayEHashNode[newn];
		n=newn;
	}

	inline void DeleteAll()
	{
		unsigned int i,j;
		for (i = 0; i < n; ++i)
			for (j = 0; j < p[i].GetN(); ++j)
				delete p[i][j].v;
	}

	inline void DeleteIfNotNullAll()
	{
		unsigned int i,j;
		for (i = 0; i < n; ++i)
		{
			for (j = 0; j < p[i].GetN(); ++j)
			{
				if (p[i][j].v != 0)
				{
					delete p[i][j].v;
				}
			}
		}
	}

	inline void RemoveAll()
	{
		unsigned int i;
		for (i=0;i<n;i++)
			p[i].SetN(0);
		m = 0;
	}
	inline void Reset()
	{
		primei = 0;
		SetHashTableN(MHashE_primes[primei]);
	}
	// Note: this destroys the content
	inline void PrepareForNElements(const unsigned int newn)
	{
		primei = 0;
		// Optimize: I could binary search on MHashE_primes instead
		while ((MHashE_primes[primei] < newn + newn / 2) && (MHashE_primes[primei + 1] != 0))		// search prime that is larger than 150 % of newn
		{
			primei++;
		}
		SetHashTableN(MHashE_primes[primei]);
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

// user interface
	inline void Insert(const KEY & key, const TYPE & v)
	{
		MHashNode hn;
		hn.key=key;
		hn.v=v;
		p[HashFunction(key)].AddBack(hn);
		m++;
	}
	inline const bool Lookup(const KEY & key, TYPE & v) const
	{
		MArrayEHashNode *pcur=p+HashFunction(key);
		unsigned int i;
		for (i=0;i<pcur->GetN();i++)
		{
			if (key==(*pcur)[i].key)
			{
				v=(*pcur)[i].v;
				return true;
			}
		}
		return false;
	}
	inline const bool Exists(const KEY & key) const
	{
		MArrayEHashNode *pcur=p+HashFunction(key);
		unsigned int i;
		for (i=0;i<pcur->GetN();i++)
		{
			if (key==(*pcur)[i].key)
			{
				return true;
			}
		}
		return false;
	}
	inline TYPE & operator[](const KEY & key)
	{
		MArrayEHashNode *pcur = p + HashFunction(key);
		unsigned int i;
		for (i=0;i<pcur->GetN();i++)
		{
			if (key==(*pcur)[i].key)
			{
				return (*pcur)[i].v;
			}
		}
		// key not found in hashtable
		// => create an entry according to the key and return its reference
		MHashNode hn;
		hn.key=key;
		pcur->AddBack(hn);
		m++;
		return pcur->GetBack().v;
	}
	inline const TYPE & operator[](const KEY & key) const
	{
		MArrayEHashNode *pcur = p + HashFunction(key);
		unsigned int i;
		for (i=0;i<pcur->GetN();i++)
		{
			if (key==(*pcur)[i].key)
			{
				return (*pcur)[i].v;
			}
		}
		// key not found in hashtable
		// above: => create an entry according to the key and return its reference
		// but we are const here, so we return nothing
		return NULL;
	}
	inline void Remove(const KEY & key)
	{
		MArrayEHashNode *pcur = p + HashFunction(key);
		unsigned int i;
		for (i = 0; i < pcur->GetN(); ++i)
		{
			if (key==(*pcur)[i].key)
			{
				(*pcur).Remove(i);
				--m;
				return;
			}
		}
	}
	inline void DeleteAndRemove(const KEY & key)
	{
		MArrayEHashNode *pcur = p + HashFunction(key);
		unsigned int i;
		for (i = 0; i < pcur->GetN(); ++i)
		{
			if (key == (*pcur)[i].key)
			{
				delete (*pcur)[i].v;
				(*pcur).Remove(i);
				--m;
				return;
			}
		}
	}
	
	struct Iterator
	{
		unsigned int bucket;
		unsigned int posinbucket;
	};
	inline Iterator CreateIterator() const
	{
		Iterator i;
		i.bucket=0;
		i.posinbucket=0;
		if (p[0].GetN()==0)
			MoveIterator(i);
		return i;
	}
	inline void MoveIterator(Iterator &i) const
	{
		if (i.bucket>=n)
			i.bucket=0xffffffff;
		else
		{
			if (i.posinbucket+1<p[i.bucket].GetN())
				i.posinbucket++;
			else
			//if (i.posinbucket>=p[i.bucket].GetN())
			{
				i.bucket++;
				if (i.bucket>=n)
					return;
				while (p[i.bucket].GetN()==0)
				{
					i.bucket++;
					if (i.bucket>=n)
						return;
				}
				i.posinbucket=0;
			}
		}
	}
	inline bool CheckContinueIterator(Iterator &i) const
	{
		return !((i.bucket>=n) || ((i.bucket==n-1) && (i.posinbucket>=p[i.bucket].GetN())));
	}
	inline void Remove(Iterator &i)
	{
		KEY key = this->Key(i);
		Remove(key);
	}
	inline void DeleteAndRemove(Iterator &i)
	{
		KEY key = this->Key(i);
		DeleteAndRemove(key);
	}

	// shortcut names:
	inline Iterator CI() const
	{
		return CreateIterator();
	}
	inline void MI(Iterator &i) const
	{
		return MoveIterator(i);
	}
	inline bool CCI(Iterator &i) const
	{
		return CheckContinueIterator(i);
	}
	inline TYPE & operator[](Iterator &i)
	{
		if (i.bucket<n)
		{
			MArrayEHashNode *pcur=p+i.bucket;
			if (i.posinbucket<pcur->GetN())
				return (*pcur)[i.posinbucket].v;
		}
		// If the Iterator stranded in between for some reason
		// we go to the next valid position and return its reference
		MoveIterator(i);
		if (i.bucket<n)
		{
			MArrayEHashNode *pcur=p+i.bucket;
			if (i.posinbucket<pcur->GetN())
				return (*pcur)[i.posinbucket].v;
		}
		// We are at the end, thus we return NULL.
		//return NULL;
		return typenull;
	}
	inline const TYPE & operator[](Iterator &i) const 
	{
		if (i.bucket<n)
		{
			MArrayEHashNode *pcur=p+i.bucket;
			if (i.posinbucket<pcur->GetN())
				return (*pcur)[i.posinbucket].v;
		}
		// If the Iterator stranded in between for some reason
		// we go to the next valid position and return its reference
		MoveIterator(i);
		if (i.bucket<n)
		{
			MArrayEHashNode *pcur=p+i.bucket;
			if (i.posinbucket<pcur->GetN())
				return (*pcur)[i.posinbucket].v;
		}
		// We are at the end, thus we return NULL.
		//return NULL;
		return typenull;
	}

	inline const KEY & Key(Iterator &i) const 
	{
		if (i.bucket<n)
		{
			MArrayEHashNode *pcur=p+i.bucket;
			if (i.posinbucket<pcur->GetN())
				return (*pcur)[i.posinbucket].key;
		}
		// If the Iterator stranded in between for some reason
		// we go to the next valid position and return its reference
		MoveIterator(i);
		if (i.bucket<n)
		{
			MArrayEHashNode *pcur=p+i.bucket;
			if (i.posinbucket<pcur->GetN())
				return (*pcur)[i.posinbucket].key;
		}
		// We are at the end, thus we return NULL.
		//return NULL;
		return keynull;
	}


	inline const unsigned int GetN() const
	{
		return m;
	}
	inline const TYPE & GetNthItem(const unsigned int k) const
	{
		unsigned int pos=0,i;
		for (i=0;i<n;i++)
		{
			if (pos+p[i].GetN()>k)
			{
				return p[i][k-pos].v;
			}
			pos+=p[i].GetN();
		}
		// return nix;
		return typenull;
	}
	inline const KEY & GetNthKey(const unsigned int k) const
	{
		unsigned int pos=0,i;
		for (i=0;i<n;i++)
		{
			if (pos+p[i].GetN()>k)
			{
				return p[i][k-pos].key;
			}
			pos+=p[i].GetN();
		}
		// return nix;
		return keynull;
	}


	// adjusts the hashtablesize to the given value;
	// Note: all iterators get invalid. Call this manually when no iterators are in use.
	inline void Refill(const unsigned int newn)
	{
		// copy everything to a new hash
		MHashE < KEY, TYPE > o;
		o.SetHashTableN(newn);
		Iterator iter;
		for (iter = CreateIterator(); CheckContinueIterator(iter); MoveIterator(iter))
		{
			o[Key(iter)] = (*this)[iter];
		}

		// exchange the p and n of the two hashmaps
		MArrayEHashNode * tp;
		unsigned int tn;
		//unsigned int tm;
		//unsigned int tprimei;
		
		tp = p;
		tn = n;
		//tm = m;
		//tprimei = primei;
		
		p = o.p;
		n = o.n;
		//m = o.m;
		//primei = o.primei;
		
		o.p = tp;
		o.n = tn;
		//o.m = tm;
		//o.primei = tprimei;
	}

	// adjusts the hashtablesize by some heuristics 
	// Note: all iterators get invalid. Call this manually when no iterators are in use.
	inline void Optimize()
	{
		// Check if we should enlarge the size
		if (((unsigned long long)m) * 256 > ((unsigned long long)n) * 180)		// if the fill amount is > 70% 
		{
			if (MHashE_primes[primei + 1] != 0)
			{
				// roughly double the size of the hashtable
				++primei;
				Refill(MHashE_primes[primei]);
				//_tprintf(_T("m=%i n=%i\n"), m, n); 
			}
		}
		else
		if (((unsigned long long)m) * 256 < ((unsigned long long)n) * 76)		// if the fill amount is < 30 %
		{
			if (primei != 0)
			{
				// roughly half the size of the hashtable
				--primei;
				Refill(MHashE_primes[primei]);
				//_tprintf(_T("m=%i n=%i\n"), m, n); 
			}
		}
	}
	

	/////////////////////////////////////////////////////////////////
	// HashFunctions


	inline const unsigned int HashFunction(const KEY & key) const
	{
		MHashable32Helper<KEY> helper;
		return helper.HashFunction32(key) % n;
		//return HashFunction32 < KEY > (key) % n;
	}

};



// these don't compile somehow :(
//#define MHashIterate (hashmaptype, hashmap, itername) hashmaptype::Iterator itername; \
//	for (itername = hashmap.CI(); hashmap.CCI(itername); hashmap.MI(itername))
//
//#define MHashIterateP (hashmaptype, hashmap, itername) hashmaptype::Iterator itername; \
//	for (itername = hashmap->CI(); hashmap->CCI(itername); hashmap->MI(itername))


#define MHashIterate(hashmap, itername) for (auto itername = (hashmap).CI(); (hashmap).CCI((itername)); (hashmap).MI((itername)))
#define MHashIterateP(hashmap, itername) for (auto itername = (hashmap)->CI(); (hashmap)->CCI((itername)); (hashmap)->MI((itername)))

#define MHashIterateDecl(hashmap, itername) decltype(hashmap)::Iterator itername; for (auto itername = (hashmap).CI(); (hashmap).CCI((itername)); (hashmap).MI((itername)))
#define MHashIterateDeclP(hashmap, itername) decltype(hashmap)::Iterator itername; for (auto itername = (hashmap)->CI(); (hashmap)->CCI((itername)); (hashmap)->MI((itername)))

#endif
