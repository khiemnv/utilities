//
#include <tchar.h>
#include <windows.h>

#ifndef _TS_HDR__
#define _TS_HDR__

typedef unsigned char         uint8_t;
typedef long                  int32_t;
typedef unsigned long         uint32_t;
typedef long long             int64_t;
typedef unsigned long long    uint64_t;

typedef struct {
	int width;
	int height;
} VideoInfo;

#if (1)
#define PACKET_SIZE					192
#define PACKET_DATA_OFFSET	4
#else
#define PACKET_SIZE					188
#define PACKET_DATA_OFFSET	0
#endif

//#define ADJUST_PTS
//#define PARSE_PES
#define PARSE_PAT

#define VIDEO_PID	0x1011
#define AUDIO_PID	0x1100
#define PAT_PID     0x0000
#define PMT_PID     0x0100

#define program_association_section  0x00
#define CA_section                   0x01
#define TS_program_map_section       0x02
#define TS_description_section       0x03

#define DU_TRACE2(level, format, ...) {_tprintf(L"packet %08d PID %04x", g_count, g_PID);_tprintf(format, __VA_ARGS__); _tprintf(L"\n");}
//#define DU_TRACE2(level, format, ...) {_ftprintf(level, format, __VA_ARGS__); _ftprintf(level, L"\n");}
#define DEBUG fout

//extern FILE *fout;
extern int g_count;
extern int g_PID;
extern int g_iGOP;

extern int g_program_map_PID;

//class TsHdr
//{
//public:
//	TsHdr(){};
//	~TsHdr(){};
	__int64 getPts(uint8_t* ipPacketData);
	__int64 getPtsByPesHdr(const uint8_t *pPesHdr);
	__int64 updateTsValue(uint8_t *pTsValue, __int64 adjust);
	__int64 adjustPacketTsValue(uint8_t *ipPacketData, __int64 adjust);
	__int64 updatePesPacket(uint8_t *ipPesPacket, __int64 adjust);
	__int64 updateAdaptationField(uint8_t *pAdaptationField, __int64 adjust = 0);
	__int64 updatePCRvalue(uint8_t *pPCRvalue, __int64 adjust = 0);
	int     updateCRCvalue(uint8_t *pCRCvalue);
	__int64 adjustPacketPCRValue(uint8_t *ipPacketData, __int64 adjust);
	void    adjustTransportPacket(uint8_t *ipPacketData, int pid, __int64 adjust = 0);
	int     getPID(uint8_t *ipPacketData);
	void    tracePEStHdr(uint8_t *ipPacketData);
	bool    PacketIsStartOfGOP(uint8_t *ipPacketData);
	int     find_IDR_frame(uint8_t *pData, int size);

	int     getVideoInfo(uint8_t *ipPacketData, int pid, VideoInfo* pInfo);
	int     parsePAT(uint8_t *pPayLoad, VideoInfo* pInfo);
	int     parseTSDS(uint8_t *pSection);
	int     parsePAS(uint8_t *pSection);
	int     parsePM(uint8_t *pData);
	uint8_t*getESDataPartial(uint8_t* pPacket);

	int     parsePES(uint8_t *pData);
	int     parseAF(uint8_t *pData);
	int     splitTransportPacket(uint8_t* pPacket, uint8_t** pPES, uint8_t** pAF);

  typedef struct block__ {
    uint8_t* data;
    int size;
  } block;
  int     unpackTransportPacket(uint8_t* pPacket, block* pPES, block* pAF, block* pPL);
///+ calculate PTS adjustment
//  pts adjustment = 2^33 - cur pts + base pts
//+ adjust PTS value
//  new PTS = (old PTS + adjust PTS) % 2^33
//          = (old PTS + adjust PTS) & (2^34 - 1)
#define PRERECF_CalcPtsAdjmt(curPts, basePts) ((0x01LL<<33) - curPts + basePts)
#define PRERECF_AdjustPtsVal(old, adjust) ((old + adjust) & ((0x01LL<<34) - 1))
//}
#endif //_TS_HDR__
