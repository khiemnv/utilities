#pragma once

#ifndef __KK_INT_ARRAY__
#define __KK_INT_ARRAY__
#define KKIA_BLOCK 20
#define KKIA_ERROR_SUCCESS	0
#define KKIA_INVALID_INDEX	1
class KKIntArray
{
public:
	KKIntArray();
	~KKIntArray();
	int*& operator[](int index);
	int* operator[](int index)const;
	int add(int val);
	int get_at(int index);
	bool isEmpty();
	int getCount(){return count;}
	int set_at(int index,int val);
	bool copy(KKIntArray& rval);
private:
	bool isFull();
	int incArrSize();
	int *arrInt;
	int count;
	int arrSize;
	int erro;
	
};
#endif
