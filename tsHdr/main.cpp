#include <stdio.h>
#include <string.h>
#include "TsPacket.h"


class testclass
{
public:
	static int func1() {return 1;}
};

void combie()
{
	char *orgfile = "C:\\tmp\\org\\MOVIE\\10s.ts";
	char *adjfile = "C:\\tmp\\adjust\\10s_adjusted.ts";
	char *cmbfile = "C:\\tmp\\adjust\\10s_combie.ts";
	FILE *forg = 0;
	FILE *fadj = 0;
	FILE *fcmb = 0;

	uint8_t buff[PACKET_SIZE];

	fopen_s(&forg, orgfile, "rb");
	fopen_s(&fadj, adjfile, "rb");
	fopen_s(&fcmb, cmbfile, "wb+");

	//copy 15 packets from org
	int i;
	for (i = 0; i < 15; i++)
	{
		fread(buff, sizeof(buff), 1, forg);
		fwrite(buff, sizeof(buff), 1, fcmb);
	}
	//copy all packets from adj
	while (fread(buff, sizeof(buff), 1, fadj))
	{
		fwrite(buff, sizeof(buff), 1, fcmb);
	}

	fclose(forg);
	fclose(fadj);
	fclose(fcmb);
}

int adjust(char*zfile)
{
	int i = testclass::func1();

	//char *tsfile = "C:\\tmp\\filets.ts";
	char tsfile[MAX_PATH];
	strncpy_s(tsfile, zfile, sizeof(tsfile));
	//char *tsfile = "C:\\tmp\\org\\pp_ntsc.ts";
	char *tsAdjustFile = "C:\\tmp\\adjust\\40s_adjusted.ts";
	FILE *fin = 0;
	FILE *fout = 0;
	uint8_t buff[PACKET_SIZE];
	//char *logFile = "c:\\tmp\\log.txt";

	//calc adjust = PRERECF_CalcPtsAdjmt( pts of GOP[i], base PTS)
	//__int64 adjust = PRERECF_CalcPtsAdjmt(9717663, 78033);
	__int64 adjust = 0;

	fopen_s(&fin, tsfile, "rb");
	if (adjust != 0)
	{
		fopen_s(&fout, tsAdjustFile, "wb+");
		if (fout) printf("open file %s OK\n", tsAdjustFile);
	}

	if (fin)
	{
		g_count = 0;
		g_iGOP = 0;
		size_t rc;
		printf("open file %s OK\n", tsfile);

		int program_map_PID = 0;

		while(1)
		{
			rc = fread(buff, PACKET_SIZE, 1, fin);
			if (rc != 0)
			{
				g_count++;
				g_PID = getPID(buff);
				unpackTransportPacket(buff, 0, 0, 0);
#ifdef ADJUST_PTS
				//adjust pts
				adjustTransportPacket(buff, g_PID, adjust);
        if (g_PID == PAT_PID) {
          DU_TRACE2(DEBUG, L"[PAT]");
        }
				//adjustPacketTsValue(buff, 0);
				if (PacketIsStartOfGOP(buff))
					DU_TRACE2(DEBUG, L"[overPTS] start of GOP %d", g_iGOP++);
				if (fout) fwrite(buff, PACKET_SIZE, 1, fout);
#endif
#ifdef PARSE_PAT
				//get video info from packet
				VideoInfo vinfo;
				if (program_map_PID == 0) {
					program_map_PID = getVideoInfo(buff, g_PID, &vinfo);
				} else {
					if (g_PID == program_map_PID) {
						//getVideoInfo(buff, g_PID, &vinfo);
						uint8_t* pData = getESDataPartial(buff);
						parsePM(pData);
					}
				}
#endif
#ifdef PARSE_PES
				//parse PES
				//if (g_PID == VIDEO_PID || g_PID == AUDIO_PID)
				{
					uint8_t* pPES = 0;
					uint8_t* pAF = 0;
					int flags = splitTransportPacket(buff, &pPES, &pAF);
					if (pPES) parsePES(pPES);
					if (pAF) parseAF(pAF);
				}
#endif
			}
			else {
				break;
			}
		}
		fclose(fin);
		if (fout) fclose(fout);
	}
	return 0;
}

//[dvioff]
void split()
{
	int size1 = 6802560;

	//char *tsfile = "C:\\tmp\\filets.ts";
	char *cmbFile = "C:\\Users\\khiemnv1\\Desktop\\dvioff\\data.txt";
	char *file1 = "C:\\Users\\khiemnv1\\Desktop\\dvioff\\12gop.ts";
	char *file2 = "C:\\Users\\khiemnv1\\Desktop\\dvioff\\blank.ts";

	FILE *fin = 0;
	FILE *fout1 = 0;
	FILE *fout2 = 0;

	uint8_t buff[PACKET_SIZE];

	__int64 adjust = 0;

	int split = 1;
	const int packetSize = PACKET_SIZE;

	fopen_s(&fin, cmbFile, "rb");
	if (split != 0)
	{
		fopen_s(&fout1, file1, "wb+");
		if (fout1) printf("open file %s OK\n", file1);
		fopen_s(&fout2, file2, "wb+");
		if (fout2) printf("open file %s OK\n", file2);
	}

	if (fin)
	{
		size_t rc;
		int nWrite1 = 0;
		printf("open file %s OK\n", cmbFile);
		
		while(1)
		{
			rc = fread(buff, packetSize, 1, fin);
			if (rc != 0)
			{
				if (nWrite1 < size1) {
					if (fout1) {
						fwrite(buff, packetSize, 1, fout1);
						nWrite1 += PACKET_SIZE;
					}
				} else {
					if (fout2) fwrite(buff, packetSize, 1, fout2);
				}
			}
			else
			break;
		}

		fclose(fin);
		if (fout1) fclose(fout1);
		if (fout2) fclose(fout2);
	}
}

int main(int argc, char** argv)
{
	if (argc >= 2)
	{
		printf(">%s %s\n", argv[0], argv[1]);

		//combie();
		adjust(argv[1]);
		//split();
	} else {
#if(0)
    //char *zfile = "E:\\work\\XAVC\\Z Sys 4k XAVC For Square Box\\XDROOT\\Clip\\Clip0010.MXF";
#else
		char *zfile = "C:\\tmp\\adjust\\10s.TS";
		//char *zfile = "C:\\tmp\\pack_nosound.ts";
		//char *zfile = "C:\\tmp\\pack";
#endif
    //char *zfile = "E:\\work\\KhoaLH\\IMG_0133.ts";
    //char* zfile = "D:\\docs\\projs\\spliter\\Debug\\out.ts";

		printf(">%s %s\n", argv[0], zfile);

		//combie();
		adjust(zfile);
	}
	return 0;
}
