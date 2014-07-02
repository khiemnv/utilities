#include "StdAfx.h"
#include "KKIntArray.h"

KKIntArray::KKIntArray(void)
{
	count=0;
	arrInt=0;
	arrSize=0;
}

KKIntArray::~KKIntArray(void)
{
	free(arrInt);
}
int KKIntArray::get_at(int index)
{
	if(index<count)
	{
		erro=KKIA_ERROR_SUCCESS;
		return arrInt[index];
	}
	else
		erro =KKIA_INVALID_INDEX;
	return 0;
}
int KKIntArray::add(int val)
{
	if(isFull())
		incArrSize();		

	arrInt[count]=val;
	count++;
	return val;
}
bool KKIntArray::isFull()
{
	if (count==arrSize)
		return true;
	return false;
}
int KKIntArray::incArrSize()
{
	arrSize+=KKIA_BLOCK;
	int *temp;
	temp=(int*)calloc(arrSize,sizeof(int));
	for (int i=0;i<count;i++)
		temp[i]=arrInt[i];
	free(arrInt);
	arrInt=temp;
	return arrSize;
}
bool KKIntArray::copy(KKIntArray &rval)
{
	for(int i=0;i<rval.getCount();i++)
	{
		add(rval.get_at(i));
	}
	return true;
}
