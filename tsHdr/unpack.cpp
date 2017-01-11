#include <tchar.h>
#include <windows.h>

#include <crtdbg.h>
#define assert(x) _ASSERT(x)
//#include <assert.h>

#include "TsPacket.h"

int g_count;
int g_PID;
int g_iGOP;
int g_program_map_PID;

typedef struct {
	uint8_t flag;
	uint8_t size;
} hdrField;

//[overPTS]
//get pts of GOP 1st packet
//assert this packet is VIDEO UINT
__int64 /*TsHdr::*/getPts(uint8_t* ipPacketData)
{
  uint32_t adaptation_field = (ipPacketData[7] & 0x30) >> 4;
  uint32_t adaptation_field_length = ipPacketData[8];
  __int64 pts = 0;
  // PES Header Syntax
  // -------------------------------------------------------------------------------------
  // | PESStartCode  | length   | flags    | HeaderData    | PESHeaderData | PacketData  |
  // | (4bytes)      | (2bytes) | (2bytes) | Length(1byte) |               |             |
  // -------------------------------------------------------------------------------------
  uint8_t* pPesHdr = ipPacketData + 8;
  if (0x02 == adaptation_field || 0x03 == adaptation_field)
  {
    pPesHdr += (1 + adaptation_field_length);
  }

  if(((pPesHdr[7] & 0x80) == 0x80))
  {
    pts = getPtsByPesHdr(pPesHdr);
  }
  return pts;
}

__int64 /*TsHdr::*/getPtsByPesHdr(const uint8_t *pPesHdr)
{
	return (static_cast<__int64>(pPesHdr[9] & 0xe) << 29) | ((pPesHdr[10]) << 22) | ((pPesHdr[11] & 0xfe) << 14) | (pPesHdr[12] << 7) | ((pPesHdr[13] & 0xfe) >> 1);
}

__int64 /*TsHdr::*/updateTsValue(uint8_t *pTsValue, __int64 adjust)
{
#define MAKER_BIT	0x01
	__int64 tsValue;

	//TsValue       bits         index
	//'0010'        4   bslbf    [0]
	//PTS [32..30]  3   bslbf    [0]
	//marker_bit    1   bslbf    [0]
	//PTS [29..15]  15  bslbf    [1, 2]
	//marker_bit    1   bslbf    [2]
	//PTS [14..0]   15  bslbf    [3, 4]
	//marker_bit    1   bslbf    [4]

	if (adjust != 0)
	{//update tsValue
		//get old tsValue
		tsValue = (static_cast<__int64>(pTsValue[0] & 0xe) << 29) | ((pTsValue[1]) << 22) | ((pTsValue[2] & 0xfe) << 14) | (pTsValue[3] << 7) | ((pTsValue[4] & 0xfe) >> 1);

		//calc new PTS
		tsValue = PRERECF_AdjustPtsVal(tsValue, adjust);

		//update to hdr
		//hdr[]
		//9   0000xxx0  >>29 & 0x0E
		//10  xxxxxxxx  >>22 & 0xFF
		//11  xxxxxxx0  >>14 & 0xFE
		//12  xxxxxxxx  >>7  & 0xFF
		//13  xxxxxxx0  <<1  & 0xFE
		uint8_t old4bits = pTsValue[0] & 0xF0;
		pTsValue[0] =  old4bits | MAKER_BIT | (uint8_t)((tsValue>>29) & 0x0E);
		pTsValue[1] = (uint8_t)((tsValue>>22) & 0xFF);

		pTsValue[2] = MAKER_BIT | (uint8_t)((tsValue>>14) & 0xFE);
		pTsValue[3] = (uint8_t)((tsValue>>7)  & 0xFF);

		pTsValue[4] = MAKER_BIT | (uint8_t)((tsValue<<1)  & 0xFE);
	}

	if (!(pTsValue[0] & 0x20)) return 0;
	if (!(pTsValue[0] & 0x01)) return 0;
	if (!(pTsValue[2] & 0x01)) return 0;
	if (!(pTsValue[4] & 0x01)) return 0;

	//if update tsValue
	//  return new tsValue
	//else
	//  return cur tsValue
	tsValue = (static_cast<__int64>(pTsValue[0] & 0xe) << 29) | ((pTsValue[1]) << 22) | ((pTsValue[2] & 0xfe) << 14) | (pTsValue[3] << 7) | ((pTsValue[4] & 0xfe) >> 1);
	return tsValue;
}

