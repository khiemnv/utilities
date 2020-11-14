//ringmem.cpp
#include "rmem.h"

#include <windows.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MY_ASSERT(x) assert(x)
#define ALIGN_BY(size, block) ((size + block - 1) & (~(block -1)))

/* RINGMEM_API */
RingMem::RingMem(char *buffer, int size)
{
	//not need init iFirst, iLast
	//ref funcs writeZ, readZ case empty
	mMode = 0;
	mCount = 0;
	mSize = size;
	mBuffer = buffer;
	mAllocatedSize = 0;
#if (USE_LOCK_FUNCTION == 1)
	mLockHandle = CreateMutex(
		NULL,              // default security attributes
		FALSE,             // initially owned
		NULL);             // unnamed mutex
#endif
}

/* RINGMEM_API */
RingMem::~RingMem()
{
#if (USE_LOCK_FUNCTION == 1)
	if (mLockHandle) {
		CloseHandle(mLockHandle);
		mLockHandle = 0;
	}
#endif
}
/* RINGMEM_API */
int RingMem::IsEmpty() {return mCount == 0;};
/* RINGMEM_API */
int RingMem::IsFull() {return mCount == mSize;};
/* RINGMEM_API */
int RingMem::Remain() {return (mSize - mCount);};
/* RINGMEM_API */
int RingMem::Write(char *data, int nByte, int *pWrited)
{
	int part1, part2;
	if (mAllocatedSize == 0)
	{
		if (nByte > mSize)
		{
			MY_ASSERT(0);	//not recommend
			nByte = mSize;
		}
		MY_ASSERT(nByte <= mSize);
		*pWrited = nByte;
		//buffer |...|data|...|
		//           |remain  |
		part1 = mSize - mCount;
		if (part1 >= nByte)
		{
			writeZ(data, nByte);
		}
		else
		{
			//buffer |data part2|...|data part1|
			//       |over write|   |remain    |
			part2 = nByte - part1;
			MY_ASSERT(part2 > 0);
			writeZ(data, part1);
			writeZ(data + part1, part2);
		}
	}
	return 0;
}

