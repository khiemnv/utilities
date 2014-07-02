#include "StdAfx.h"
#include "KKUintArray.h"

KKUintArray::KKUintArray(void)
{
	count=0;
	arrInt=0;
	arrSize=0;
}

KKUintArray::~KKUintArray(void)
{
	free(arrInt);
}
UINT KKUintArray::get_at(UINT index)
{
	if(index<count)
	{
		erro=KKUIA_ERROR_SUCCESS;
		return arrInt[index];
	}
	else
		erro =KKUIA_INVALID_INDEX;
	return 0;
}
UINT KKUintArray::add(UINT val)
{
	if(isFull())
		incArrSize();		

	arrInt[count]=val;
	count++;
	return val;
}
bool KKUintArray::isFull()
{
	if (count==arrSize)
		return true;
	return false;
}
UINT KKUintArray::incArrSize()
{
	arrSize+=KKUIA_BLOCK;
	UINT *temp;
	temp=(UINT*)calloc(arrSize,sizeof(UINT));
	for (UINT i=0;i<count;i++)
		temp[i]=arrInt[i];
	free(arrInt);
	arrInt=temp;
	return arrSize;
}
bool KKUintArray::copy( KKUintArray &rval)
{
	for(int i=0;i<rval.getCount();i++)
	{
		add(rval.get_at(i));
	}
	return true;
}