//[overPTS]
//+ update/qry tsValue of unit has PID
//  VIDEO_PID  = 0x1011
//  AUDIO_PID  = 0x1100
//+ new tsValue = old tsValue - adjust
//+ if adjust == 0 return the uint tsValue
//  else update then return new tsValue
//+ ref: ITU-T Rec. H.222.0 (05/2006) - Table 2-21 - PES packet - page 31
//  if (stream_id != program_stream_map
//  && stream_id != padding_stream
//  && stream_id != private_stream_2
//  && stream_id != ECM
//  && stream_id != EMM
//  && stream_id != program_stream_directory
//  && stream_id != DSMCC_stream
//  && stream_id != ITU-T Rec. H.222.1 type E stream) {
//    PTS/DTS available
//  }
__int64 /*TsHdr::*/adjustPacketTsValue(uint8_t *ipPacketData, __int64 adjust)
{
	uint32_t pid = ((ipPacketData[5] << 8) | ipPacketData[6]) & 0x1fff;
	BOOL unit_start = (ipPacketData[5] & 0x40) > 0;
	uint32_t adaptation_field = (ipPacketData[7] & 0x30) >> 4;
	uint32_t adaptation_field_length = ipPacketData[8];
	__int64 tsValue = 0;

	int tsFlags = 0;

	union {
		struct {
			hdrField PTS                  ;
			hdrField DTS                  ;
			hdrField ESCR                 ;
			hdrField ES_rate              ;
			hdrField DSM_trick_mode       ;
			hdrField additional_copy_info ;
			hdrField PES_CRC              ;
			hdrField PES_extension        ;
		};
		hdrField fields[8];
	} hdr = {
		0x80, 5,	//flag , field length
		0x40, 5,
		0x20, 6,
		0x10, 3,
		0x08, 1,
		0x04, 1,
		0x02, 2,
		0x01, 0,
	};

	enum {
		videoPacket		= 0x0100,
		audioPacket		= 0x0200,
	};

	if (pid == VIDEO_PID)
	{
		tsFlags |= videoPacket;
	}
	else if (pid == AUDIO_PID)
	{
		tsFlags |= audioPacket;
	}

	if ((tsFlags != 0) && unit_start)
	{
		uint8_t* pPesHdr = ipPacketData + 8;
		if (0x02 == adaptation_field || 0x03 == adaptation_field)
		{
			//check PCR in adaption field
			DU_TRACE2(DEBUG, L"[overPTS] adaptation_field ON");
			updateAdaptationField(ipPacketData + 8);
			pPesHdr += (1 + adaptation_field_length);
		}

		if (pPesHdr[7] & hdr.PTS.flag)
		{//case video pts or audio pts
			tsValue = updateTsValue(pPesHdr + 9, adjust);
			DU_TRACE2(DEBUG, L"[overPTS] tsFlags 0x%04x PtsValue %I64d", pPesHdr[7], tsValue);
		}

		if (pPesHdr[7] & hdr.DTS.flag)
		{//case video dts
			tsValue = updateTsValue(pPesHdr + 14, adjust);
			DU_TRACE2(DEBUG, L"[overPTS] tsFlags 0x%04x DtsValue %I64d", pPesHdr[7], tsValue);
		}

		if (pPesHdr[7] & hdr.PES_CRC.flag)
		{//case packet require CRC
			DU_TRACE2(DEBUG, L"[overPTS] PES_CRC_flag ON");
			int index = 9;			//start of PES hdr data
			for (int i = 0; &hdr.fields[i] != &hdr.PES_CRC; i++)
				if (hdr.fields[i].flag & pPesHdr[7])
					index += hdr.fields[i].size;

			updateCRCvalue(pPesHdr + index);
		}
	}
	return tsValue;
}

__int64 /*TsHdr::*/updateAdaptationField(uint8_t *pAdaptationField, __int64 adjust)
{
	uint8_t *pPCRvalue = pAdaptationField + 2;
	__int64 pcr;

	union {
		struct {
			hdrField discontinuity_indicator              ;
			hdrField random_access_indicator              ;
			hdrField elementary_stream_priority_indicator ;
			hdrField PCR                                  ;
			hdrField OPCR                                 ;
			hdrField splicing_point_flag                  ;
			hdrField transport_private_data_flag          ;
			hdrField adaptation_field_extension_flag      ;
		};
		hdrField fields[8];
	} hdr = {
		0x80, 0,	//discontinuity_indicator               1 [1]
		0x40, 0,	//random_access_indicator               1 [1]
		0x20, 0,	//elementary_stream_priority_indicator  1 [1]
		0x10, 6,	//PCR_flag                              1 [1]
		0x08, 6,	//OPCR_flag                             1 [1]
		0x04, 0,	//splicing_point_flag                   1 [1]
		0x02, 0,	//transport_private_data_flag           1 [1]
		0x01, 0,	//adaptation_field_extension_flag       1 [1]
	};

	DU_TRACE2(DEBUG, L"[overPTS] AdaptationField flag %02x", pAdaptationField[1]);

	if (pAdaptationField[1] & hdr.discontinuity_indicator.flag)
	{
		DU_TRACE2(DEBUG, L"[overPTS] discontinuity_indicator ON");
	}
	if (pAdaptationField[1] & hdr.PCR.flag)
	{
		pcr = updatePCRvalue(pPCRvalue, adjust);
		DU_TRACE2(DEBUG, L"[overPTS] PCR value %I64d", pcr);
		pPCRvalue += hdr.PCR.size;						//old PCR value
	}
	if (pAdaptationField[1] & hdr.OPCR.flag)
	{
		pcr = updatePCRvalue(pPCRvalue, adjust);
		DU_TRACE2(DEBUG, L"[overPTS] OPCR value %I64d", pcr);
	}
	return 0;
}

__int64 /*TsHdr::*/updatePCRvalue(uint8_t *pPCRvalue, __int64 adjust)
{
	DU_TRACE2(DEBUG, L"[overPTS] PCR value 0x%02x%02x %02x%02x %02x%02x",
		pPCRvalue[0], pPCRvalue[1], pPCRvalue[2], pPCRvalue[3], pPCRvalue[4], pPCRvalue[5]);

	//OPCR(i) = OPCR _ base(i)Å~300 +OPCR _ ext(i)
	//bits|shift |index
	//8   |<<25  |[0]
	//8   |<<17  |[1]
	//8   |<<9   |[2]
	//8   |<<1   |[3]
	//1   |>>7   |[4]
	//6
	//1   |<<8   |
	//8   |      |[5]
	__int64 pcr_base =
		(__int64)pPCRvalue[0] << 25
		| (pPCRvalue[1] << 17)
		| (pPCRvalue[2] << 9)
		| (pPCRvalue[3] << 1)
		| (pPCRvalue[4] >> 7);
	__int64 pcr_ext =
		(__int64)(pPCRvalue[4] & 0x01) << 8
		| (pPCRvalue[5])
		;

	if (adjust != 0)
	{//only adjust pcr_base
		uint8_t old7bits = pPCRvalue[4] & 0x7F;
		pcr_base = PRERECF_AdjustPtsVal(pcr_base, adjust);
		pPCRvalue[0] = (pcr_base >> 25) & 0xFF;
		pPCRvalue[1] = (pcr_base >> 17) & 0xFF;
		pPCRvalue[2] = (pcr_base >> 9)  & 0xFF;
		pPCRvalue[3] = (pcr_base >> 1)  & 0xFF;
		pPCRvalue[4] = old7bits | ((pcr_base << 7) & 0x80);

		pcr_base =
		(__int64)pPCRvalue[0] << 25
		| (pPCRvalue[1] << 17)
		| (pPCRvalue[2] << 9)
		| (pPCRvalue[3] << 1)
		| (pPCRvalue[4] >> 7);
	}

	DU_TRACE2(DEBUG, L"[overPTS] pcr_base %I64d pcr_ext %I64d", pcr_base, pcr_ext);
	return (pcr_base * 300 + pcr_ext);
	//return pcr_base;
}

