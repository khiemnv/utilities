//#include "TsPacket.h"
#include <tchar.h>
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

#include <crtdbg.h>
#define assert(x) _ASSERT(x)

//-------------options
#define PES_PCR 0		//remove packet PCR_PID
//-------------MACRO
#define DU_ENSURE_RETURN2(...)

#define CONST_LL(X) X

#define BSWAP16(x) \
    (Ipp16u) ((x) >> 8 | (x) << 8)

#define BSWAP32(x) \
    (Ipp32u)(((x) << 24) + \
    (((x)&0xff00) << 8) + \
    (((x) >> 8)&0xff00) + \
    ((Ipp32u)(x) >> 24))

#define BSWAP64(x) \
    (Ipp64u)(((x) << 56) + \
    (((x)&0xff00) << 40) + \
    (((x)&0xff0000) << 24) + \
    (((x)&0xff000000) << 8) + \
    (((x) >> 8)&0xff000000) + \
    (((x) >> 24)&0xff0000) + \
    (((x) >> 40)&0xff00) + \
    ((x) >> 56))

#ifdef _BIG_ENDIAN_
#   define BIG_ENDIAN_SWAP16(x) BSWAP16(x)
#   define BIG_ENDIAN_SWAP32(x) BSWAP32(x)
#   define BIG_ENDIAN_SWAP64(x) BSWAP64(x)
#   define LITTLE_ENDIAN_SWAP16(x) (x)
#   define LITTLE_ENDIAN_SWAP32(x) (x)
#   define LITTLE_ENDIAN_SWAP64(x) (x)
#else // _BIG_ENDIAN_
#   define BIG_ENDIAN_SWAP16(x) (x)
#   define BIG_ENDIAN_SWAP32(x) (x)
#   define BIG_ENDIAN_SWAP64(x) (x)
#   define LITTLE_ENDIAN_SWAP16(x) BSWAP16(x)
#   define LITTLE_ENDIAN_SWAP32(x) BSWAP32(x)
#   define LITTLE_ENDIAN_SWAP64(x) BSWAP64(x)
#endif // _BIG_ENDIAN_

#define INC_COUNTER(c)  (((c)+1) & 0x0f)

//-------------type
typedef double              Ipp64f;
typedef unsigned long long  Ipp64u;
typedef signed long long    Ipp64s;
typedef unsigned int        Ipp32u;
typedef signed int          Ipp32s;
typedef unsigned short      Ipp16u;
typedef signed short        Ipp16s;
typedef unsigned char       Ipp8u;
typedef signed char         Ipp8s;

typedef signed int          Status;

typedef unsigned char       uint8_t;
typedef signed int          int32_t;
typedef unsigned int        uint32_t;
typedef signed long long    int64_t;
typedef unsigned long long  uint64_t;

//-------------constant

static Ipp8u *pInvalidPtr = (Ipp8u *)1;
static const Ipp32u MPEG2MUX_SYS_CLOCK_FREQ     = 27000000;
static const Ipp32u MPEG2MUX_TIME_TO_SLEEP      = 5;

static const Ipp32u MPEG2MUX_LPCM_FRAME_SIZE    = 320;
static const Ipp32u MPEG2MUX_CHUNK_HEADER_SIZE  = 256;
static const Ipp32u MPEG2MUX_TS_PACKET_LENGTH   = 188;
static const Ipp32u MPEG2MUX_MAX_TS_PAYLOAD     = 184;

static const Ipp32u MPEG2MUX_SYS_ID_AUDIO       = 0xB8;
static const Ipp32u MPEG2MUX_SYS_ID_VIDEO       = 0xB9;
static const Ipp32u MPEG2MUX_PES_ID_AUDIO       = 0xC0;
static const Ipp32u MPEG2MUX_PES_ID_VIDEO       = 0xE0;
static const Ipp32u MPEG2MUX_PES_ID_PRIVATE_1   = 0xBD;
static const Ipp32u MPEG2MUX_PES_ID_PADDING     = 0xBE;
static const Ipp32u MPEG2MUX_AC3_SUB_ID         = 0x80;
static const Ipp32u MPEG2MUX_LPCM_SUB_ID        = 0xA0;

static const Ipp32u MPEG2MUX_PROGRAM_MAP_PID    = 0x006E;
static const Ipp32u MPEG2MUX_INITIAL_ES_PID     = 0x0070;

static const Ipp32u MPEG2MUX_TS_ID              = 0x0000;

typedef enum
{
  MPEG2MUX_ES_MPEG1_VIDEO = 0x01,
  MPEG2MUX_ES_MPEG2_VIDEO = 0x02,
  MPEG2MUX_ES_MPEG4_VIDEO = 0x10,
  MPEG2MUX_ES_H264_VIDEO  = 0x1B,
  MPEG2MUX_ES_LPCM_AUDIO  = 0x83,
  MPEG2MUX_ES_AC3_AUDIO   = 0x81,
  MPEG2MUX_ES_AAC_AUDIO   = 0x0F,
  MPEG2MUX_ES_MPEG1_AUDIO = 0x03,
  MPEG2MUX_ES_MPEG2_AUDIO = 0x04,
  MPEG2MUX_ES_VBI_DATA    = 0x06,
  MPEG2MUX_ES_UNKNOWN     = 0xFF
} MPEG2MuxerESType;

enum
{
  TYPE_I_PIC = 0x01,
  TYPE_P_PIC = 0x02,
  TYPE_B_PIC = 0x04,
  TYPE_UNDEF = 0x08
};

//-------------TS CONSTs
const int PACKET_SIZE = 188;
const int TIME_STAMP_SIZE = 4;
const int VIDEO_PID = 0x1011;
const int AUDIO_PID = 0x1100;
const int PCR_PID   = 0x1001;
const int PAT_PID   = 0x0000;
const int PMT_PID   = 0x0100;
const int SIT_PID   = 0x001F;

const int VIDEO_STREAM_ID = 0xE0;
const int AUDIO_STREAM_ID = 0xC0;

//-------------crc - zlib/crc32.c
typedef unsigned int uLongf;
typedef unsigned int uLong;
typedef unsigned int uInt;
typedef unsigned char Bytef;

#define ZEXPORT
#define Z_NULL  0

#define local static

#ifdef DYNAMIC_CRC_TABLE

local int crc_table_empty = 1;
local uLongf crc_table[256];
local void make_crc_table OF((void));

/*
  Generate a table for a byte-wise 32-bit CRC calculation on the polynomial:
  x^32+x^26+x^23+x^22+x^16+x^12+x^11+x^10+x^8+x^7+x^5+x^4+x^2+x+1.

  Polynomials over GF(2) are represented in binary, one bit per coefficient,
  with the lowest powers in the most significant bit.  Then adding polynomials
  is just exclusive-or, and multiplying a polynomial by x is a right shift by
  one.  If we call the above polynomial p, and represent a byte as the
  polynomial q, also with the lowest power in the most significant bit (so the
  byte 0xb1 is the polynomial x^7+x^3+x+1), then the CRC is (q*x^32) mod p,
  where a mod b means the remainder after dividing a by b.

  This calculation is done using the shift-register method of multiplying and
  taking the remainder.  The register is initialized to zero, and for each
  incoming bit, x^32 is added mod p to the register if the bit is a one (where
  x^32 mod p is p+x^32 = x^26+...+1), and the register is multiplied mod p by
  x (which is shifting right by one and adding x^32 mod p if the bit shifted
  out is a one).  We start with the highest power (least significant bit) of
  q and repeat for all eight bits of q.

  The table is simply the CRC of all possible eight bit values.  This is all
  the information needed to generate CRC's on data a byte at a time for all
  combinations of CRC register values and incoming bytes.
*/
local void make_crc_table()
{
  uLong c;
  int n, k;
  uLong poly;            /* polynomial exclusive-or pattern */
  /* terms of polynomial defining this crc (except x^32): */
  static const Byte p[] = {0,1,2,4,5,7,8,10,11,12,16,22,23,26};

  /* make exclusive-or pattern from polynomial (0xedb88320L) */
  poly = 0L;
  for (n = 0; n < sizeof(p)/sizeof(Byte); n++)
    poly |= 1L << (31 - p[n]);
 
  for (n = 0; n < 256; n++)
  {
    c = (uLong)n;
    for (k = 0; k < 8; k++)
      c = c & 1 ? poly ^ (c >> 1) : c >> 1;
    crc_table[n] = c;
  }
  crc_table_empty = 0;
}
#else
/* ========================================================================
 * Table of CRC-32's of all single-byte values (made by make_crc_table)
 */