/* RINGMEM_API */
int RingMem::Read(char *outBuff, int nByte, int *pRead)
{
	if (nByte <= mCount)
	{	//buffer |data   ...|
		//       |outbuff|
		readZ(outBuff, nByte, pRead);
	}
	else
	{	//buffer |data   ...|
		//       |outbuff......|
		MY_ASSERT(nByte > mCount);
		readZ(outBuff, mCount, pRead);
	}
	return 0;
}
/* RINGMEM_API */
int RingMem::Peek(int index, char *outBuff, int nByte, int *pRead)
{
	int adjust = mCount - index;
	if (adjust <= 0)
	{
		*pRead = 0;
	}
	else if (nByte <= adjust)
	{	//buffer |data   ...|
		//       |outbuff|
		peekZ(index, outBuff, nByte, pRead);
	}
	else
	{	//buffer |data   ...|
		//       |outbuff......|
		MY_ASSERT((nByte > adjust) && (adjust > 0));
		peekZ(index, outBuff, adjust, pRead);
	}
	return 0;
}
/* RINGMEM_API */
int RingMem::Discard(int nByte, int *pDiscarded)
{
	if (nByte <= mCount)
	{	//buffer |data   ...|
		//       |discard|
		mCount -= nByte;
		miFirst += nByte;
		miFirst %= mSize;
		*pDiscarded = nByte;
	}
	else
	{	//buffer |data   ...|
		//       |discard......|
		MY_ASSERT(nByte > mCount);
		*pDiscarded = mCount;
		mCount = 0;
	}
	return 0;
}
/* RINGMEM_API */
int RingMem::Empty()
{
	mCount = 0;	//ref funcs writeZ, readZ case empty
	return 0;
}
/* RINGMEM_API */
int RingMem::Allocate(PCHAR *pHandle, int size, int *allocatedSize)
{
	if (mAllocatedSize == 0)
	{
		if (size > mSize)
		{
			size = mSize;
		}

		if (mCount == 0)
		{
			miFirst = 0;
			*pHandle = get(0);
			*allocatedSize = size;
			mAllocatedSize = size;
		}
		else
		{
			allocateZ(pHandle, size, allocatedSize);
		}
	}
	return 0;
}
int RingMem::allocateZ(PCHAR *pHandle, int size, int *allocatedSize)
{
	int delta;
	MY_ASSERT(mAllocatedSize == 0);
	//case 1
	//                  |allocate size|
	//  buffer |data ...|allocated    |         |
	//                 ^-iLast
	//         |<-- buffer size              -->|
	//case 2
	//                               |allocate size|
	//  buffer |data ...             |allocated |
	mAllocatedSize = mSize - ((miLast + 1) % mSize);
	if (mAllocatedSize > size)
	{
		mAllocatedSize = size;
	}
	*allocatedSize = mAllocatedSize;
	*pHandle = get((miLast + 1) % mSize);
	//adjust miFist like Write(mAllocatedSize...)
	//      |buffer                                  |
	//case1 |data                |                   |
	//       ^-iFirst           ^-iLast
	//                           |<-allocated->|
	//case2 |data2   |                 |data1        |
	//              ^-iLast             ^-iFirst
	//               |<-allocated->|
	//case3 |data2   |          |data1               |
	//              ^-iLast      ^-iFirst
	//               |<-allocated->|
	MY_ASSERT((miLast + mAllocatedSize) < mSize);
	delta = mAllocatedSize - (mSize - mCount);
	if (delta > 0)
	{ //case 3
		miFirst = (miFirst + delta) % mSize;
		mCount -= delta;
	}
	return 0;
}
char* RingMem::get(char* outBuf, int index, int size)
{
	if (mMode == RingMemAddrModeAbsolute) {
		return (char*)memcpy(outBuf, mBuffer + index, size);
	} else {
		MY_ASSERT(mMode == RingMemAddrModeRelative);
		return (char*)memcpy(outBuf, (char*)this + mOffset + index, size);
	}
}
char* RingMem::set(int index, char* data, int size)
{
	if (mMode == RingMemAddrModeAbsolute) {
		return (char*)memcpy(mBuffer + index, data, size);
	} else {
		MY_ASSERT(mMode == RingMemAddrModeRelative);
		return (char*)memcpy((char*)this + mOffset + index, data, size);
	}
}
char* RingMem::get(int index)
{
	if (mMode == RingMemAddrModeAbsolute) {
		return (mBuffer + index);
	} else {
		MY_ASSERT(mMode == RingMemAddrModeRelative);
		return ((char*)this + mOffset + index);
	}
}
/* RINGMEM_API */
int RingMem::Locate(int index, PCHAR *pHandle, int nByte, int *locatedSize)
{
	int adjust = mCount - index;
	if (adjust <= 0)
	{
		*locatedSize = 0;
	}
	else if (nByte <= adjust)
	{	//buffer |data   ...    |
		//           |nByte|
		locateZ(index, pHandle, nByte, locatedSize);
	}
	else
	{	//buffer |data   ...|
		//        |nByte......|
		MY_ASSERT((nByte > adjust) && (adjust > 0));
		locateZ(index, pHandle, adjust, locatedSize);
	}
	return 0;
}
int RingMem::locateZ(int index, PCHAR *pHandle, int nByte, int *locatedSize)
{
	int located, delta;
	//LOGICAL
	//                             |locate  |
	//RingMem |data                         |
	//         ^-iFirst             ^      ^-iLast
	//                              index
	//PHYSICAL
	//case 1
	//buffer  |data             |           |
	//              ^-delta    ^-iLast
	//case 2  |data 2        |     |data 1  |
	//                      ^-iLast    ^delta
	delta = (miFirst + index) % mSize;
	if (delta < miLast)
	{	//case 1
		MY_ASSERT(miFirst < miLast);
		located = miLast - delta + 1;
	}
	else
	{	//case 2
		located = mSize - delta;
	}

	*pHandle = get(delta);
	if (nByte <= located)
	{
		*locatedSize = nByte;
	}
	else
	{
		*locatedSize = located;
	}
	return 0;
}
/* RINGMEM_API */
int RingMem::Commit(int nByte, int *commited)
{
	if (mAllocatedSize != 0)
	{
		if (nByte > mAllocatedSize)
		{
			MY_ASSERT(0);	//not recommend
			nByte = mAllocatedSize;
		}

		mAllocatedSize = 0;
		*commited = nByte;
		//adjust miLast like Write(nByte...)
		if (mCount == 0)
		{
			miLast = nByte - 1;
		}
		else
		{
			miLast = (miLast + nByte) % mSize;
		}
		mCount += nByte;
	}
	return 0;
}
/*RINGMEM_API*/
int RingMem::BufferSize() {return mSize;}
/*RINGMEM_API*/
int RingMem::Count() {return mCount;}
int RingMem::writeZ(char *data, int nByte)
{
	//int remain = mSize - mCount;
	int delta;
	//MY_ASSERT(remain >= nByte || remain == 0);
	MY_ASSERT((mSize - mCount) >= nByte || (mSize - mCount) == 0);

	if (mCount == 0)
	{	//case empty
		// buffer|data|...|
		//        ^  ^-miLast
		//        miFirst
		mCount += nByte;
		miLast = nByte - 1;
		miFirst = 0;
		set(0, data, nByte);
	}
	else
	{
		if (mCount < mSize)
		{
			//case normal
			mCount += nByte;
			delta = (miLast + 1) % mSize;
			push(delta, data, nByte);
			miLast += nByte;
			miLast %= mSize;
		}
		else
		{
			MY_ASSERT(mCount == mSize);
			//case full
			miFirst += nByte;
			miFirst %= mSize;
			delta = (miLast + 1) % mSize;
			push(delta, data, nByte);
			miLast += nByte;
			miLast %= mSize;
		}
	}
	return 0;
}