int /*TsHdr::*/updateCRCvalue(uint8_t *pCRCvalue)
{
	DU_TRACE2(DEBUG, L"[overPTS] CRC value 0x%02x%02x", pCRCvalue[0], pCRCvalue[1]);
	return 0;
}

__int64 /*TsHdr::*/adjustPacketPCRValue(uint8_t *packetData, __int64 adjust)
{
  uint8_t* pData = packetData + PACKET_DATA_OFFSET;
	uint32_t pid = ((pData[1] << 8) | pData[2]) & 0x1fff;
	BOOL unit_start = (pData[2] & 0x40) > 0;
	uint32_t adaptation_field = (pData[3] & 0x30) >> 4;
	uint32_t adaptation_field_length = pData[4];

	if (unit_start)
	{
		uint8_t* pPesHdr = pData + 4;
		if (0x02 == adaptation_field || 0x03 == adaptation_field)
		{
			//check PCR in adaption field
			DU_TRACE2(DEBUG, L"[overPTS] adaptation_field ON");
			updateAdaptationField(pData + 4);
			pPesHdr += (1 + adaptation_field_length);
		}
	}
	return 0;
}

void adjustTransportPacket(uint8_t *pPacket, int pid, __int64 adjust)
{
	//field                         bits              index
	//sync_byte                     8    bslbf        [4]
	//transport_error_indicator     1    bslbf        [+1]
	//payload_unit_start_indicator  1    bslbf        [+1]
	//transport_priority            1    bslbf        [+1]
	//PID                           13   uimsbf       [+1, 2]
	//transport_scrambling_control  2    bslbf        [+3]
	//adaptation_field_control      2    bslbf        [+3]
	//continuity_counter            4    uimsbf       [+3]

	enum {
		payload_unit_start_indicator = 0x40,
		adaptation_field_control     = 0x30,
	};

	uint8_t* pData = pPacket + PACKET_DATA_OFFSET;
	uint8_t flags = pData[3] & adaptation_field_control;
	uint8_t *pPayLoad = pData + 4;

	//extract continuity_counter
	//if (flags & 0x10)
	//{//if packet has payload field (adaptation_field_control EQ '01' or '11')
	//	uint8_t continuity_counter = pData[3] & 0x0F;
	//	DU_TRACE2(DEBUG, L"[overPTS] continuity_counter %d", continuity_counter);
	//}

	if (flags & 0x20)
	{ //if adaptation_field_control EQ '10' or '11
		DU_TRACE2(DEBUG, L"[overPTS]adaptation_field_control %02x", flags);
		DU_TRACE2(DEBUG, L"[overPTS]adaptation_field_length %d", pData[4]);
		
		//if (pData[4])
		if (pid == 0x1001)
		{
			updateAdaptationField(pData + 4, adjust);
		}

		pPayLoad += 1 + pData[4];
	}

	if (pData[1] & payload_unit_start_indicator)
	{
		DU_TRACE2(DEBUG, L"[overPTS] payload_unit_start_indicator ON %02x", flags);

		if (pid == VIDEO_PID || pid == AUDIO_PID)
		{
			updatePesPacket(pPayLoad, adjust);
		}
		if (pid == VIDEO_PID) {
			uint8_t* pPesHdr = pPayLoad;
			int hdrSize = pPesHdr[8];
			uint8_t* pPesData = pPesHdr + 9 + hdrSize;
			find_IDR_frame(pPesData, pPacket + PACKET_SIZE - pPesData);
		}
		//else {
		//	uint8_t* ipPesPacket = pPayLoad;
		//	enum {DSM_flag = 0x08,};
		//	if (ipPesPacket[7] & DSM_flag) {
		//		DU_TRACE2(DEBUG, L"[DSM] on pes[7] %x", ipPesPacket[7]);
		//	} else {
		//		DU_TRACE2(DEBUG, L"[DSM] off pes[7] %x", ipPesPacket[7]);
		//	}
		//}
	}
}

