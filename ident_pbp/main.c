#include <pspsdk.h>
#include <pspkernel.h>
#include <pspctrl.h>
#include <pspdisplay.h>
#include <pspge.h>
#include <psprtc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../lodepng/lodepng.h"

#define VER_MAJOR 2
#define VER_MINOR 5
#define VER_BUILD " \"Drambuie\""

PSP_MODULE_INFO("pspIdent", 0, VER_MAJOR, VER_MINOR);
PSP_MAIN_THREAD_ATTR(0);
PSP_HEAP_SIZE_KB(1024);

#include "../kernel_prx/kernel_prx.h"
#include "main.h"

u32*vramaddr(int x, int y) {
	u32 *vram;

	vram = (void *)(0x40000000 | (u32)sceGeEdramGetAddr());
	vram += x;
	vram += y * 512;

	return vram;
}

void savepict(const char*file) {
	int h, w;
	u32*vptr; u32*vptr0;
	unsigned char*image = malloc(480 * 272 * 4);

	vptr0 = vramaddr(0, 272 - 1);
	for (h = 272; h > 0; h--) {
		vptr = vptr0;

		for (w = 480; w > 0; w--) {
			u8 r = (*vptr & 0x000000FFL);
			u8 g = (*vptr & 0x0000FF00L) >> 8;
			u8 b = (*vptr & 0x00FF0000L) >> 16;

			image[4 * 480 * h - 4 * w + 0] = r;
			image[4 * 480 * h - 4 * w + 1] = g;
			image[4 * 480 * h - 4 * w + 2] = b;
			image[4 * 480 * h - 4 * w + 3] = 0xFF;

			*vptr = *vptr ^ 0x00FFFFFF; // invert colors
			vptr++;
		}

		vptr0 -= 512;
	}

	lodepng_encode32_file(file, image, 480, 272);
	free(image);

	// revert back colors
	vptr0 = vramaddr(0, 272 - 1);
	for (h = 0; h < 272; h++) {
		vptr = vptr0;
		for (w = 0; w < 480; w++) {
			*vptr = *vptr ^ 0x00FFFFFF;
			vptr++;
		}
		vptr0 -= 512;
	}
}

void warn(void) {
	int i;
	color(YELLOW);
	for (i = 0; i < 51; i++)
		printf("-");
	printf("\n");
	printf(" Please send this screenshot to the Yoti\'s GitHub!\n");
	printf("  https://github.com/Yoti/psp_pspident/issues/new\n");
	for (i = 0; i < 51; i++)
		printf("-");
	printf("\n\n");
	color(WHITE);
}

void version_txt(void) {
	int i;
	int size;
	SceUID fd;
	unsigned char version[512];
	unsigned char tab[4] = "   \0";
	memset(version, 0, sizeof(version));

	fd = sceIoOpen("flash0:/vsh/etc/version.txt", PSP_O_RDONLY, 0777);
	if (fd >= 0) {
		size = sceIoRead(fd, version, sizeof(version));
		sceIoClose(fd);
		color(LGRAY);
		printf("%s", tab);
		for (i = 0; i < size - 1; i++) {
			if (version[i] == 0x0a)
				printf("\n%s", tab);
			else if (version[i] != 0x0d)
				printf("%c", version[i]);
		}
		printf("\n");
		color(WHITE);
	}
}

