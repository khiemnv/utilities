#pragma once

#ifndef __KK_UINT_ARRAY__
#define __KK_UINT_ARRAY__
#define KKUIA_BLOCK 20
#define KKUIA_ERROR_SUCCESS	0
#define KKUIA_INVALID_INDEX	1
class KKUintArray
{
public:
	KKUintArray();
	~KKUintArray();
	UINT*& operator[](UINT index);
	UINT* operator[](UINT index)const;
	UINT add(UINT val);
	UINT get_at(UINT index);
	bool isEmpty();
	int getCount(){return count;}
	UINT set_at(UINT index,UINT val);
	bool copy(KKUintArray& rval);
private:
	bool isFull();
	UINT incArrSize();
	UINT *arrInt;
	int count;
	UINT arrSize;
	UINT erro;
};
#endif