int splitTransportPacket(uint8_t* pPacket, block* pPES, block* pAF)
{
	//field                         bits              index
	//sync_byte                     8    bslbf        [4]
	//transport_error_indicator     1    bslbf        [+1]
	//payload_unit_start_indicator  1    bslbf        [+1]
	//transport_priority            1    bslbf        [+1]
	//PID                           13   uimsbf       [+1, 2]
	//transport_scrambling_control  2    bslbf        [+3]
	//adaptation_field_control      2    bslbf        [+3]
	//continuity_counter            4    uimsbf       [+3]

	enum {
		payload_unit_start_indicator = 0x40,
		adaptation_field_control     = 0x30,
	};

	uint8_t* pData = pPacket + PACKET_DATA_OFFSET;
	uint8_t flags = pData[3] & adaptation_field_control;
	uint8_t *pPayLoad = pData + 4;
  uint32_t pid = ((pData[1] << 8) | pData[2]) & 0x1fff;

  pAF->size = 0;
  pPES->size = 0;

	if (flags & 0x20)
	{
		//if adaptation_field_control EQ '10' or '11
		pAF->data = pData + 4;
    pAF->size = pData[4];

		pPayLoad += 1 + pData[4];
  }

	if (pData[1] & payload_unit_start_indicator) {
		pPES->data = pPayLoad;
    pPES->size = PACKET_SIZE - (pPayLoad - pPacket);
	}

	return pid;
}
int unpackTransportPacket(uint8_t* pPacket, block* pPES, block* pAF, block* pPL)
{
	enum {
    //field                         bits              index
    //sync_byte                     8    bslbf        [4]
    //transport_error_indicator     1    bslbf        [+1]
    //payload_unit_start_indicator  1    bslbf        [+1]
    //transport_priority            1    bslbf        [+1]
    //PID                           13   uimsbf       [+1, 2]
    //transport_scrambling_control  2    bslbf        [+3]
    //adaptation_field_control      2    bslbf        [+3]
    //continuity_counter            4    uimsbf       [+3]
    TsHdrLen                    = 4,
    payload_unit_start_indicator= 0x40,
    adaptation_field_control    = 0x30,

    //---------adaptation
    //discontinuity_indicator               1 [1]
    //random_access_indicator               1 [1]
    //elementary_stream_priority_indicator  1 [1]
    //PCR_flag                              1 [1]
    //OPCR_flag                             1 [1]
    //splicing_point_flag                   1 [1]
    //transport_private_data_flag           1 [1]
    //adaptation_field_extension_flag       1 [1]
    PCR_flag  = 0x10,
    OPCR_flag = 0x08,

    //---------PES section
    //PES start code          24  [0]
    //stream_id               8
    //PES_packet_length       16  [4]
    //'10'                    2   [6]
    //PES_scrambling_c        1
    //PES_priority            1
    //data_alignment_i        1
    //copyright               1
    //original_or_copy        1
    //PTS_DTS_flags           2   [7]
    //ESCR_flag               1
    //ES_rate_flag            1
    //DSM_trick_mode_flag     1
    //additional_copy_info_f  1
    //PES_CRC_flag            1
    //PES_extension_flag      1
    //PES_header_data_length  8   [8]
    PESHeaderLen    = 9,
    PTS_DTS_flags   = 0xC0,
	};

  uint32_t uiTimeStamp = 0;
  //memcpy_s(&uiTimeStamp, 4, pPacket, 4);
  uiTimeStamp = (pPacket[0] <<24) | (pPacket[1] <<16) | (pPacket[2] <<8) | pPacket[3];

	uint8_t* pData = pPacket + PACKET_DATA_OFFSET;
  uint8_t* nextField = pData;

  // TS header
  int iStartIndicator = pData[1] & payload_unit_start_indicator;
  int iPID            = ((pData[1] << 8) | pData[2]) & 0x1fff;
  int iAdaptFieldCtrl = pData[3] & adaptation_field_control;
  int iCounter        = pData[3] & 0x0f;

  //Adaptation field
  uint8_t* pAdaptField= 0;
  int iAdaptFieldLen  = 0;
  int iPCRFlag        = 0;
  int iOPCRFlag       = 0;

  // PES section
  //PTS                  ;
  //DTS                  ;
  //ESCR                 ;
  //ES_rate              ;
  //DSM_trick_mode       ;
  //additional_copy_info ;
  //PES_CRC              ;
  //PES_extension        ;
  uint8_t* pPESHeader     = 0;
  int iPESPacketLength    = 0;
  int iPTSDTSFlags        = 0x00;
  int iPESHeaderDataLength= 0;
  int iNOfStuffingBytes   = 0;

  //payload
  int iMaxPayloadSize = 188;

  iMaxPayloadSize -= TsHdrLen;
  nextField += TsHdrLen;

  if (iAdaptFieldCtrl & 0x20)
  {
    pAdaptField = nextField;
    iAdaptFieldLen = pAdaptField[0];
    if (iAdaptFieldLen > 0)
    {
      iPCRFlag = pAdaptField[1] & PCR_flag;
      iOPCRFlag = pAdaptField[1] & OPCR_flag;
    }

    iMaxPayloadSize -= 1 + iAdaptFieldLen;
    nextField += 1 + iAdaptFieldLen;
  }

  if (iStartIndicator)
  {
    pPESHeader = nextField;
    iPESPacketLength = (pPESHeader[4] << 8) | pPESHeader[5];
    iPTSDTSFlags = pPESHeader[7] & PTS_DTS_flags;
    iPESHeaderDataLength = pPESHeader[8];

    iMaxPayloadSize -= PESHeaderLen + iPESHeaderDataLength;
    nextField += PESHeaderLen + iPESHeaderDataLength;
  }

  if (pPES)
  {
    pPES->data = pPESHeader;
    pPES->size = iPESHeaderDataLength;
  }

  if (pAF)
  {
    pAF->data = pAdaptField;
    pAF->size = iAdaptFieldLen;
  }

  if (pPL)
  {
    pPL->data = iMaxPayloadSize?nextField:NULL;
    pPL->size = iMaxPayloadSize;
  }

  DU_TRACE2(DEBUG, L"[TS] timeStamp %u Counter %d Start 0x%x AF 0x%x PLSize %d", uiTimeStamp, iCounter, iStartIndicator, iAdaptFieldCtrl, iMaxPayloadSize);

	return iPID;
}