local const uLongf crc_table[256] = {
  0x00000000L, 0x77073096L, 0xee0e612cL, 0x990951baL, 0x076dc419L,
  0x706af48fL, 0xe963a535L, 0x9e6495a3L, 0x0edb8832L, 0x79dcb8a4L,
  0xe0d5e91eL, 0x97d2d988L, 0x09b64c2bL, 0x7eb17cbdL, 0xe7b82d07L,
  0x90bf1d91L, 0x1db71064L, 0x6ab020f2L, 0xf3b97148L, 0x84be41deL,
  0x1adad47dL, 0x6ddde4ebL, 0xf4d4b551L, 0x83d385c7L, 0x136c9856L,
  0x646ba8c0L, 0xfd62f97aL, 0x8a65c9ecL, 0x14015c4fL, 0x63066cd9L,
  0xfa0f3d63L, 0x8d080df5L, 0x3b6e20c8L, 0x4c69105eL, 0xd56041e4L,
  0xa2677172L, 0x3c03e4d1L, 0x4b04d447L, 0xd20d85fdL, 0xa50ab56bL,
  0x35b5a8faL, 0x42b2986cL, 0xdbbbc9d6L, 0xacbcf940L, 0x32d86ce3L,
  0x45df5c75L, 0xdcd60dcfL, 0xabd13d59L, 0x26d930acL, 0x51de003aL,
  0xc8d75180L, 0xbfd06116L, 0x21b4f4b5L, 0x56b3c423L, 0xcfba9599L,
  0xb8bda50fL, 0x2802b89eL, 0x5f058808L, 0xc60cd9b2L, 0xb10be924L,
  0x2f6f7c87L, 0x58684c11L, 0xc1611dabL, 0xb6662d3dL, 0x76dc4190L,
  0x01db7106L, 0x98d220bcL, 0xefd5102aL, 0x71b18589L, 0x06b6b51fL,
  0x9fbfe4a5L, 0xe8b8d433L, 0x7807c9a2L, 0x0f00f934L, 0x9609a88eL,
  0xe10e9818L, 0x7f6a0dbbL, 0x086d3d2dL, 0x91646c97L, 0xe6635c01L,
  0x6b6b51f4L, 0x1c6c6162L, 0x856530d8L, 0xf262004eL, 0x6c0695edL,
  0x1b01a57bL, 0x8208f4c1L, 0xf50fc457L, 0x65b0d9c6L, 0x12b7e950L,
  0x8bbeb8eaL, 0xfcb9887cL, 0x62dd1ddfL, 0x15da2d49L, 0x8cd37cf3L,
  0xfbd44c65L, 0x4db26158L, 0x3ab551ceL, 0xa3bc0074L, 0xd4bb30e2L,
  0x4adfa541L, 0x3dd895d7L, 0xa4d1c46dL, 0xd3d6f4fbL, 0x4369e96aL,
  0x346ed9fcL, 0xad678846L, 0xda60b8d0L, 0x44042d73L, 0x33031de5L,
  0xaa0a4c5fL, 0xdd0d7cc9L, 0x5005713cL, 0x270241aaL, 0xbe0b1010L,
  0xc90c2086L, 0x5768b525L, 0x206f85b3L, 0xb966d409L, 0xce61e49fL,
  0x5edef90eL, 0x29d9c998L, 0xb0d09822L, 0xc7d7a8b4L, 0x59b33d17L,
  0x2eb40d81L, 0xb7bd5c3bL, 0xc0ba6cadL, 0xedb88320L, 0x9abfb3b6L,
  0x03b6e20cL, 0x74b1d29aL, 0xead54739L, 0x9dd277afL, 0x04db2615L,
  0x73dc1683L, 0xe3630b12L, 0x94643b84L, 0x0d6d6a3eL, 0x7a6a5aa8L,
  0xe40ecf0bL, 0x9309ff9dL, 0x0a00ae27L, 0x7d079eb1L, 0xf00f9344L,
  0x8708a3d2L, 0x1e01f268L, 0x6906c2feL, 0xf762575dL, 0x806567cbL,
  0x196c3671L, 0x6e6b06e7L, 0xfed41b76L, 0x89d32be0L, 0x10da7a5aL,
  0x67dd4accL, 0xf9b9df6fL, 0x8ebeeff9L, 0x17b7be43L, 0x60b08ed5L,
  0xd6d6a3e8L, 0xa1d1937eL, 0x38d8c2c4L, 0x4fdff252L, 0xd1bb67f1L,
  0xa6bc5767L, 0x3fb506ddL, 0x48b2364bL, 0xd80d2bdaL, 0xaf0a1b4cL,
  0x36034af6L, 0x41047a60L, 0xdf60efc3L, 0xa867df55L, 0x316e8eefL,
  0x4669be79L, 0xcb61b38cL, 0xbc66831aL, 0x256fd2a0L, 0x5268e236L,
  0xcc0c7795L, 0xbb0b4703L, 0x220216b9L, 0x5505262fL, 0xc5ba3bbeL,
  0xb2bd0b28L, 0x2bb45a92L, 0x5cb36a04L, 0xc2d7ffa7L, 0xb5d0cf31L,
  0x2cd99e8bL, 0x5bdeae1dL, 0x9b64c2b0L, 0xec63f226L, 0x756aa39cL,
  0x026d930aL, 0x9c0906a9L, 0xeb0e363fL, 0x72076785L, 0x05005713L,
  0x95bf4a82L, 0xe2b87a14L, 0x7bb12baeL, 0x0cb61b38L, 0x92d28e9bL,
  0xe5d5be0dL, 0x7cdcefb7L, 0x0bdbdf21L, 0x86d3d2d4L, 0xf1d4e242L,
  0x68ddb3f8L, 0x1fda836eL, 0x81be16cdL, 0xf6b9265bL, 0x6fb077e1L,
  0x18b74777L, 0x88085ae6L, 0xff0f6a70L, 0x66063bcaL, 0x11010b5cL,
  0x8f659effL, 0xf862ae69L, 0x616bffd3L, 0x166ccf45L, 0xa00ae278L,
  0xd70dd2eeL, 0x4e048354L, 0x3903b3c2L, 0xa7672661L, 0xd06016f7L,
  0x4969474dL, 0x3e6e77dbL, 0xaed16a4aL, 0xd9d65adcL, 0x40df0b66L,
  0x37d83bf0L, 0xa9bcae53L, 0xdebb9ec5L, 0x47b2cf7fL, 0x30b5ffe9L,
  0xbdbdf21cL, 0xcabac28aL, 0x53b39330L, 0x24b4a3a6L, 0xbad03605L,
  0xcdd70693L, 0x54de5729L, 0x23d967bfL, 0xb3667a2eL, 0xc4614ab8L,
  0x5d681b02L, 0x2a6f2b94L, 0xb40bbe37L, 0xc30c8ea1L, 0x5a05df1bL,
  0x2d02ef8dL
};
#endif

/* =========================================================================
 * This function can be used by asm versions of crc32()
 */
const uLongf * ZEXPORT get_crc_table()
{
#ifdef DYNAMIC_CRC_TABLE
  if (crc_table_empty) make_crc_table();
#endif
  return (const uLongf *)crc_table;
}

/* ========================================================================= */
#define DO1(buf) crc = crc_table[((int)crc ^ (*buf++)) & 0xff] ^ (crc >> 8);
#define DO2(buf)  DO1(buf); DO1(buf);
#define DO4(buf)  DO2(buf); DO2(buf);
#define DO8(buf)  DO4(buf); DO4(buf);

/* ========================================================================= */
//uLong ZEXPORT crc32(crc, buf, len)
//    uLong crc;
//    const Bytef *buf;
//    uInt len;
uLong crc32(uLong crc, const Bytef *buf, uInt len)
{
    if (buf == Z_NULL) return 0L;
#ifdef DYNAMIC_CRC_TABLE
    if (crc_table_empty)
      make_crc_table();
#endif
    crc = crc ^ 0xffffffffL;
    while (len >= 8)
    {
      DO8(buf);
      len -= 8;
    }
    if (len) do {
      DO1(buf);
    } while (--len);
    return crc ^ 0xffffffffL;
}

//-------------struct
struct TsProgramNumbers
{
  TsProgramNumbers() : pProgramPid(0), pProgramNum(0) {}
  Ipp16u pProgramPid;
  Ipp16u pProgramNum;
};
class PATTableWriterParams
{
public:
  PATTableWriterParams()
  {
    iNOfPrograms = 0;
    pProgramNumbers = NULL;
    transportStreamId = MPEG2MUX_TS_ID;
  }

  Ipp32s iNOfPrograms;
  TsProgramNumbers* pProgramNumbers;
  Ipp16u transportStreamId;
};
class PMTTableWriterParams
{
public:
  PMTTableWriterParams()
  {
    uiProgramPID = 0;
    uiProgramNumber = 0;
    uiNOfStreams = 0;
    pStreamType = NULL;
    pPID = NULL;
    uiPCRPID = 0;
  }

  Ipp32u uiProgramPID;
  Ipp32u uiProgramNumber;
  Ipp32u uiNOfStreams;
  MPEG2MuxerESType *pStreamType;
  Ipp32u *pPID;
  Ipp32u uiPCRPID;
};
//-------------bit stream
class AdvancedBitStream
{
public:
  // Default constructor
  AdvancedBitStream(void);
  // Reset bit stream
  void Reset(void *lpvBuffer, Ipp32s lSize, bool bReset = false);
  // Set bit pointer
  void SetPointer(Ipp32s lBitsFromBegining);
  // Skip bits in stream
  void SkipBits(Ipp32s lSkip);
  // Insert bits in stream
  void AddBits(Ipp32u nBits, Ipp32s lCount);