int main(int argc, char*argv[]) {
	pspDebugScreenInit();
	pspDebugScreenClear();
	printf("\n pspIdent v%i.%i%s by Yoti\n\n", VER_MAJOR, VER_MINOR, VER_BUILD);

	int firmware = sceKernelDevkitVersion();

	SceUID mod = pspSdkLoadStartModule("kernel.prx", PSP_MEMORY_PARTITION_KERNEL);
	if (mod < 0) {
		color(ORANGE); printf(" *"); color(WHITE);
		printf(" %-10s %x.%x%x (0x%08x)", "Firmware", firmware >> 24,
				(firmware >> 16) & 0xff, (firmware >> 8) & 0xff, firmware);
		printf("\n");
		version_txt();
		printf("\n");

		color(ORANGE);
		printf(" Error: pspSdkLoadStartModule() returned 0x%08x\n", mod);
		printf(" The program will automatically quit after 8 seconds...\n");
		color(WHITE);
		sceKernelDelayThread(8*1000*1000);
		sceKernelExitGame();
	}

	SceCtrlData pad;
	sceCtrlSetSamplingCycle(0);
	sceCtrlSetSamplingMode(PSP_CTRL_MODE_ANALOG);

	int flag = 0;
	char shippedfw[5]; memset(shippedfw, 0, sizeof(shippedfw));
		prxIdStorageLookup(0x51, 0, shippedfw, 4);

	int baryon; prxSysconGetBaryonVersion(&baryon);
	unsigned int bromver = prxTachyonGetTimeStamp();
	int polestar; prxSysconGetPolestarVersion(&polestar);
	int pommel; prxSysconGetPommelVersion(&pommel);
	int tachyon = prxSysregGetTachyonVersion();
	char times[64]; memset(times, 0, sizeof(times));
		prxSysconGetTimeStamp(times);

	int fusecfg = prxSysregGetFuseConfig();
	long long fuseid = prxSysregGetFuseId();
	int generation = prxKernelGetModel() + 1;
	char kirk[4]; *(int*)kirk = prxSysregGetKirkVersion();
	int scramble = prxNandGetScramble();
	char spock[4]; *(int*)spock = prxSysregGetSpockVersion();

	char model[64]; memset(model, 0, sizeof(model));
	unsigned char idsbtmac[7]; memset(idsbtmac, 0, sizeof(idsbtmac));
		prxIdStorageLookup(0x0050, 0x41, &idsbtmac, 6);
	char idswfreg[2]; memset(idswfreg, 0, sizeof(idswfreg));
		prxIdStorageLookup(0x0045, 0x00, &idswfreg, 1);
		if ((int)idswfreg[0] < 0)
			idswfreg[0] = '\1';
		else if ((int)idswfreg[0] > ((sizeof(WiFiRegion) / sizeof(WiFiRegion[0])) - 1))
			idswfreg[0] = '\1';
		if ((int)idswfreg[0] == 1)
			flag = 1;
	char c2dreg[2]; memset(c2dreg, 0, sizeof(c2dreg));
		prxIdStorageLookup(0x0100, 0x3D, &c2dreg, 1);
	char region[2]; memset(region, 0, sizeof(region));
		prxIdStorageLookup(0x0100, 0xF5, &region, 1);
	char c2dqaf[2]; memset(c2dqaf, 0, sizeof(c2dqaf));
		prxIdStorageLookup(0x0100, 0x40, &c2dqaf, 1);
	char qaflag[2]; memset(qaflag, 0, sizeof(qaflag));
		prxIdStorageLookup(0x0100, 0xF8, &qaflag, 1);
	char tlotr[64]; memset(tlotr, 0, sizeof(tlotr));

	switch(tachyon) {
		case 0x00140000: // TA-079v1/2/3
			strcpy(tlotr, "First");
			sprintf(model, "PSP-10%02i", ModelRegion[(int)region[0]]);
			strcat(model, " TA-079v");

			switch(baryon) {
				case 0x00010600:
					strcat(model, "1");
				break;
				case 0x00020600:
					strcat(model, "2");
				break;
				case 0x00030600:
					strcat(model, "3");
				break;

				case 0x00010601:
					flag = 1;
					strcat(tlotr, " (Proto Tool)");
					strcpy(model, "DEM-1000(?) TMU-001(?)");
				break;
				case 0x00020601:
					if (fusecfg != 0x2501)
						flag = 1;
					strcat(tlotr, " (Dev Tool)");
					strcpy(model, "DTP-T1000 TMU-001");
				break;
				case 0x00030601:
					if ((int)region[0] == 0x02) {
						if (fusecfg != 0x2501)
							flag = 1;
						strcat(tlotr, " (Test Tool)");
						strcpy(model, "DTP-H1500 TMU-002");
					} else {//if ((int)region[0] == 0x0e)
						if (fusecfg != 0x1b01)
							flag = 1;
						strcat(tlotr, " (Test Tool for AV)");
						strcpy(model, "DTP-L1500 TMU-002");
					}
				break;

				default:
					flag = 1;
					strcat(model, "?");
				break;
			}
		break;

		case 0x00200000: // TA-079 v4/5
			strcpy(tlotr, "First");
			sprintf(model, "PSP-10%02i", ModelRegion[(int)region[0]]);
			strcat(model, " TA-079v");

			switch(baryon) {
				case 0x00030600:
					strcat(model, "4");
				break;
				case 0x00040600:
					strcat(model, "5");
				break;
				default:
					flag = 1;
					strcat(model, "?");
				break;
			}
		break;

		case 0x00300000: // TA-081v1/2
			strcpy(tlotr, "First");
			sprintf(model, "PSP-10%02i", ModelRegion[(int)region[0]]);
			strcat(model, " TA-081v");

			switch(pommel) {
				case 0x00000103:
					strcat(model, "1");
				break;
				case 0x00000104:
					strcat(model, "2");
				break;
				default:
					flag = 1;
					strcat(model, "?");
				break;
			}
		break;

		case 0x00400000: // TA-082/6
			strcpy(tlotr, "Legolas");
			sprintf(model, "PSP-10%02i", ModelRegion[(int)region[0]]);
			strcat(model, " TA-08");

			switch(baryon) {
				case 0x00114000:
					strcat(tlotr, "1"); // Legolas1
					strcat(model, "2"); // TA-082
				break;
				case 0x00121000:
					strcat(tlotr, "2"); // Legolas2
					strcat(model, "6"); // TA-086
				break;
				default:
					flag = 1;
					strcat(tlotr, "?"); // Legolas?
					strcat(model, "?"); // TA-08?
				break;
			}
		break;

		case 0x00500000: // TA-085/8
			strcpy(tlotr, "Frodo");
			sprintf(model, "PSP-20%02i", ModelRegion[(int)region[0]]);
			strcat(model, " TA-0");

			switch(baryon) {
				case 0x0022B200:
					strcat(model, "85v1");
				break;
				case 0x00234000:
					strcat(model, "85v2");
				break;
				case 0x00243000:
					switch(pommel) {
						case 0x00000123:
							// v1: 3.71/3.90/3.95
							// v2: 3.95
							if (shippedfw[0] != '5')
								strcat(model, "88v1");
							else
								strcat(model, "88v1/v2"); // TODO: proper detection
						break;
						case 0x00000132:
							flag = 1; // TODO: remove after proof
							strcat(model, "90v1");
						break;
						default:
							flag = 1;
							strcat(model, "??"); // TA-0??
						break;
					}
				break;
				default:
					flag = 1;
					strcat(model, "??"); // TA-0??
				break;
			}
		break;

		case 0x00600000:
			switch(baryon) {
				case 0x00234000:
					strcpy(tlotr, "Frodo");
					sprintf(model, "PSP-20%02i TA-088v3/TA-085v2 (hybrid)", ModelRegion[(int)region[0]]);
				break;
				case 0x00243000:
					strcpy(tlotr, "Frodo");
					sprintf(model, "PSP-20%02i TA-088v3", ModelRegion[(int)region[0]]);
				break;
				case 0x00263100:
					strcpy(tlotr, "Samwise");
					sprintf(model, "PSP-30%02i TA-090v", ModelRegion[(int)region[0]]);
					switch(pommel) {
						case 0x00000132:
							strcat(model, "2"); // TA-090v2
						break;
						case 0x00000133:
							strcat(model, "3"); // TA-090v3
						break;
						default:
							flag = 1;
							strcat(model, "?"); // TA-090v?
						break;
					}
				break;
				case 0x00285000:
					strcpy(tlotr, "Samwise");
					sprintf(model, "PSP-30%02i TA-092", ModelRegion[(int)region[0]]);
				break;
				default:
					flag = 1;
					strcpy(tlotr, "???");
					strcpy(model, "PSP-?000 TA-0??");
				break;
			}
		break;

		case 0x00720000:
			strcpy(tlotr, "Strider");
			sprintf(model, "PSP-N10%02i TA-091", ModelRegion[(int)region[0]]);
		break;

		case 0x00810000:
			strcpy(tlotr, "Samwise VA2");
			sprintf(model, "PSP-30%02i TA-09", ModelRegion[(int)region[0]]);
			switch(baryon) {
				case 0x002C4000:
					strcat(model, "3v"); // TA-093v
					switch(pommel) {
						case 0x00000141:
							strcat(model, "1"); // TA-093v1
						break;
						case 0x00000143:
							strcat(model, "2"); // TA-093v2
						break;
						default:
							flag = 1;
							strcat(model, "?"); // TA-093v?
						break;
					}
				break;
				case 0x002E4000:
					strcat(model, "5v1"); // TA-095v1 [09g]
				break;
				case 0x012E4000:
					strcat(model, "5v3"); // TA-095v3 [07g]
				break;
				default:
					flag = 1;
					strcat(model, "?"); // TA-09?
				break;
			}
		break;

		case 0x00820000:
			strcpy(tlotr, "Samwise VA2");
			sprintf(model, "PSP-30%02i TA-095v", ModelRegion[(int)region[0]]);
			switch(baryon) {
				case 0x002E4000:
					strcat(model, "2"); // TA-095v2 [09g]
				break;
				case 0x012E4000:
					strcat(model, "4"); // TA-095v4 [07g]
				break;
				default:
					flag = 1;
					strcat(model, "?"); // TA-095v?
				break;
			}
		break;

		case 0x00900000:
			strcpy(tlotr, "Bilbo");
			sprintf(model, "PSP-E10%02i TA-096", ModelRegion[(int)region[0]]);
			if (shippedfw[2] != '5')
				strcat(model, "/TA-097"); // TODO: proper detection
		break;

		case 0x8002013a:
			u64 tick;
			sceRtcGetCurrentTick(&tick);
			srand(tick);
			printf("%s", wiki[rand() % (sizeof(wiki) / sizeof(wiki[0]))]);
			sceKernelDelayThread(8*1000*1000);
			sceKernelExitGame();
		break;

		default:
			flag = 1;
			strcpy(tlotr, "???");
			strcpy(model, "PSP-?000 TA-0??");
		break;
	}

	if (((unsigned char)c2dreg[0] == 0x02) && ((unsigned char)region[0] != (unsigned char)c2dreg[0]))
		strcat(tlotr, " (fake Test Tool)");

	if ((generation == 4) && (baryon == 0x002E4000)) {
		strcat(model, " (fake 04g/real 09g)");
	} else {
		char real_gen[16] = "\0";
		sprintf(real_gen, " (%02ig)", generation);
		strcat(model, real_gen);
	}

	if ((unsigned char)c2dqaf[0] == 0x8C) { // QAF
		if ((unsigned char)qaflag[0] != (unsigned char)c2dqaf[0])
			strcat(model, " [fake QAF]");
		else
			strcat(model, " [QAF]");
	} else { // non-QAF
		strcat(model, " [non-QAF]");
	}

	color(ORANGE); printf(" *"); color(WHITE);
	printf(" %-10s %x.%x%x (0x%08x)\n", "Firmware", firmware >> 24,
			(firmware >> 16) & 0xff, (firmware >> 8) & 0xff, firmware);
	version_txt();
	if (shippedfw[0] != 0) {
		color(ORANGE); printf(" *"); color(WHITE);
		printf(" %-10s %s\n", "Shipped FW", shippedfw);
	}
	printf("\n");

	color(RED); printf(" *"); color(WHITE);
	printf(" %-10s 0x%08x [ROM:%08x]\n", "Tachyon", tachyon, bromver);
	color(RED); printf(" *"); color(WHITE);
	printf(" %-10s 0x%08x [%s]\n", "Baryon", baryon, times);
	color(RED); printf(" *"); color(WHITE);
	printf(" %-10s 0x%08x\n", "Pommel", pommel);
	color(RED); printf(" *"); color(WHITE);
	printf(" %-10s 0x%08x\n", "Polestar", polestar); // 01g Polestars are 0x8###
	printf("\n");

	color(GREEN); printf(" *"); color(WHITE);
	printf(" %-10s 0x%c%c%c%c\n", "Kirk", kirk[3], kirk[2], kirk[1], kirk[0]);
	if (generation != 5) {
		color(GREEN); printf(" *"); color(WHITE);
		printf(" %-10s 0x%c%c%c%c\n", "Spock", spock[3], spock[2], spock[1], spock[0]);
	}
	color(GREEN); printf(" *"); color(WHITE);
	printf(" %-10s 0x%llx\n", "FuseId", fuseid);
	color(GREEN); printf(" *"); color(WHITE);
	printf(" %-10s 0x%04x\n", "FuseCfg", fusecfg);
	color(GREEN); printf(" *"); color(WHITE);
	printf(" %-10s 0x%08x\n", "Scramble", scramble);
	printf("\n");

	color(BLUE); printf(" *"); color(WHITE);
	printf(" %s\n", model);
	color(BLUE); printf(" *"); color(WHITE);
	if (generation == 5)
		printf(" Bluetooth MAC: %02x:%02x:%02x:%02x:%02x:%02x\n",
		idsbtmac[0], idsbtmac[1], idsbtmac[2], idsbtmac[3], idsbtmac[4], idsbtmac[5]);
	else
		printf(" UMD drive FW: [xxxxxxxxxx]\n");*/
	if (generation < 11) {
		color(BLUE); printf(" *"); color(WHITE);
		printf(" Wi-Fi region: %s\n", WiFiRegion[(int)idswfreg[0]]);
	}
	color(BLUE); printf(" *"); color(WHITE);
	printf(" Call me ");
	color(ORANGE); printf(tlotr); color(WHITE);
	printf(", Gandalf!\n\n");

	if (flag)
		warn();

	sceKernelDelayThread(1*1000*1000);

	printf(" Press X to save screenshot or O to quit the program\n");
	for(;;) {
		sceCtrlReadBufferPositive(&pad, 1);
		if (pad.Buttons & PSP_CTRL_CROSS) {
			int i;
			pspDebugScreenSetXY(0, pspDebugScreenGetY()-1);
			for (i = 0; i < 54; i++)
				printf(" ");
			pspDebugScreenSetXY(0, pspDebugScreenGetY());

			char dir[128] = "\0";
			sprintf(dir, "%c%c0:/PICTURE", argv[0][0], argv[0][1]);
			sceIoMkdir(dir, 0777);
			strcat(dir, "/pspIdent");
			sceIoMkdir(dir, 0777);

			SceUID fd;
			pspTime time;
			char file[128] = "\0";
			for(;;) {
				sceRtcGetCurrentClockLocalTime(&time);
				sprintf(file, "%s/%04d%02d%02d_%02d%02d%02d.png", dir,
						time.year, time.month, time.day,
						time.hour, time.minutes, time.seconds);
				fd = sceIoOpen(file, PSP_O_RDONLY, 0777);
				if (fd < 0) {
					savepict(file);
					break;
				} else {
					sceIoClose(fd);
				}
				sceKernelDelayThread(1*1000*1000);
			}

			color(ORANGE);
			printf(" Screenshot was saved to %s!\n", file);
			printf(" The program will automatically quit after 8 seconds...\n");
			color(WHITE);
			sceKernelDelayThread(8*1000*1000);
			break;
		} else if (pad.Buttons & PSP_CTRL_CIRCLE) {
			break;
		}
		sceDisplayWaitVblank();
	}

	sceKernelExitGame();
	return 0;
}