int getPID_old(uint8_t* pPacketData)
{
	typedef struct {
		uint8_t flag;
		uint8_t size;
	} hdrField;

	union {
		struct {
			hdrField transport_error_indicator     ;
			hdrField payload_unit_start_indicator  ;
			hdrField transport_priority            ;
			hdrField PID_1                         ;
			hdrField PID_2                         ;
			hdrField transport_scrambling_control  ;
			hdrField adaptation_field_control      ;
			hdrField continuity_counter            ;
		};
		hdrField fields[8];
	} hdr = {	//flag , field length                   index
		0x80, 0,	//transport_error_indicator     1   [5]
		0x40, 0,	//payload_unit_start_indicator  1   [5]
		0x20, 0,	//transport_priority            1   [5]
		0x1F, 1,	//PID_1                         5   [5]
		0xFF, 1,	//PID_2                         8   [5, 6]
		0xC0, 0,	//transport_scrambling_control  2   [7]
		0x30, 0,	//adaptation_field_control      2   [7]
		0x0F, 1,	//continuity_counter            4   [7]
	};

	//get pid
	uint8_t* pData = pPacketData + PACKET_DATA_OFFSET;
	uint8_t pid_1 = pData[1] & hdr.PID_1.flag;
	uint8_t pid_2 = pData[2] & hdr.PID_2.flag;
	return ((int)(pid_1 <<8) | pid_2);
}

int getPID(uint8_t *pPacketData)
{
	//flag   field                      length index
	//0x80   transport_error_indicator     1   [1]
	//0x40   payload_unit_start_indicator  1   [1]
	//0x20   transport_priority            1   [1]
	//0x1F   PID_1                         5   [1]
	//0xFF   PID_2                         8   [1, 2]
	//0xC0   transport_scrambling_control  2   [3]
	//0x30   adaptation_field_control      2   [3]
	//0x0F   continuity_counter            4   [3]

	enum {
		PID_1 = 0x1F,
		PID_2 = 0xFF,
	};

	//get pid
	uint8_t* pData = pPacketData + PACKET_DATA_OFFSET;
	uint8_t pid_1 = pData[1] & PID_1;
	uint8_t pid_2 = pData[2] & PID_2;
	return ((int)(pid_1 <<8) | pid_2);
}

void tracePEStHdr(uint8_t *ipPesPacket)
{
	updatePesPacket(ipPesPacket, 0);
}
int find_IDR_frame(uint8_t *pData, int size)
{
	int rc = 0;
	int i=0,j=0;
	for(;j<10;j++) {
		for(;i<size;i++) {
			uint8_t *buf = pData+i;
			if (buf[0]==0x00 && buf[1]==0x00 && buf[2]==0x01)
			{
				// Found a NAL unit with 3-byte start code
				if((buf[3] & 0x1F) == 0x5)
				{
					// Found a reference frame, do something with it
					rc++;
				}
				rc++;
				DU_TRACE2(DEBUG, L"[NAL] rc %d byte[%d+3] %x", rc, i, buf[3]);
				i+=4;
        DU_TRACE2(DEBUG, L"[NAL] next 4 bytes %x %x %x %x", pData[i], pData[i+1], pData[i+2], pData[i+3]);
				break;
			}
			else if (buf[0]==0x00 && buf[1]==0x00 && buf[2]==0x00 && buf[3]==0x01)
			{
				// Found a NAL unit with 4-byte start code
				if((buf[4] & 0x1F) == 0x5)
				{
					// Found a reference frame, do something with it
					rc++;
				}
				rc++;
				DU_TRACE2(DEBUG, L"[NAL] rc %d byte[%d+4] %x", rc, i, buf[4]);
				i+=5;
        DU_TRACE2(DEBUG, L"[NAL] next 4 bytes %x %x %x %x", pData[i], pData[i+1], pData[i+2], pData[i+3]);
				break;
			}
		}
	}
	return rc;
}
bool PacketIsStartOfGOP(uint8_t *pPacketData)
{
//#define PRERECV_PACKET_SIZE 192
#define PRERECV_PACKET_SIZE PACKET_SIZE
	const uint8_t AVC_SPS_NAL_UNIT_TYPE = 7;
	const uint8_t AVC_PPS_NAL_UNIT_TYPE = 8;
	const uint8_t AVC_NON_IDR_SLICE_NAL_UNIT_TYPE	= 1;
	const uint8_t AVC_IDR_SLICE_NAL_UNIT_TYPE		= 5;
	const uint8_t IS_SPSNAL		= 0x01;
	const uint8_t IS_PPSNAL		= 0x02;
	const uint8_t IS_SLICENAL	= 0x04;
	//const int VIDEO_PID			= 0x1011;

	//flag   field                      length index
	//0x80   transport_error_indicator     1   [1]
	//0x40   payload_unit_start_indicator  1   [1]
	//0x20   transport_priority            1   [1]
	//0x1F   PID_1                         5   [1]
	//0xFF   PID_2                         8   [1, 2]
	//0xC0   transport_scrambling_control  2   [3]
	//0x30   adaptation_field_control      2   [3]
	//0x0F   continuity_counter            4   [3]

	bool rc = FALSE;
	uint8_t* pData = pPacketData + PACKET_DATA_OFFSET;
	uint32_t pid = ((pData[1] << 8) | pData[2]) & 0x1fff;
	BOOL unit_start = pData[1] & 0x40;
	uint32_t adaptation_field = (pData[3] & 0x30) >> 4;
	uint32_t adaptation_field_length = pData[4];
	uint8_t* pPayload = pData + 4 + 1 + adaptation_field_length;

  block PES, AF;
  pid = splitTransportPacket(pPacketData, &PES, &AF);

	if(pid == VIDEO_PID)
	{
		if(unit_start)
		{
			uint8_t* pPesHdr = PES.data;
			int payloadsize = PES.size;

			//rc = checkNALUnit(&ipPacketData[payload], payloadSize);
			{
				uint8_t flags = 0;
				bool NALStartCode = FALSE;
				int zeroBitCnt = 0;

				while(payloadsize > 0)
				{
					if(NALStartCode)
					{
						NALStartCode = FALSE;
						uint8_t nalunit = *pPayload & 0x1f;
						if(nalunit == AVC_SPS_NAL_UNIT_TYPE)
						{
							flags |= IS_SPSNAL;
						}
						else if(nalunit == AVC_PPS_NAL_UNIT_TYPE)
						{
							flags |= IS_PPSNAL;
						}
						else if((nalunit == AVC_NON_IDR_SLICE_NAL_UNIT_TYPE) ||
							(nalunit == AVC_IDR_SLICE_NAL_UNIT_TYPE))
						{
							flags |= IS_SLICENAL;
							break;
						}
					}
					else
					{
						if(*pPayload == 0)
						{
							zeroBitCnt++;
						}
						else if(*pPayload == 1)
						{
							if(zeroBitCnt >= 2)
							{
								NALStartCode = TRUE;
							}
							zeroBitCnt = 0;
						}
						else
						{
							zeroBitCnt = 0;
						}
					}
					pPayload++;
					payloadsize--;
				}
				rc = (flags == 0x7);
			}
			//checkNALUnit END
		}
	}
	return rc;
}