  Ipp8u *m_lpbBuffer; // pointer to begining of buffer
  Ipp8u *m_lpb; // current position in buffer
  Ipp32s m_lBufferLength; // buffer size
  Ipp8u m_bRegistr; // register
  Ipp32s m_lRemainBits; // available bits in register
};


AdvancedBitStream::AdvancedBitStream(void)
{
    Reset(NULL, 0);
} //AdvancedBitStream::AdvancedBitStream(void)

void ippsZero_8u(void* buff, int size)
{
  memset(buff, 0, size);
}
void AdvancedBitStream::Reset(void *lpvBuffer, Ipp32s lSize, bool bReset)
{
    if (bReset)
        ippsZero_8u((Ipp8u *)lpvBuffer, lSize);
    m_lpbBuffer = reinterpret_cast<Ipp8u *> (lpvBuffer);
    m_lpb = m_lpbBuffer;
    m_lBufferLength = lSize;
    m_bRegistr = 0;
    m_lRemainBits = 8;
} //void AdvancedBitStream::Reset(void *lpvBuffer, Ipp32s lSize, bool bReset)

void AdvancedBitStream::SetPointer(Ipp32s lBitsFromBegining)
{
    if ((lBitsFromBegining / 8) < m_lBufferLength)
    {
        m_lpb = m_lpbBuffer + lBitsFromBegining / 8;
        m_bRegistr = *m_lpb;
        m_lRemainBits = 8 - (lBitsFromBegining % 8);
    }
} //void AdvancedBitStream::SetPointer(Ipp32s lBitsFromBegining)

void AdvancedBitStream::SkipBits(Ipp32s lSkip)
{
    if (m_lpbBuffer + m_lBufferLength > m_lpb)
    {
        if (m_lRemainBits <= lSkip)
        {
            lSkip -= m_lRemainBits;
            m_lRemainBits = 8;
            m_lpb += 1;
            m_bRegistr = *m_lpb;

            if (lSkip)
                SkipBits(lSkip);
        }
        else
            m_lRemainBits -= lSkip;
    }
} //void AdvancedBitStream::SkipBits(Ipp32s lSkip)


void AdvancedBitStream::AddBits(Ipp32u nBits, Ipp32s lCount)
{
    if (m_lpbBuffer + m_lBufferLength > m_lpb)
    {
        if (lCount < m_lRemainBits)
        {
            m_lRemainBits -= lCount;
            nBits <<= m_lRemainBits;
            m_bRegistr &= ~((~((0xffffffff) << lCount)) << m_lRemainBits);
            m_bRegistr |= static_cast<Ipp8u> (nBits);
            *m_lpb = m_bRegistr;
        }
        else
        {
            Ipp32u nPrefix;

            nPrefix = nBits >> (lCount - m_lRemainBits);
            m_bRegistr &= ((0xffffffff) << (m_lRemainBits));
            m_bRegistr |= static_cast<Ipp8u> (nPrefix);
            *m_lpb = m_bRegistr;
            lCount -= m_lRemainBits;

            m_lRemainBits = 8;
            m_lpb += 1;
            m_bRegistr = *m_lpb;

            if (lCount)
            {
                Ipp32u nPostfix;

                nPostfix = nBits & ~(0xffffffff << lCount);

                AddBits(nPostfix, lCount);
            }
        }
    }
} //void AdvancedBitStream::AddBits(Ipp32u nBits, Ipp32s lCount)


class TsPacket
{
public:
//-----------TS PACKET
  int generateTsHdr(
    void* buff,
    int size,
    int iPID,
    int iCounter,
    int iStartIndicator = 0,
    int iAdaptFieldCtrl = 0
    );
  int generateAdaptationField(
    void* buff,
    int size,
    int iAdaptFieldCtrl,
    int iAdaptFieldLen,
    int iPCRFlag
    );
  int generatePCR(
    void* buff,
    int size,
    double dPCR
    );
  int generatePCR(
    void* buff,
    int size,
    uint64_t uiPCR
    );
  int generatePESHeader(
    void* buff,
    int size,
    int iStreamID,
    int iPESPacketLength,
    int iPTSDTSFlags,
    int iPESHeaderDataLength
    );
  int generatePTS(
    void* buff,
    int size,
    int iPTSDTSFlags,
    double dPTS,
    double dDTS
    );
  int generatePTS(
    void* buff,
    int size,
    int iPTSDTSFlags,
    int64_t iPTS,
    int64_t iDTS
    );
  int generateStuffingBytes(
    void* buff,
    int size,
    int iNOfTSStuffingBytes
    );

  //+ packetize first ES partial
  int createChunk_PES(
    void* buff,
    int size,
    int iPID,
    int iCounter,
    void* ESData,
    int ESSize,
    double PTS,
    double DTS,
    int optLPCM,
    int* piNOfFrames,
    int* piNextOffset
    );
  int createChunk_PES(
    void* buff,
    int size,
    int iPID,
    int iCounter,
    void* ESData,
    int ESSize,
    int64_t iPTS,
    int64_t iDTS,
    int optLPCM,
    int* piNOfFrames,
    int* piNextOffset
    );
  //+ packetize PCR
  int createChunk_PCR(
    void* buff,
    int size,
    int iPID,
    int iCounter,
    double dPCR
    );
  //+ packetize normal ES partial
  int createChunk_NORMAL(
    void* buff,
    int size,
    int iPID,
    int iCounter,
    void* ESData,
    int remain
    );
  //+ packet 192 - 4 bytes time stamp + 188 bytes
  int createTimeStamp(
    void* buff,
    int size,
    int zeroTs,
    double dReferenceClock
    );
  int createTimeStamp(
    void* buff,
    int size,
    int zeroTs,
    int64_t iPTS
    );
  //-----------PAT
  typedef class PATTableWriterParams PATTableWriterParams;
  int createPAT(
    void* buff,
    int size,
    PATTableWriterParams *pParams,
    int* piCounter
    );
  int updatePAT(
    void* buff,
    int size,
    int* piCounter
    );
  //-----------PMT
  //+ PMTID = 0x0100
  typedef class PMTTableWriterParams PMTTableWriterParams;
  int createPMT(
    void* buff,
    int size,
    PMTTableWriterParams *pParams,
    int* piCounter
    );
  int updatePMT(
    void* buff,
    int size,
    int* piCounter
    );
  //continuity counter
  int m_iCounter;
  PATTableWriterParams* m_pPATParams;
  PMTTableWriterParams** m_ppPMTParams;
};

