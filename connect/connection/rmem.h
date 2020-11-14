//ringmem.h
#pragma once
//#include <windows.h>

#ifndef RING_MEM_H__
#define RING_MEM_H__

//#define RINGMEM_DLL
#define USE_LOCK_FUNCTION (0)

#ifdef RINGMEM_DLL
#ifdef RINGMEM_LIBRARY_EXPORT   // inside DLL
#   define RINGMEM_API   __declspec(dllexport)
#else // outside DLL
#   define RINGMEM_API   __declspec(dllimport)
#endif /*RINGMEM_LIBRARY_EXPORT*/
#else
#   define RINGMEM_API
#endif /*RINGMEM_DLL*/

//RingMem
//CASE 1
//             |data          |
//mBuffer |... |x| ...      |x|...         |
//              ^            ^
//              miFirst      miLast
//CASE 2
//        |data part2|          |data part1|
//mBuffer |...     |x|...       |x|...     |
//                  ^            ^
//                  miLast       miFirst       
#ifndef PCHAR
typedef char * PCHAR;
#endif

class RingMem
{
private:
	int mMode;    //addr absolute(0) or relative(1)
	int miFirst;
	int miLast;
	int mCount;   //by byte
	int mSize;    //by byte
	int mAllocatedSize;  //size of recent allocated space
	union {
		int mOffset;
		char *mBuffer;
	};
	int push(int index, char *outBuf, int size);
	int pop(char *outBuf, int index, int size);
	int writeZ(char *data, int size);
	int readZ(char *outBuff, int size, int *read);
	int peekZ(int index, char *outBuff, int nByte, int *pRead);
	int locateZ(int index, PCHAR *pHandle, int nByte, int *locatedSize);
	int allocateZ(PCHAR *pHandle, int size, int *allocatedSize);
#if (USE_LOCK_FUNCTION == 1)
	HANDLE mLockHandle;
#endif
	char* get(char* outBuf, int index, int size);
	char* set(int index, char* data, int size);
	char* get(int index);
public:
	enum {
		RingMemAddrModeAbsolute = 0,
		RingMemAddrModeRelative = 1,
	};
	//func RingMem
	//  init ring mem
	//PARAMS
	//  __in buffer: pointer to buffer that hold ring mem data
	//  __int size: size of buffer
	RINGMEM_API RingMem(char *buffer=0, int size=0);
	RINGMEM_API ~RingMem();
	//func Write
	//  Write data to ring mem
	//PARAMS
	//  __in data: poiter to buffer that hold data
	//  __in nByte: number of byte need be written
	//  __out pWrited: number of byte was written
	//NOTE
	//  + pointer auto seek after write (like file write)
	//  + if remain is not enough, overwrite the data in begin mem
	//    write data|data part2|...|data part1|
	//    ringmem   |over write|   |remain    |
	//  + nByte should > 0
	RINGMEM_API int Write(char *data, int nByte, int *pWrited);
	//func Read
	//  Read data from ring mem
	//PARAMS
	//  __out outBuff: pointer to buff that hold read data
	//  __in nByte: number of byte want to read
	//  __out pRead: number of byte was read
	//NOTE
	//  + pointer auto seek after read (like file read)
	//  + nByte should > 0
	RINGMEM_API int Read(char *outBuff, int nByte, int *pRead);
	//func Peek
	//  like Read() but DONT seek pointer
	//PARAMS
	//  __in index: start read byte (zero base)
	//  __out outBuff: pointer to buff that hold read data
	//  __in nByte: number of byte want to read
	//  __out pRead: number of byte was read
	RINGMEM_API int Peek(int index, char *outBuff, int nByte, int *pRead);
	//func Discard
	//  remove nByte from begin mem
	//NOTE
	//  like Read() but DONT write out data to outBuff
	RINGMEM_API int Discard(int nByte, int *pDiscarded);
	RINGMEM_API int IsEmpty();
	RINGMEM_API int IsFull();
	//return the remain of the ring mem in byte
	RINGMEM_API int Remain();
	//empty the ring mem
	RINGMEM_API int Empty();
	//func Allocate
	//  allocate a mem space in  RMem
	//NOTE
	//  can not allocate new space if pre allocated space not commit
	//  if (remain is not enough) overwrite the data in begin of mem
	RINGMEM_API int Allocate(PCHAR *pHandle, int size, int *allocatedSize);
	//func Locate
	//  locate data at special index
	//NOTE
	//  like Peek() but not copy data to outBuff
	RINGMEM_API int Locate(int index, PCHAR *pHandle, int nByte, int *locatedSize);
	//func Commit
	//  commit recent allocated space
	RINGMEM_API int Commit(int nByte, int *commited);
	//func Size
	//  return ringmem buffer size
	RINGMEM_API int BufferSize();
	//func Count
	//  return size of data in byte
	RINGMEM_API int Count();
	//func Reset
	//  reset RingMem buffer and size
	RINGMEM_API void Reset(char *buffer, int size, int mode = 0);
#if (USE_LOCK_FUNCTION == 1)
	//func AcqLock
	//  wait for enter the mutex
	RINGMEM_API int AcqLock(DWORD timeOut = INFINITE);
	//func RlsLock
	//  release the mutex
	//NOTE
	//  take care when use AcqLock, RlsLock to avoid death lock
	RINGMEM_API int RlsLock();
#endif /*USE_LOCK_FUNCTION*/
};
#endif //RING_MEM_H__