__int64 updatePesPacket(uint8_t *ipPesPacket, __int64 adjust)
{
	//packet_start_code_prefix  24 bslbf   [0]
	//stream_id                 8  uimsbf
	//PES_packet_length         16 uimsbf  [4]
	//'10'                      2  bslbf   [6]
	//PES_scrambling_control    2  bslbf
	//PES_priority              1  bslbf
	//data_alignment_indicator  1  bslbf
	//copyright                 1  bslbf
	//original_or_copy          1  bslbf   [6]
	//PTS_DTS_flags             2  bslbf   [7]
	//ESCR_flag                 1  bslbf
	//ES_rate_flag              1  bslbf
	//DSM_trick_mode_flag       1  bslbf
	//additional_copy_info_flag 1  bslbf
	//PES_CRC_flag              1  bslbf
	//PES_extension_flag        1  bslbf   [7]
	//PES_header_data_length    8  uimsbf  [8]

  int PES_packet_length = (ipPesPacket[4] << 8) | ipPesPacket[5];
  DU_TRACE2(DEBUG, L"[PES] PES_packet_length %d", PES_packet_length);

	enum {
		PTS_flag = 0x80,
		DTS_flag = 0x40,
		DSM_flag = 0x08,
	};

	if (ipPesPacket[7] & DSM_flag) {
		DU_TRACE2(DEBUG, L"[DSM] on pes[7] %x", ipPesPacket[7]);
	} else {
		DU_TRACE2(DEBUG, L"[DSM] off pes[7] %x", ipPesPacket[7]);
	}

	if (ipPesPacket[0] == 0x00 && ipPesPacket[1] == 0x00 && ipPesPacket[2] == 0x01)
	{
		uint8_t *pTsValue = ipPesPacket + 9;
		__int64 tsValue = 0;

		if (ipPesPacket[7] & PTS_flag)
		{//case video pts or audio pts
			tsValue = updateTsValue(pTsValue, adjust);
			DU_TRACE2(DEBUG, L"[overPTS] tsFlags 0x%04x PtsValue %I64d", ipPesPacket[7], tsValue);
			pTsValue += 5;
		}

		if (ipPesPacket[7] & DTS_flag)
		{//case video dts
			tsValue = updateTsValue(pTsValue, adjust);
			DU_TRACE2(DEBUG, L"[overPTS] tsFlags 0x%04x DtsValue %I64d", ipPesPacket[7], tsValue);
		}

		return tsValue;
	}
	else
		return 0;
}