//no ES size greater than 2^31
//packetize es parital
int TsPacket::
generateTsHdr(
  void* buff,
  int size,
  int iPID,
  int iCounter,
  int iStartIndicator,
  int iAdaptFieldCtrl
  )
{
  int iTsHdrLen = 4;
  AdvancedBitStream bs;
  uint8_t* pHeaderBuf = (uint8_t*)buff;

  DU_ENSURE_RETURN2(ERROR, size >= iTsHdrLen, -1);
  bs.Reset(pHeaderBuf, iTsHdrLen);

  bs.AddBits(0x47, 8);             // sync_byte
  bs.AddBits(0x00, 1);             // transport_error_indicator
  bs.AddBits(iStartIndicator, 1);  // payload_unit_start_indicator
  bs.AddBits(0x00, 1);             // transport_priority
  bs.AddBits(iPID, 13);            // PID
  bs.AddBits(0x00, 2);             // transport_scrambling_control
  bs.AddBits(iAdaptFieldCtrl, 2);  // adaptation_field_control
  bs.AddBits(iCounter, 4);         // continuity_counter

  return iTsHdrLen;
}
//packetize first partial es data
//packetize last partial es data
int TsPacket::
generateAdaptationField(
  void* buff,
  int size,
  int iAdaptFieldCtrl,
  int iAdaptFieldLen,
  int iPCRFlag
  )
{
  int iFieldLen = 1;
  AdvancedBitStream bs;
  uint8_t* pHeaderBuf = (uint8_t*)buff;

  DU_ENSURE_RETURN2(ERROR, size >= iFieldLen, -1);
  bs.Reset(pHeaderBuf, iFieldLen);

  DU_ENSURE_RETURN2(ERROR, 0x03 == iAdaptFieldCtrl, -1);
  DU_ENSURE_RETURN2(ERROR, iAdaptFieldLen > 0, -1);

  bs.AddBits(iAdaptFieldLen, 8);  // adaptation_field_length
  //if (iAdaptFieldLen > 0)
  {
    bs.AddBits(0x00, 1);            // discontinuity_indicator
    bs.AddBits(0x00, 1);            // random_access_indicator
    bs.AddBits(0x00, 1);            // elementary_stream_priority_indicator
    bs.AddBits(iPCRFlag, 1);        // PCR_flag
    bs.AddBits(0x00, 1);            // OPCR_flag
    bs.AddBits(0x00, 1);            // splicing_point_flag
    bs.AddBits(0x00, 1);            // transport_private_data_flag
    bs.AddBits(0x00, 1);            // adaptation_field_extension_flag
    iFieldLen++;
  }
  return iFieldLen;
}
int TsPacket::
generatePCR(
  void* buff,
  int size,
  double dPCR
  )
{
  return generatePCR(
    buff,
    size,
    (uint64_t)(dPCR * 27000000 + 0.5)
    );
}
int TsPacket::
generatePCR(
  void* buff,
  int size,
  uint64_t uiPCR
  )
{
  int iFieldLen = 4;
  AdvancedBitStream bs;
  uint8_t* pHeaderBuf = (uint8_t*)buff;

  DU_ENSURE_RETURN2(ERROR, size >= iFieldLen, -1);
  bs.Reset(pHeaderBuf, iFieldLen);

  uint64_t pcr_base = (uiPCR / 300) & CONST_LL(0x1FFFFFFFF);
  uint32_t pcr_ext = (uint32_t)((uiPCR % 300) & 0x1FF);

  bs.AddBits((int32_t)(pcr_base >> 32), 1);         // program_clock_reference_base[32..32]
  bs.AddBits((int32_t)(pcr_base & 0xFFFFFFFF), 32); // program_clock_reference_base[31..0]
  bs.AddBits(0x3F, 6);                              // reserved
  bs.AddBits((pcr_ext), 9);                         // program_clock_reference_extension

  return iFieldLen;
}
int TsPacket::
generatePESHeader(
  void* buff,
  int size,
  int iStreamID,
  int iPESPacketLength,
  int iPTSDTSFlags,
  int iPESHeaderDataLength
  )
{
  int iFieldLen = 9;
  AdvancedBitStream bs;
  uint8_t* pHeaderBuf = (uint8_t*)buff;

  DU_ENSURE_RETURN2(ERROR, size >= iFieldLen, -1);
  bs.Reset(pHeaderBuf, iFieldLen);

  bs.AddBits(0x000001, 24);               // PES start code
  bs.AddBits(iStreamID, 8);               // stream_id
  bs.AddBits(iPESPacketLength, 16);       // PES_packet_length
  bs.AddBits(0x02, 2);                    // '10'
  bs.AddBits(0x00, 2);                    // PES_scrambling_control
  bs.AddBits(0x00, 1);                    // PES_priority
  bs.AddBits(0x00, 1);                    // data_alignment_indicator
  bs.AddBits(0x00, 1);                    // copyright
  bs.AddBits(0x00, 1);                    // original_or_copy
  bs.AddBits(iPTSDTSFlags, 2);            // PTS_DTS_flags
  bs.AddBits(0x00, 1);                    // ESCR_flag
  bs.AddBits(0x00, 1);                    // ES_rate_flag
  bs.AddBits(0x00, 1);                    // DSM_trick_mode_flag
  bs.AddBits(0x00, 1);                    // additional_copy_info_flag
  bs.AddBits(0x00, 1);                    // PES_CRC_flag
  bs.AddBits(0x00, 1);                    // PES_extension_flag
  bs.AddBits(iPESHeaderDataLength, 8);    // PES_header_data_length

  return iFieldLen;
}
int TsPacket::
generatePTS(
  void* buff,
  int size,
  int iPTSDTSFlags,
  double dPTS,
  double dDTS
  )
{
  return generatePTS(buff,
    size,
    iPTSDTSFlags,
    (int64_t)(dPTS * 90000.0 + 0.5),
    (int64_t)(dDTS * 90000.0 + 0.5)
    );
}
int TsPacket::
generatePTS(
  void* buff,
  int size,
  int iPTSDTSFlags,
  int64_t pts,
  int64_t dts
  )
{
  int iFieldLen = (iPTSDTSFlags==0x03)?10:5;
  AdvancedBitStream bs;
  uint8_t* pHeaderBuf = (uint8_t*)buff;

  DU_ENSURE_RETURN2(ERROR, size >= iFieldLen, -1);
  bs.Reset(pHeaderBuf, iFieldLen);

  bs.AddBits(iPTSDTSFlags & 3, 4);             // '0010' or '0011'
  bs.AddBits((int32_t)((pts >> 30) & 0x07), 3);    // PTS [32..30]
  bs.AddBits(0x01, 1);                            // marker_bit
  bs.AddBits((int32_t)((pts >> 15) & 0x7fff), 15); // PTS [29..15]
  bs.AddBits(0x01, 1);                            // marker_bit
  bs.AddBits((int32_t)(pts & 0x7fff), 15);         // PTS [14..0]
  bs.AddBits(0x01, 1);                            // marker_bit

  if (0x03 == iPTSDTSFlags)
  {
    bs.AddBits(0x01, 4);                            // '0001'
    bs.AddBits((int32_t)((dts >> 30) & 0x07), 3);    // DTS [32..30]
    bs.AddBits(0x01, 1);                            // marker_bit
    bs.AddBits((int32_t)((dts >> 15) & 0x7fff), 15); // DTS [29..15]
    bs.AddBits(0x01, 1);                            // marker_bit
    bs.AddBits((int32_t)(dts & 0x7fff), 15);         // DTS [14..0]
    bs.AddBits(0x01, 1);                            // marker_bit
  }

  return iFieldLen;
}
int TsPacket::
generateStuffingBytes(
  void* buff,
  int size,
  int iNOfTSStuffingBytes
  )
{
  AdvancedBitStream bs;
  uint8_t* pHeaderBuf = (uint8_t*)buff;

  DU_ENSURE_RETURN2(ERROR, size >= iNOfTSStuffingBytes, -1);
  bs.Reset(pHeaderBuf, iNOfTSStuffingBytes);

  for (int i = 0; i < iNOfTSStuffingBytes; i++)
  {
    bs.AddBits(0xFF, 8);        // stuffing byte
  }

  return iNOfTSStuffingBytes;
}
//packetize first es partial
int TsPacket::
createChunk_PES(
  void* buff,
  int size,
  int iPID,
  int iCounter,
  void* ESData,
  int ESSize,
  double dPTS,
  double dDTS,
  int optLPCM,
  int* piNOfFrames,
  int* piNextOffset
  )
{
  return createChunk_PES(
    buff,
    size,
    iPID,
    iCounter,
    ESData,
    ESSize,
    (int64_t)(dPTS * 90000.0 + 0.5),
    (int64_t)(dDTS * 90000.0 + 0.5),
    optLPCM,
    piNOfFrames,
    piNextOffset
    );
}