int RingMem::readZ(char *outBuff, int nByte, int *pRead)
{
	int delta;
	MY_ASSERT(mCount >= nByte);
	//case empty
	if (mCount == 0)
	{
		*pRead = 0;
		return 0;
	}
	else
	{
		//case normal
		mCount -= nByte;
		delta = miFirst;
		pop(outBuff, delta, nByte);
		miFirst += nByte;
		miFirst %= mSize;
		*pRead = nByte;
	}
	return 0;
}

int RingMem::peekZ(int index, char *outBuff, int nByte, int *pRead)
{
	int delta;
	MY_ASSERT(mCount >= (nByte + index));
	//case empty
	if (mCount == 0)
	{
		*pRead = 0;
		return 0;
	}
	else
	{
		//case normal
		delta = (miFirst + index) % mSize;
		pop(outBuff, delta, nByte);
		*pRead = nByte;
	}
	return 0;
}

int RingMem::push(int index, char *srcBuf, int size)
{
	int part1;
	//mBuffer: |...|srcbuff|...|
	//              ^
	//              index
	part1 = mSize - index;
	if (part1 >= size)
	{
		set(index, srcBuf, size);
	}
	else
	{
		//mBuffer: |srcbuff part2|...|srcbuff part1|
		//                            ^
		//                            index
		MY_ASSERT(part1 < size);
		set(index, srcBuf, part1);
		set(0, srcBuf + part1, size - part1);
	}
	return 0;
}

int RingMem::pop(char *outBuf, int index, int size)
{
	int part1;
	//mBuffer: |...|outData|...|
	//              ^
	//              index
	part1 = mSize - index;
	if (part1 >= size)
	{
		get(outBuf, index, size);
	}
	else
	{
		//mBuffer: |outData part2|...|outData part1|
		//                            ^
		//                            index
		MY_ASSERT(part1 < size);
		get(outBuf, index, part1);
		get(outBuf + part1, 0, size - part1);
	}
	return 0;
}
#if (USE_LOCK_FUNCTION == 1)
/*RINGMEM_API*/
//if success func return 0 (WAIT_OBJECT_0)
//else func return error code != 0
int RingMem::AcqLock(DWORD timeOut)
{
	int rc = 0;
	if (mLockHandle)
	{
		rc = WaitForSingleObject(mLockHandle, timeOut);
	}
	return rc;
}
/*RINGMEM_API*/
//if success func return 0
//else func return error code != 0
int RingMem::RlsLock()
{
	int rc = 0;
	if (mLockHandle)
	{
		if (ReleaseMutex(mLockHandle))
			rc = 0;
		else
			rc = GetLastError();
	}
	return rc;
}
#endif /*USE_LOCK_FUNCTION*/
/*RINGMEM_API*/
void RingMem::Reset(char *buffer, int size, int mode)
{
	if (mode == RingMemAddrModeAbsolute) {
		MY_ASSERT((char*)this != buffer);
		mMode = RingMemAddrModeAbsolute;
		mBuffer = buffer;
	} else {
		MY_ASSERT(mode == RingMemAddrModeRelative);
		MY_ASSERT((char*)this == buffer);

		SYSTEM_INFO si;
		GetSystemInfo(&si);

		int moduleSize = ALIGN_BY(sizeof(RingMem), si.dwPageSize);
		memset(buffer, 0, moduleSize);

		mMode = RingMemAddrModeRelative;
		mOffset = moduleSize;
		size -= moduleSize;
	}

	MY_ASSERT(size > 0);
	mCount = 0;
	mSize = size;
	mAllocatedSize = 0;
}