int getVideoInfo(uint8_t *pPacket, int pid, VideoInfo* pInfo)
{
	int rc = 0;
	//field                         bits              index
	//sync_byte                     8    bslbf        [4]
	//transport_error_indicator     1    bslbf        [+1]
	//payload_unit_start_indicator  1    bslbf        [+1]
	//transport_priority            1    bslbf        [+1]
	//PID                           13   uimsbf       [5, 6]
	//transport_scrambling_control  2    bslbf        [7]
	//adaptation_field_control      2    bslbf        [7]
	//continuity_counter            4    uimsbf       [7]

	enum {
		payload_unit_start_indicator = 0x40,
		adaptation_field_control     = 0x30,
	};

	uint8_t* pData = pPacket + PACKET_DATA_OFFSET;

	uint8_t flags = pData[3] & adaptation_field_control;
	uint8_t *pPayLoad = pData + 4;

	if (flags & 0x20)
	{ //if adaptation_field_control EQ '10' or '11
		DU_TRACE2(DEBUG, L"[overPTS]adaptation_field_control %02x", flags);
		DU_TRACE2(DEBUG, L"[overPTS]adaptation_field_length %d", pData[4]);

		pPayLoad += 1 + pData[4];
	}

	if (pData[1] & payload_unit_start_indicator)
	{
		//assert(flags & 0x10);
    if (flags & 0x10) {
		DU_TRACE2(DEBUG, L"[overPTS] payload_unit_start_indicator ON %02x", flags);
		if (pid == PAT_PID) {
			//PSI
			//pointer_field   8   uimsbf   [0]
			uint8_t pointer_field = pPayLoad[0];
			//is Program Association Table
			rc = parsePAT(pPayLoad + 1 + pointer_field, pInfo);
		} else if (pid == g_program_map_PID) {
			parsePM(pPayLoad);
		}
    }
	}
	return rc;
}
uint8_t* getESDataPartial(uint8_t* pPacket)
{
	int rc = 0;
	//field                         bits              index
	//sync_byte                     8    bslbf        [4]
	//transport_error_indicator     1    bslbf        [+1]
	//payload_unit_start_indicator  1    bslbf        [+1]
	//transport_priority            1    bslbf        [+1]
	//PID                           13   uimsbf       [+1, 2]
	//transport_scrambling_control  2    bslbf        [+3]
	//adaptation_field_control      2    bslbf        [+3]
	//continuity_counter            4    uimsbf       [+3]

	enum {
		payload_unit_start_indicator = 0x40,
		adaptation_field_control     = 0x30,
	};

	uint8_t* pData = pPacket+PACKET_DATA_OFFSET;
	uint8_t flags = pData[3] & adaptation_field_control;
	uint8_t *pPayLoad = pData + 4;

	if (flags & 0x20)
	{ //if adaptation_field_control EQ '10' or '11
		DU_TRACE2(DEBUG, L"[overPTS]adaptation_field_control %02x", flags);
		DU_TRACE2(DEBUG, L"[overPTS]adaptation_field_length %d", pData[4]);

		pPayLoad += 1 + pData[4];
	}

	if (pData[1] & payload_unit_start_indicator)
	{
		assert(flags & 0x10);
		DU_TRACE2(DEBUG, L"[overPTS] payload_unit_start_indicator ON %02x", flags);
		return pPayLoad;
	}

	return 0;
}
int parsePAT(uint8_t *pSection, VideoInfo* pInfo)
{
	int rc = 0;
	//Program association section
	//table_id                  8   uimsbf [0]
	//section_syntax_indicator  1   bslbf  [1]
	//'0'                       1   bslbf  [1]
	//reserved                  2   bslbf  [1]
	//section_length            12  uimsbf [1,2]
	//transport_stream_id       16  uimsbf [3,4]
	//reserved                  2   bslbf  [5]
	//version_number            5   uimsbf [5]
	//current_next_indicator    1   bslbf  [5]
	//section_number            8   uimsbf [6]
	//last_section_number       8   uimsbf [7]

	uint8_t table_id = pSection[0];
	uint8_t *section = pSection;
	uint8_t section_number = section[6];
	uint8_t last_section_number = section[7];
	int section_length = ((section[1] & 0x0F) << 8) | section[2];
	uint8_t* next_section = section + 2 + section_length;

	switch(table_id) {
		case program_association_section:
			rc = parsePAS(pSection);
			break;
		case TS_description_section:
			parseTSDS(pSection);
			break;
		default:
			table_id = 0;
			break;
	}

	return rc;
}
int parsePAS(uint8_t *pSection)
{
	uint8_t *section = pSection;
	uint8_t section_number = section[6];
	uint8_t last_section_number = section[7];
	int section_length = ((section[1] & 0x0F) << 8) | section[2];
	uint8_t* next_section = section + 2 + section_length;

	uint8_t *pData = pSection + 8;
	//data
	//program_number  16 uimsbf   [0,1]
	//reserved        3 bslbf     [2]
	//if (program_number = = '0') {
	//network_PID     13 uimsbf   [2,3]
	//}
	//else {
	//program_map_PID 13 uimsbf   [2,3]
	//}

  int iNOfPrograms = (section_length - 9) / 4;

  struct TsProgramNumbers
  {
    int pProgramPid;
    int pProgramNum;
  };
  TsProgramNumbers *arrProgs = new TsProgramNumbers[iNOfPrograms];

  for (int i = 0; i<iNOfPrograms; i++) {
    //first program number
    int program_number = (pData[0] <<8) | pData[1];
    int PID = ((pData[2] & 0x1F) <<8) | pData[3];
    arrProgs[i].pProgramNum = program_number;
    arrProgs[i].pProgramPid = PID;
    if (program_number == 0) {
      DU_TRACE2(DEBUG, L"[vinfo] network_PID %d", PID);
    } else {
      DU_TRACE2(DEBUG, L"[vinfo] program_map_PID %d", PID);
      g_program_map_PID = PID;
    }

    //next program number
    pData += 4;
  }

  delete arrProgs;
	return g_program_map_PID;
}
int parseTSDS(uint8_t *pSection)
{
	//TS_description_section
	//table_id                  8   uimsbf
	//section_syntax_indicator  1   bslbf
	//'0'                       1   bslbf
	//Reserved                  2   bslbf
	//section_length            12  uimsbf
	//Reserved                  18  bslbf
	//version_number            5   uimsbf
	//current_next_indicator    1   bslbf
	//section_number            8   uimsbf
	//last_section_number       8   uimsbf
	uint8_t table_id = pSection[0];
	uint8_t *section = pSection;
	uint8_t section_number = section[6];
	uint8_t last_section_number = section[7];
	int section_length = ((section[1] & 0x0F) << 8) | section[2];
	uint8_t* next_section = section + 2 + section_length;

	return 0;
}