int TsPacket::
createChunk_PES(
  void* buff,
  int size,
  int iPID,
  int iCounter,
  void* ESData,
  int ESSize,
  int64_t iPTS,
  int64_t iDTS,
  int optLPCM,
  int* piNOfFrames,
  int* piNextOffset
  )
{
  // PES section
  assert((iPID == VIDEO_PID) || (iPID == AUDIO_PID));
  int iStreamID = (iPID==VIDEO_PID)?VIDEO_STREAM_ID:AUDIO_STREAM_ID;
  int iPESPacketLength = 0;
  int iPTSDTSFlags = 0x00;
  int iPESHeaderDataLength = 0;
  int iNOfStuffingBytes = 0;

  // TS section
  int iMaxPayloadSize = MPEG2MUX_TS_PACKET_LENGTH;
#if(PES_PCR)
  int iAdaptFieldCtrl = 0x03;   //PCR
  int iAdaptFieldLen  = 7;
  int iPCRFlag = 1;
#else
  int iAdaptFieldCtrl = 0x01;
#endif

  int iSpecialHeaderSize = 0;
  int iOffset = 0;
  int& iNOfFrames = *piNOfFrames;
  int& iNextOffset = *piNextOffset;

  iMaxPayloadSize -= 4; // obligatory TS packet fields
#if(PES_PCR)
  iMaxPayloadSize -= 8;         //PCR
#endif

  //[PTS]
  iPTSDTSFlags = 0x02;
  iPESHeaderDataLength += 5;
  if (iDTS > 1)
  {
    iPTSDTSFlags = 0x03;
    iPESHeaderDataLength += 5;
  }

  iPESPacketLength = ESSize;
  iPESPacketLength += iSpecialHeaderSize; // special header is needed for LPCM
  iPESPacketLength += iPESHeaderDataLength; // PTS/DTS
  iPESPacketLength += 3; // PES flags and PES_header_data_length field
  if (iPESPacketLength > 0xFFFF)
  {
    iPESPacketLength = 0;
  }

  iMaxPayloadSize -= iSpecialHeaderSize;
  iMaxPayloadSize -= iPESHeaderDataLength; // PTS/DTS
  iMaxPayloadSize -= 3; // PES flags and PES_header_data_length field
  iMaxPayloadSize -= 6; // PES_start_code and PES_packet_length

  if (optLPCM)
  {
    iNOfFrames = (ESSize - iOffset + MPEG2MUX_LPCM_FRAME_SIZE - 1) / MPEG2MUX_LPCM_FRAME_SIZE;
    iNextOffset = iOffset + iNOfFrames * MPEG2MUX_LPCM_FRAME_SIZE - ESSize;
  }

  if (ESSize < iMaxPayloadSize)
  {
    iNOfStuffingBytes = iMaxPayloadSize - ESSize;
    iPESHeaderDataLength += iNOfStuffingBytes;
    iMaxPayloadSize -= iNOfStuffingBytes;
  }

  if (optLPCM && (ESSize & 1))
  { // even number of byte of LPCM should be written
    iMaxPayloadSize--;
    iNOfStuffingBytes++;
    iPESHeaderDataLength++;
  }

  //fill packet header space
  uint8_t* pHeaderBuf = (uint8_t*)buff;
  int len = generateTsHdr(pHeaderBuf,
    size,
    iPID,
    iCounter,
    1,                    //start indicator = 1
    iAdaptFieldCtrl);
# if (PES_PCR)
  len += generateAdaptationField(pHeaderBuf + len, size - len,
    iAdaptFieldCtrl, iAdaptFieldLen, iPCRFlag);
  len += generatePCR(pHeaderBuf + len, size - len, (uint64_t)iPTS);
#endif
  len += generatePESHeader(pHeaderBuf + len,
    size - len,
    iStreamID,
    iPESPacketLength,
    iPTSDTSFlags,
    iPESHeaderDataLength);

  len += generatePTS(pHeaderBuf + len,
    size - len,
    iPTSDTSFlags,
    iPTS,
    iDTS);

  len += generateStuffingBytes(pHeaderBuf + len,
    size - len,
    iNOfStuffingBytes);

  //fill packet payload space
  memcpy_s(pHeaderBuf + len, size - len, ESData, iMaxPayloadSize);

  return iMaxPayloadSize;
}
//packetize PCR packet
int TsPacket::
createChunk_PCR(
  void* buff,
  int size,
  int iPID,
  int iCounter,
  double dPCR
  )
{
  int iPCRFlag = 1;
  int iAdaptFieldCtrl = 0x02;
  int iAdaptFieldLen = 183;

  uint8_t* pHeaderBuf = (uint8_t*)buff;
  int len = generateTsHdr(pHeaderBuf,
    size,
    iPID,             //PCRID = 0x1001
    iCounter,
    0,                //start indicator = 0
    iAdaptFieldCtrl);

  len += generateAdaptationField(pHeaderBuf + len,
    size - len,
    iAdaptFieldCtrl,
    iAdaptFieldLen,
    iPCRFlag);

  len += generatePCR(pHeaderBuf + len,
    size - len,
    dPCR);

  int iNOfTSStuffingBytes = MPEG2MUX_TS_PACKET_LENGTH - len;
  len += generateStuffingBytes(pHeaderBuf + len,
    size - len,
    iNOfTSStuffingBytes);

  return 0;
}

int TsPacket::
createChunk_NORMAL(
  void* buff,
  int size,
  int iPID,
  int iCounter,
  void* ESData,
  int remain
  )
{
  int iMaxPayloadSize = MPEG2MUX_TS_PACKET_LENGTH;
  int iAdaptFieldCtrl = 0x01;
  int iAdaptFieldLen = 0;
  int iPCRFlag = 0;
  int iNOfTSStuffingBytes = 0;

  iMaxPayloadSize -= 4; // obligatory TS packet fields

  if (remain < iMaxPayloadSize)
  {
    iAdaptFieldCtrl = 0x03;
    iAdaptFieldLen = iMaxPayloadSize - remain - 1;
    if (iAdaptFieldLen > 0)
      iNOfTSStuffingBytes = iAdaptFieldLen - 1;
  }

  uint8_t* pHeaderBuf = (uint8_t*)buff;
  int len = generateTsHdr(pHeaderBuf,
    size,
    iPID,
    iCounter,
    0,                //start indicator = 0
    iAdaptFieldCtrl);

  if (remain < iMaxPayloadSize)
  {
    len += generateAdaptationField(pHeaderBuf + len,
      size - len,
      iAdaptFieldCtrl,
      iAdaptFieldLen,
      iPCRFlag);

    len += generateStuffingBytes(pHeaderBuf + len,
      size - len,
      iNOfTSStuffingBytes);

    iMaxPayloadSize = remain;
  }

  //fill packet payload space
  memcpy_s(pHeaderBuf + len, size - len, ESData, iMaxPayloadSize);

  return iMaxPayloadSize;
}
int TsPacket::createTimeStamp(
  void* buff,
  int size,
  int zeroTs,
  double dReferenceClock
  )
{
  int32_t iToWrite = 4;
  uint32_t uiTimeStamp = zeroTs ? 0 : LITTLE_ENDIAN_SWAP32((uint32_t)(dReferenceClock * MPEG2MUX_SYS_CLOCK_FREQ));
  memcpy_s(buff, size, &uiTimeStamp, iToWrite);
  return 0;
}
int TsPacket::createTimeStamp(
  void* buff,
  int size,
  int zeroTs,
  int64_t iPTS
  )
{
  int32_t iToWrite = 4;
  uint32_t uiTimeStamp = zeroTs ? 0 : LITTLE_ENDIAN_SWAP32((uint32_t)(iPTS * 300));
  memcpy_s(buff, size, &uiTimeStamp, iToWrite);
  return 0;
}
//-------------PAT
int TsPacket::
createPAT(
  void* buff,
  int size,
  PATTableWriterParams *pParams,
  int* piCounter
  )
{
  AdvancedBitStream bs;
  uint8_t* pHeaderBuf = (uint8_t*) buff;
  int& iCounter = *piCounter;

  DU_ENSURE_RETURN2(ERROR, size >= MPEG2MUX_TS_PACKET_LENGTH, -1);
  memset(buff, -1, MPEG2MUX_TS_PACKET_LENGTH);
  bs.Reset(pHeaderBuf, MPEG2MUX_TS_PACKET_LENGTH);

  // write Program Association Table
  bs.AddBits(0x47, 8);                        // sync_byte
  bs.AddBits(0x00, 1);                        // transport_error_indicator
  bs.AddBits(0x01, 1);                        // payload_unit_start_indicator
  bs.AddBits(0x00, 1);                        // transport_priority
  bs.AddBits(0x00, 13);                       // PID
  bs.AddBits(0x00, 2);                        // transport_scrambling_control
  bs.AddBits(0x01, 2);                        // adaptation_field_control
  bs.AddBits(iCounter, 4);                    // continuity_counter
  bs.AddBits(0x00, 8);                        // pointer_field
  bs.AddBits(0x00, 8);                        // table_id
  bs.AddBits(0x01, 1);                        // section_syntax_indicator
  bs.AddBits(0x00, 1);                        // '0'
  bs.AddBits(0x03, 2);                        // reserved
  bs.AddBits(4 * pParams->iNOfPrograms + 9, 12);  // section_length
  bs.AddBits(pParams->transportStreamId, 16); // transport_stream_id
  bs.AddBits(0x03, 2);                        // reserved
  bs.AddBits(0x00, 5);                        // version_number
  bs.AddBits(0x01, 1);                        // current_next_indicator
  bs.AddBits(0x00, 8);                        // section_number
  bs.AddBits(0x00, 8);                        // last_section_number

  int32_t i;
  for (i = 0; i < pParams->iNOfPrograms; i++)
  {
    bs.AddBits(pParams->pProgramNumbers[i].pProgramNum, 16);    // program_number
    bs.AddBits(0x07, 3);                                        // reserved
    bs.AddBits(pParams->pProgramNumbers[i].pProgramPid, 13);    // program_map_PID
  }

  uint32_t uiCRCDataLen = 4 * pParams->iNOfPrograms + 8;
  uint32_t uiCRC32 = 0;
  //ippsCRC32_BZ2_8u(bs.m_lpb - uiCRCDataLen, uiCRCDataLen, &uiCRC32);
  //uiCRC32 ^= 0xFFFFFFFF;
  uiCRC32 = crc32(0, bs.m_lpb - uiCRCDataLen, uiCRCDataLen);
  //uiCRC32 ^= 0xFFFFFFFF;
  bs.AddBits(uiCRC32, 32);

  // update continuity_counter
  iCounter = (iCounter + 1) & 0x0f;

  return 0;
}
int TsPacket::updatePAT(
  void* buff,
  int size,
  int* piCounter
  )
{
  AdvancedBitStream bs;
  uint8_t* pHeaderBuf = (uint8_t*) buff;
  int& iCounter = *piCounter;

  DU_ENSURE_RETURN2(ERROR, size >= MPEG2MUX_TS_PACKET_LENGTH, -1);
  bs.Reset(pHeaderBuf, MPEG2MUX_TS_PACKET_LENGTH);

  bs.SkipBits(8 + 1 + 1 + 1 + 13 + 2 + 2);
  bs.AddBits(iCounter, 4);

  // update continuity_counter
  iCounter = (iCounter + 1) & 0x0f;

  return 0;
}

