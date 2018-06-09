#pragma once

class MSerialize
{
public:
	inline MSerialize()
	{
	}
	virtual ~MSerialize()
	{
	}
	virtual void Serialize(class MSerializeBuffer & b) = 0;
	virtual void Unserialize(class MUnserializeBuffer & b) = 0;
};