int parsePM(uint8_t *pData)
{
	//pointer_field   8   uimsbf   [0]

	uint8_t* pSec = pData + 1 + pData[0];

	//TS_program_map_section()  p46
	//table_id                  8  uimsbf  [0]
	//section_syntax_indicator  1  bslbf   [1]
	//'0'                       1  bslbf   [1]
	//reserved                  2  bslbf   [1]
	//section_length            12 uimsbf  [1,2]
	//program_number            16 uimsbf  [3,4]
	//reserved                  2  bslbf   [5]
	//version_number            5  uimsbf  [5]
	//current_next_indicator    1  bslbf   [5]
	//section_number            8  uimsbf  [6]
	//last_section_number       8  uimsbf  [7]
	//reserved                  3  bslbf   [8]
	//PCR_PID                   13 uimsbf  [8,9]
	//reserved                  4  bslbf   [10]
	//program_info_length       12 uimsbf  [10,11]

	int rc = 0;

	uint8_t table_id = pSec[0];
  uint8_t section_syntax_indicator = pSec[1] & 0x80;
  uint8_t reserved = pSec[1] & 0x30;
	int section_length = ((pSec[1] & 0xF) << 8) | pSec[2];
	int program_number = (pSec[3] << 8) | pSec[4];
  uint8_t current_next_indicator = pSec[5] & 0x01;
	uint8_t section_number = pSec[6];
	uint8_t last_section_number = pSec[7];
  reserved = pSec[8] & 0xE0;
  int iPCR_PID = ((pSec[8] & 0x1F) << 8) | pSec[9];
	int program_info_length = ((pSec[10] & 0xF) << 8) | pSec[11];

  //parse section
  int len = 12 + program_info_length;

  if (program_info_length == 0) return 0;
	//registration_descriptor()
	//descriptor_tag    8   uimsbf
	//descriptor_length 8   uimsbf
	//format_identifier 32  uimsbf

	//REF: tsFileAnalyzer
	//descriptor_tag|describe
	//0x05          |pmt1stLoopHas_HDMV_registration_descriptor
	//0x88          |pmt1stLoopHas_HDMV_copy_control_descriptor

	uint8_t* pDescriptor = pSec + 12; //section header = 12 bytes
	uint8_t descriptor_tag = pDescriptor[0];
	uint8_t descriptor_length = pDescriptor[1];
	unsigned int format_identifier =
		(pDescriptor[2] <<24)
		| (pDescriptor[3] <<16)
		| (pDescriptor[4] <<8)
		| pDescriptor[5];

	DU_TRACE2(DEBUG, L"descriptor_tag %d descriptor_length %d", descriptor_tag, descriptor_length);

  //first stream
	uint8_t* pStream = pSec + 12 + program_info_length;
	//stream_type     8   uimsbf  [0]
	//reserved        3   bslbf   [1]
	//elementary_PID  13  uimsbf  [1,2]
	//reserved        4   bslbf   [3]
	//ES_info_length  12  uimsbf  [3,4]

	uint8_t stream_type = pStream[0];
	int elementary_PID = ((pStream[1] & 0x1F) << 8) | pStream[2];
	int ES_info_length = ((pStream[3] & 0x0F) << 8) | pStream[4];
  len += 5 + ES_info_length;
	DU_TRACE2(DEBUG, L"stream_type %x elementary_PID %x", stream_type, elementary_PID);

	//descriptor() - 6 bytes
	pDescriptor = pStream + 5;
	descriptor_tag = pDescriptor[0];
	descriptor_length = pDescriptor[1];
	format_identifier =
		(pDescriptor[2] <<24)
		| (pDescriptor[3] <<16)
		| (pDescriptor[4] <<8)
		| pDescriptor[5];
	DU_TRACE2(DEBUG, L"descriptor_tag %d descriptor_length %d format_identifier %x", descriptor_tag, descriptor_length, format_identifier);

	//REF: tsFileAnalyzer
	//format_identifier|describe
	//0x48444D56       |"HDMV" -> HDMV video registration descriptor or HDMV LPCM audio registration descriptor

  //next stream
  while (len < (section_length - 5)) {
    pStream += 5 + ES_info_length;  //ES_info_length = 16; stream hdr = 5
    stream_type = pStream[0];
    elementary_PID = ((pStream[1] & 0x1F) << 8) | pStream[2];
    ES_info_length = ((pStream[3] & 0x0F) << 8) | pStream[4];
    len += 5 + ES_info_length;
    DU_TRACE2(DEBUG, L"stream_type %x elementary_PID %x", stream_type, elementary_PID);

    pDescriptor = pStream + 5;
    descriptor_tag = pDescriptor[0];
    descriptor_length = pDescriptor[1];
    format_identifier =
      (pDescriptor[2] <<24)
      | (pDescriptor[3] <<16)
      | (pDescriptor[4] <<8)
      | pDescriptor[5];
    DU_TRACE2(DEBUG, L"descriptor_tag %d descriptor_length %d format_identifier %x", descriptor_tag, descriptor_length, format_identifier);
  }

	return rc;
}

//PES data
int     parsePES(uint8_t *pData)
{
	int rc = 0;
	uint8_t* pPayLoad = pData;
	int pid = g_PID;

	if (pid == PAT_PID) {
		//PSI
		//pointer_field   8   uimsbf   [0]
		uint8_t pointer_field = pPayLoad[0];
		//is Program Association Table
		VideoInfo vInfo;
		rc = parsePAT(pPayLoad + 1 + pointer_field, &vInfo);
	}
	else if (pid == PMT_PID) {
		rc = parsePM(pPayLoad);
	}
	else if (pid == g_program_map_PID) {
		rc = parsePM(pPayLoad);
	}
	return 0;
}

//adaption field
int     parseAF(uint8_t *pData)
{
	//adaptation_field_length               8 uimsbf
	uint8_t adaptation_field_length = pData[0];

	//discontinuity_indicator               1 bslbf
	//random_access_indicator               1 bslbf
	//elementary_stream_priority_indicator  1 bslbf
	//PCR_flag                              1 bslbf
	//OPCR_flag                             1 bslbf
	//splicing_point_flag                   1 bslbf
	//transport_private_data_flag           1 bslbf
	//adaptation_field_extension_flag       1 bslbf

	enum {
		PCR_flag                    = 0x10,
		OPCR_flag                   = 0x08,
		splicing_point_flag         = 0x04,
		transport_private_data_flag = 0x02,
		adaptation_field_extension_flag = 0x01,
	};

	uint8_t flags = pData[1];
	int rc;

	if (flags != 0) {
		if (flags & transport_private_data_flag) {
			rc = 0;
		}
		if (flags & PCR_flag) {
			rc = 0;
		} else {
			rc = 1;
		}
		rc = 1;
	}

	return 0;
}