int TsPacket::createPMT(
  void* buff,
  int size,
  PMTTableWriterParams *pParams,
  int* piCounter
  )
{
  AdvancedBitStream bs;
  uint8_t* pHeaderBuf = (uint8_t*) buff;
  int& iCounter = *piCounter;

  DU_ENSURE_RETURN2(ERROR, size >= MPEG2MUX_TS_PACKET_LENGTH, -1);
  memset(buff, -1, MPEG2MUX_TS_PACKET_LENGTH);
  bs.Reset(pHeaderBuf, MPEG2MUX_TS_PACKET_LENGTH);

  // write Program Map Table
  bs.AddBits(0x47, 8);                        // sync_byte
  bs.AddBits(0x00, 1);                        // transport_error_indicator
  bs.AddBits(0x01, 1);                        // payload_unit_start_indicator
  bs.AddBits(0x00, 1);                        // transport_priority
  bs.AddBits(pParams->uiProgramPID, 13);      // PID
  bs.AddBits(0x00, 2);                        // transport_scrambling_control
  bs.AddBits(0x01, 2);                        // adaptation_field_control
  bs.AddBits(iCounter, 4);                    // continuity_counter
  bs.AddBits(0x00, 8);                        // pointer_field
  bs.AddBits(0x02, 8);                        // table_id
  bs.AddBits(0x01, 1);                        // section_syntax_indicator
  bs.AddBits(0x00, 1);                        // '0'
  bs.AddBits(0x03, 2);                        // reserved
  bs.AddBits(13 + 5 * pParams->uiNOfStreams, 12);    // section_length
  bs.AddBits(pParams->uiProgramNumber, 16);          // program_number
  bs.AddBits(0x03, 2);                        // reserved
  bs.AddBits(0x00, 5);                        // version_number
  bs.AddBits(0x01, 1);                        // current_next_indicator
  bs.AddBits(0x00, 8);                        // section_number
  bs.AddBits(0x00, 8);                        // last_section_number
  bs.AddBits(0x07, 3);                        // reserved
  bs.AddBits(pParams->uiPCRPID, 13);                 // PRC_PID
  bs.AddBits(0x0F, 4);                        // reserved
  bs.AddBits(0x00, 12);                       // program_info_length

  uint32_t uiNum;
  for (uiNum = 0; uiNum < pParams->uiNOfStreams; uiNum++)
  {
    bs.AddBits(pParams->pStreamType[uiNum],  8);   // stream_type
    bs.AddBits(0x07,  3);                   // reserved
    bs.AddBits(pParams->pPID[uiNum], 13);          // elementary_PID
    bs.AddBits(0x0F,  4);                   // reserved
    bs.AddBits(0x00, 12);                   // ES_info_length
  }

  uint32_t uiCRC32 = 0;
  //uint32_t uiCRCDataLen = 12 + 5 * m_uiNOfStreams;
  //ippsCRC32_BZ2_8u(bs.m_lpb - uiCRCDataLen, uiCRCDataLen, &uiCRC32);
  uiCRC32 ^= 0xFFFFFFFF;
  bs.AddBits(uiCRC32, 32);                    // CRC_32

  // update continuity_counter
  iCounter = (iCounter + 1) & 0x0f;

  return 0;
}
int TsPacket::updatePMT(
  void* buff,
  int size,
  int* piCounter
  )
{
  AdvancedBitStream bs;
  uint8_t* pHeaderBuf = (uint8_t*) buff;
  int& iCounter = *piCounter;

  DU_ENSURE_RETURN2(ERROR, size >= MPEG2MUX_TS_PACKET_LENGTH, -1);
  bs.Reset(pHeaderBuf, MPEG2MUX_TS_PACKET_LENGTH);

  bs.SkipBits(8 + 1 + 1 + 1 + 13 + 2 + 2);
  bs.AddBits(iCounter, 4);

  // update continuity_counter
  iCounter = (iCounter + 1) & 0x0f;

  return 0;
}

int fileSize(FILE* f)
{
  int curPos = ftell(f);
  fseek(f, 0, SEEK_END);
  int size = ftell(f);
  fseek(f, curPos, SEEK_SET);
  return size;
}

//int m_iCounter = 0;
PATTableWriterParams* m_pPATParams = 0;
PMTTableWriterParams** m_ppPMTParams = 0;

//pack one ES
int pack(uint8_t* buff, int size, uint8_t* data, int dataSize, int iCounter, int64_t iPts, int64_t iDts)
{
  uint8_t* ESData = (uint8_t*)data;
  int ESSize = dataSize;

  int len = 0;        //ts data len
  int nBytes = 0;     //packetized ES data

  //+ paketize PAT, PMT
  if (0)
  {
    TsPacket().createTimeStamp(buff + len, size - len, 1, iPts);
    len += TIME_STAMP_SIZE;
    TsPacket().createPAT(buff + len, size - len, m_pPATParams, &iCounter);
    len += PACKET_SIZE;
    for (int i = 0; i < m_pPATParams->iNOfPrograms; i++)
    {
      TsPacket().createTimeStamp(buff + len, size - len, 1, iPts);
      len += TIME_STAMP_SIZE;
      TsPacket().createPMT(buff + len, size - len, m_ppPMTParams[i], &iCounter);
      len += PACKET_SIZE;
    }
  }

  //+ packetize first ES data parital
  TsPacket().createTimeStamp(buff + len, size - len, 1, iPts);
  len += TIME_STAMP_SIZE;
  nBytes = TsPacket().createChunk_PES(buff + len,
    size - len,
    VIDEO_PID,
    iCounter,
    ESData,
    ESSize,
    iPts,    //PTS
    iDts,    //DTS
    0,       //optLPCM = 0
    NULL,
    NULL);
  len += PACKET_SIZE;
  iCounter = INC_COUNTER(iCounter);
  for (;(size > len) && (ESSize > nBytes);) {
    TsPacket().createTimeStamp(buff + len, size - len, 1, iPts);
    len += TIME_STAMP_SIZE;
    nBytes += TsPacket().createChunk_NORMAL(
      buff + len,
      size - len,
      VIDEO_PID,
      iCounter,
      ESData + nBytes,
      ESSize - nBytes
      );
    len += PACKET_SIZE;
    iCounter = INC_COUNTER(iCounter);
  }
  return len;
}

int pack_first(uint8_t* buff, int size, int iPID, uint8_t* data, int dataSize, int iCounter, int64_t iPts, int64_t iDts, int64_t iTimeStamp)
{
  uint8_t* ESData = (uint8_t*)data;
  int ESSize = dataSize;

  int len = 0;        //ts data len
  int nBytes = 0;     //packetized ES data

  //+ packetize first ES data parital
  TsPacket().createTimeStamp(buff + len, size - len,
    0,            //use input iTimeStamp
    iTimeStamp);
  len += TIME_STAMP_SIZE;
  nBytes = TsPacket().createChunk_PES(buff + len,
    size - len,
    iPID,
    iCounter,
    ESData,
    ESSize,
    iPts,    //PTS
    iDts,    //DTS
    0,       //optLPCM = 0
    NULL,
    NULL);
  len += PACKET_SIZE;

  return nBytes;
}
int pack_next(uint8_t* buff, int size, int iPID, uint8_t* data, int dataSize, int iCounter, int64_t iTimeStamp)
{
  uint8_t* ESData = (uint8_t*)data;
  int ESSize = dataSize;

  int len = 0;        //ts data len
  int nBytes = 0;     //packetized ES data parital [i]

  TsPacket().createTimeStamp(buff + len, size - len,
    0,            //use input iTimeStamp
    iTimeStamp);
  len += TIME_STAMP_SIZE;
  nBytes += TsPacket().createChunk_NORMAL(
    buff + len,
    size - len,
    iPID,
    iCounter,
    ESData + nBytes,
    ESSize - nBytes
    );
  len += PACKET_SIZE;

  return nBytes;
}
void test_pack(char* zFin, char* zFout, int64_t iPts, int64_t iDts)
{
  FILE *fin = 0;
  FILE *fout = 0;
  fopen_s(&fin, zFin, "rb");
  fopen_s(&fout, zFout, "wb+");
  assert(fin && fout);

#if(1)
  int size = 1024*1024;   //1MB
  size = size - (size % (TIME_STAMP_SIZE + PACKET_SIZE));
  uint8_t* buff = new uint8_t[size];
  assert(buff);

  int dataSize = fileSize(fin);
  uint8_t* data = new uint8_t[dataSize];
  assert(data);
  int rc = fread(data, dataSize, 1, fin);
  assert(rc);

  int len = pack(buff,
    size,
    data,         //es data
    dataSize,
    0,            //counter
    iPts, iDts);

  rc = fwrite(buff, len, 1, fout);
  assert(rc);
#endif
  delete buff;
  delete data;
  fclose(fin);
  fclose(fout);
}


