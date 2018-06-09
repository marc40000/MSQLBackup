
// Note: N has to be a power of 2

#pragma once

template <class TYPE, unsigned int N> class MRingBufferStatic
{
public:
	TYPE p[N];
	unsigned int pointer;

	MRingBufferStatic()
	{
		pointer = 0;
	}	
	~MRingBufferStatic()
	{
	}
	inline TYPE * GetNext(const unsigned int ahead = 0)
	{
		return p + ((pointer + ahead) % N);
	}
	inline void IncPointer(const unsigned int amount = 1)
	{
		pointer += amount;
	}
	inline unsigned int GetPointer()
	{
		return pointer;
	}
	inline TYPE * Get(const unsigned int pointer_)
	{
		return p + (pointer_ % N);
	}
	inline TYPE * operator[](const unsigned int pointer_)
	{
		return Get(pointer_);
	}
	inline const unsigned int GetN() const
	{
		return N;
	}
};