#define HZERROR_SUCCESS 0
int m_VideoPID = VIDEO_PID;
int m_AudioPID = AUDIO_PID;
int m_findVideoStart = 0;
int m_findAudioStart = 0;
int m_hasLPCMAudio = 0;
#define TS_PACKET_BYTE_SIZE 192
int64_t m_pts = 78000;
int64_t m_dts = 67000;

int64_t getPtsByPesHdr(const uint8_t *pPesHdr)
{
	return ((int64_t)(pPesHdr[9] & 0xe) << 29) | ((int64_t)(pPesHdr[10]) << 22) | ((int64_t)(pPesHdr[11] & 0xfe) << 14) | ((int64_t)(pPesHdr[12]) << 7) | ((int64_t)(pPesHdr[13] & 0xfe) >> 1);
}
int64_t getDtsByPesHdr(const uint8_t *pPesHdr)
{
	return ((int64_t)(pPesHdr[14] & 0xe) << 29) | ((int64_t)(pPesHdr[15]) << 22) | ((int64_t)(pPesHdr[16] & 0xfe) << 14) | ((int64_t)(pPesHdr[17]) << 7) | ((int64_t)(pPesHdr[18] & 0xfe) >> 1);
}

//if success func return ES data partial size
//if error func return -1;
//NODE:
//+ this func: use global vars
//  m_VideoPID
//  m_AudioPID
//  m_findVideoStart
//  m_findAudioStart
//  m_hasLPCMAudio
int getESDataPartial(uint8_t* packetData, uint8_t* buff, int size)
{
  int len = 0;
#if(1)
  uint8_t *pData = packetData;

  uint32_t pid = ((pData[5] << 8) | pData[6]) & 0x1fff;
  BOOL unit_start = (pData[5] & 0x40) > 0;
  uint32_t adaptation_field = (pData[7] & 0x30) >> 4;
  uint32_t adaptation_field_length = pData[8];
  uint32_t payloadSize = 184;
  //int32_t ret;

  // Video/AudioˆÈŠO‚ÌPacket‚Í“Ç‚Ý”ò‚Î‚·
  if((pid != m_VideoPID) && (pid != m_AudioPID))
  {
    assert(0);
    return 0;
  }

  if(pid == m_VideoPID){
    if(unit_start){
      if(!m_findVideoStart){
        m_findVideoStart = TRUE;
      } else {
        //m_VideoEs.ChangeCurrentEs();
        assert(0);
        return -1;
      }

      if(unit_start){
        // PES Header Syntax
        // -------------------------------------------------------------------------------------
        // | PESStartCode  | length   | flags    | HeaderData    | PESHeaderData | PacketData  |
        // | (4bytes)      | (2bytes) | (2bytes) | Length(1byte) |               |             |
        // -------------------------------------------------------------------------------------
        uint8_t* pPesHdr = pData + 8;
        if (0x02 == adaptation_field || 0x03 == adaptation_field) {
          pPesHdr += (1 + adaptation_field_length);
        }
        payloadSize -= (9 + pPesHdr[8]);	// PESHeaderLength + HeaderDataLength
      }
    }

    if(adaptation_field == 3){
      payloadSize -= (adaptation_field_length + 1);
    } else if(adaptation_field == 2){
      payloadSize = 0;
    }

    if(m_findVideoStart){
      memcpy_s(buff + len, size - len, &pData[TS_PACKET_BYTE_SIZE - payloadSize], payloadSize);
      len += payloadSize;
    }
  } else if(pid == m_AudioPID){
    if(unit_start){
      if(!m_findAudioStart){
        m_findAudioStart = TRUE;
      } else {
        //m_AudioEs.ChangeCurrentEs();
        assert(0);
        return -1;
      }
      uint8_t* pPesHdr = pData + 8;
      if (0x02 == adaptation_field || 0x03 == adaptation_field) {
        pPesHdr += (1 + adaptation_field_length);
      }
      payloadSize -= (9 + pPesHdr[8]);	// PESHeaderLength + HeaderDataLength

      if(m_hasLPCMAudio)
      {
        payloadSize -= 4;	// 4 = AVCHDLPCMAudioDataHeader Size
      }
    }

    if(adaptation_field == 3){
      payloadSize -= (adaptation_field_length + 1);
    } else if(adaptation_field == 2){
      payloadSize = 0;
    }

    if(m_findAudioStart){
      memcpy_s(buff + len, size - len, &pData[TS_PACKET_BYTE_SIZE - payloadSize], payloadSize);
      len += payloadSize;
    }
  }
#endif
  return len;
}

//unpack one ES
void test_unpack(char* zFin, char* zFout)
{
  FILE *fin = 0;
  FILE *fout = 0;
  fopen_s(&fin, zFin, "rb");
  fopen_s(&fout, zFout, "wb+");

  uint8_t packetData[192];
  int size = 1024*1024;   //1MB
  uint8_t* buff = new uint8_t[size];
  int len = 0;
  assert(buff);
  //add packet
  while(fread(packetData, 192, 1, fin))
  {
    len += getESDataPartial(packetData, buff + len, size - len);
  }
  int rc = fwrite(buff, len, 1, fout);
  assert(rc);
  delete buff;
  fclose(fin);
  fclose(fout);
}

int test_file(int argc, char** argv)
{
  char* zFin = "C:\\tmp\\org.txt";
  char* zPackFile = "C:\\tmp\\pack";
  char* zUnpackFile = "C:\\tmp\\unpack";

  test_pack(zFin, zPackFile, m_pts, m_dts);
  test_unpack(zPackFile, zUnpackFile);
  return 0;
}

int getPacketInfo(uint8_t* packetData, int* pPid, int* pCounter, int* pIsStart, int64_t* pPts, int64_t* pDts)
{
  uint8_t *pData = packetData;

  uint32_t uiTimeStamp = 0;
  memcpy_s(&uiTimeStamp, 4, pData, 4);

  uint32_t pid = ((pData[5] << 8) | pData[6]) & 0x1fff;
  BOOL unit_start = (pData[5] & 0x40) > 0;
  uint32_t adaptation_field = (pData[7] & 0x30) >> 4;
  uint32_t continuity_counter = pData[7] & 0x0F;
  uint32_t adaptation_field_length = pData[8];
  uint32_t payloadSize = 184;

  int64_t pts = 0;
  int64_t dts = 0;


  if((pid == m_VideoPID) || (pid == m_AudioPID))
  {
    if(unit_start){
      // PES Header Syntax
      // -------------------------------------------------------------------------------------
      // | PESStartCode  | length   | flags    | HeaderData    | PESHeaderData | PacketData  |
      // | (4bytes)      | (2bytes) | (2bytes) | Length(1byte) |               |             |
      // -------------------------------------------------------------------------------------
      uint8_t* pPesHdr = pData + 8;
      if (0x02 == adaptation_field || 0x03 == adaptation_field) {
        pPesHdr += (1 + adaptation_field_length);
      }
      payloadSize -= (9 + pPesHdr[8]);	// PESHeaderLength + HeaderDataLength

      if(((pPesHdr[7] & 0x80) == 0x80)){
        pts = getPtsByPesHdr(pPesHdr);
      }

      if(((pPesHdr[7] & 0x40) == 0x40)){
        dts = getDtsByPesHdr(pPesHdr);
      }
    }
  }

  *pPid = pid;
  *pCounter = continuity_counter;
  *pIsStart = unit_start;
  *pPts = pts;
  *pDts = dts;

  return pid;
}
int isStartPacket(uint8_t* packet)
{
  return 0;
}
//adjust packet 192 continuity counter
int adjustCounter(uint8_t* packet, int size, int iCounter)
{
  assert(size == 192);
  AdvancedBitStream bs;
  bs.Reset(packet + 4, 188);

  bs.SkipBits(8 + 1 + 1 + 1 + 13 + 2 + 2);
  bs.AddBits(iCounter, 4);
  return 0;
}
int adjustTimeStamp(uint8_t* packet, int size, int64_t iTimeStamp)
{
  assert(size == 192);
  AdvancedBitStream bs;
  bs.Reset(packet, 4);

  uint32_t uiTimeStamp = (uint32_t)(iTimeStamp * 300);
  bs.AddBits(uiTimeStamp, 4 * 8);
  return 0;
}
//get and update counter
typedef struct {
  enum {size = 10};
  struct {
    int iPID;
    int iCounter;
  } data[size];
  int len;
} CounterMng;
void CM_init(CounterMng* pCM)
{
  pCM->len = 0;
}
//if success return 0,
// else if error return error code != 0
int CM_Add(CounterMng* pCM, int iPID)
{
  //find existing one
  int i = 0;
  for (; i < pCM->len; i++) {
    if (pCM->data[i].iPID == iPID) {
      break;
    }
  }
  do {
    //if found return error
    if (i < pCM->len) break;
    //if no more space return error
    if (i >= pCM->size) break;
    //adding one
    pCM->len++;
    pCM->data[i].iPID = iPID;
    pCM->data[i].iCounter = 0;
    return 0;
  } while (false);
  return -1;
}
//if success func return counter value 0-15
//else if error func return -1
int CM_GetAndInc(CounterMng* pCM, int iPID)
{
  //find existing one
  int i = 0;
  for (; i < pCM->len; i++) {
    if (pCM->data[i].iPID == iPID) {
      break;
    }
  }

  //if found
  if (i < pCM->len) {
    int iCounter = pCM->data[i].iCounter;          //get counter
    pCM->data[i].iCounter = INC_COUNTER(iCounter); //inc counter for next step
    return iCounter;
  }

  assert(0);
  return -1;
}
//pack multiple ES
#define SKIP_AUDIO 0  //play error
#define SKIP_VIDEO 0
void test_pack_ts(char* zFin, char* zFout)
{
  FILE *fin = 0;
  FILE *fout = 0;
  fopen_s(&fin, zFin, "rb");
  fopen_s(&fout, zFout, "wb+");

  enum {iAudioES = 0, iVideoES = 1};
  struct {
    uint8_t* data;
    int len;
    int size;
    //int counter;
    int64_t pts;
    int64_t dts;
    int pid;
  } ES[2], Packets;
  int i;
  for (i = 0; i < 2; i++) {
    ES[i].size = 1024*1024;
    ES[i].len = 0;
    ES[i].data = new uint8_t[ES[i].size];
  }
  ES[iAudioES].pid = AUDIO_PID;
  ES[iVideoES].pid = VIDEO_PID;
  m_findAudioStart = FALSE;
  m_findVideoStart = FALSE;

  Packets.size = 1024*1024;
  Packets.data = new uint8_t[Packets.size];

  //counter
  CounterMng cm;
  enum {nPID = 6};
  int arrPID[6] = {VIDEO_PID, AUDIO_PID, PCR_PID, PAT_PID, PMT_PID, SIT_PID};
  CM_init(&cm);
  for (int i = 0; i< nPID; i++) {CM_Add(&cm, arrPID[i]);}

  uint8_t onePacket[192];
  int rc;

  int pid;
  int counter;
  int isStart;
  int64_t pts;
  int64_t dts;

  //get time stamp from first packet - getPacketInfo
  //int64_t iTimeStamp = 4089112083;
  int64_t iTimeStamp = 0;         //use zero timeStamp

  //(1) get first packet 192
  rc = fread(onePacket, 192, 1, fin);
  assert(rc);
  getPacketInfo(onePacket, &pid, &counter, &isStart, &pts, &dts);

  int nPackets = 1;
  for(;;)
  {
    //(2) collect ES partial then stored in buffer
    int iWriteLastES = 0;
    int flags = 0;
    int iES = -1;
    enum {collect = 0x01, ignore = 0x02, skip = 0x04, pack = 0x10, metEOF = 0x20,};
    if (pid == VIDEO_PID){
#if(SKIP_VIDEO)
      flags = skip;
#else
      flags = collect;
      iES = iVideoES;
      if (isStart) {
        ES[iES].len = 0;
        ES[iES].pts = pts;
        ES[iES].dts = dts;
        assert(ES[iES].pid == VIDEO_PID);
        m_findVideoStart = FALSE;
      }
#endif
    }
    else if (pid == AUDIO_PID) {
#if(SKIP_AUDIO)
      flags = skip;
#else
      flags = collect;
      iES = iAudioES;
      if (isStart) {
        ES[iES].len = 0;
        ES[iES].pts = pts;
        ES[iES].dts = dts;
        assert(ES[iES].pid == AUDIO_PID);
        m_findAudioStart = FALSE;
      }
#endif
    }
#if (PES_PCR)
    else if (pid == PCR_PID)
      flags = ignore;
#endif
    else
      flags = skip;

    if (flags & collect) {
      assert(pid == VIDEO_PID || pid == AUDIO_PID);
      rc = getESDataPartial(onePacket, ES[iES].data + ES[iES].len, ES[iES].size - ES[iES].len);
      //assert(rc > 0);
      ES[iES].len += rc;

      //pack
      if (isStart) {
        //pack_first
      } else {
        //pack_next
      }
    }
    else if (flags & skip)
    {
      //adjust other packet timeStamp
      adjustTimeStamp(onePacket, 192,
        iTimeStamp          //use zero timeStamp
        );
      //adjust other packet continuity counter
      adjustCounter(onePacket, 192,
        CM_GetAndInc(&cm, pid)  //get and inc counter
        );
      rc = fwrite(onePacket, 192, 1, fout);
      assert(rc);
    }

    //(3) get next packet 192
    rc = fread(onePacket, 192, 1, fin);
    if (!rc)
    {
      flags = metEOF | pack;
      iES = iVideoES;
      iWriteLastES = 2;
    }
    else
    {
      getPacketInfo(onePacket, &pid, &counter, &isStart, &pts, &dts);
      nPackets++;

      //(4) met next ES first partial
      // packetize ES that stored in buffer
      if(isStart)
      {
        if (pid == VIDEO_PID) {
          iES = iVideoES;
          flags = pack;
        }
        else if (pid == AUDIO_PID) {
          iES = iAudioES;
          flags = pack;
        }
      }
    }

    do {  //case met eof

      if ((flags & pack) && (ES[iES].len > 0)) {
        //pack ES
        int len = 0;
        int nBytes = 0;
        int size = Packets.size - (Packets.size % 192);
        nBytes += pack_first(Packets.data, Packets.size,
          ES[iES].pid,
          ES[iES].data, ES[iES].len,
          CM_GetAndInc(&cm, ES[iES].pid),  //get and inc counter
          ES[iES].pts, ES[iES].dts,
          iTimeStamp);
        len += 192;

        for(;ES[iES].len > nBytes && size > len;) {
          nBytes += pack_next(Packets.data + len, Packets.size - len,
            ES[iES].pid,
            ES[iES].data + nBytes, ES[iES].len - nBytes,
            CM_GetAndInc(&cm, ES[iES].pid),  //get and inc counter
            iTimeStamp);
          len += 192;
        }
        rc = fwrite(Packets.data, len, 1, fout);
        assert(rc);
      }

      //case met eof
      iWriteLastES--;
      iES = (iES == iVideoES)?iAudioES:iVideoES;    //switch to other ES
    } while (iWriteLastES > 0);

    //exit if end of file
    if (flags & metEOF) break;

    //goto (2)
  }

  for (i = 0; i< 2; i++) {
    delete ES[i].data;
  }
  delete Packets.data;
  fclose(fin);
  fclose(fout);
}
void test_create_PAT(char* zFout)
{
  const int iNOfPrograms = 2;
  TsProgramNumbers ProgNumbers[iNOfPrograms];
  ProgNumbers[0].pProgramNum = 1;
  ProgNumbers[0].pProgramPid = 256;
  ProgNumbers[1].pProgramNum = 0;
  ProgNumbers[1].pProgramPid = 31;

  PATTableWriterParams PATparams;
  PATparams.iNOfPrograms = iNOfPrograms;
  PATparams.pProgramNumbers = ProgNumbers;
  PATparams.transportStreamId = MPEG2MUX_TS_ID;   //=0

  int size = 192;
  uint8_t* buff = new uint8_t[192];
  int iCounter = 0;

  int len;
  len = TsPacket().createTimeStamp(buff, size, 1, (int64_t)0);
  TsPacket().createPAT(buff + len, size - len, &PATparams, &iCounter);

  FILE* f;
  fopen_s(&f, zFout, "wb+");
  fwrite(buff, 192, 1, f);
  fclose(f);
  delete buff;
}
int main(int argc, char** argv)
{
  assert(sizeof(Ipp64f) == 8);
  assert(sizeof(Ipp64u) == 8);
  assert(sizeof(Ipp64s) == 8);
  assert(sizeof(Ipp32u) == 4);
  assert(sizeof(Ipp32s) == 4);
  assert(sizeof(Ipp16u) == 2);
  assert(sizeof(Ipp16s) == 2);
  assert(sizeof(Ipp8u)  == 1);
  assert(sizeof(Ipp8u)  == 1);

  assert(sizeof(uint8_t)  == 1);
  assert(sizeof(int32_t)  == 4);
  assert(sizeof(uint32_t) == 4);
  assert(sizeof(int64_t)  == 8);
  assert(sizeof(uint64_t) == 8);

  //char* zFin = "C:\\tmp\\adjust\\10s.TS";
  char* zFin = "E:\\work\\XAVC\\muxer_ut\\10s.TS";
  char* zFout = "C:\\tmp\\pack";

  if (argc == 3) {
    if (GetFileAttributesA(argv[1]) != INVALID_FILE_ATTRIBUTES) {
      test_pack_ts(argv[1], argv[2]);
    }
  }
  else {
    test_pack_ts(zFin, zFout);
    //test_create_PAT(zFout);
  }
  return 0;
  test_file(argc, argv);
}
