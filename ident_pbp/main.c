#include <pspsdk.h>
#include <pspkernel.h>
#include <pspctrl.h>
#include <pspdisplay.h>
#include <pspge.h>
#include <psprtc.h>
#include <stdio.h>
#include <string.h>

#define VER_MAJOR 1
#define VER_MINOR 1 // 5
#define VER_BUILD "-beta"
//#define VER_BUILD " \"Bourbon\""
//#define VER_BUILD " \"Campari\""

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
	struct tagBITMAPFILEHEADER fhead = {0x4d42, 391734, 0, 0, 54};
	struct tagBITMAPINFOHEADER ihead = {40, 480, 272, 1, 24, 0, 0, 0, 0, 0, 0};

	SceUID fd = sceIoOpen(file, PSP_O_CREAT | PSP_O_TRUNC | PSP_O_RDWR, 0777);
	sceIoWrite(fd, &fhead, sizeof(fhead));
	sceIoWrite(fd, &ihead, sizeof(ihead));

	vptr0 = vramaddr(0, 272 - 1);
	for (h = 0; h < 272; h++) {
		vptr = vptr0;
		u8 buffer[512 * 3];
		int bufidx = 0;

		for (w = 0; w < 480; w++) {
			u8 r = (*vptr & 0x000000FFL);
			u8 g = (*vptr & 0x0000FF00L) >> 8;
			u8 b = (*vptr & 0x00FF0000L) >> 16;

			buffer[bufidx] = b; bufidx++;
			buffer[bufidx] = g; bufidx++;
			buffer[bufidx] = r; bufidx++;

			*vptr = *vptr ^ 0x00FFFFFF; // invert colors
			vptr++;
		}

		sceIoWrite(fd, buffer, 480 * 3);
		vptr0 -= 512;
	}

	sceIoClose(fd);

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
	color(ORANGE);
	int i;
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

int main(int argc, char*argv[]) {
	pspDebugScreenInit();
	pspDebugScreenClear();
	printf("\n pspIdent v%i.%i%s by Yoti\n\n", VER_MAJOR, VER_MINOR, VER_BUILD);

	SceUID mod = pspSdkLoadStartModule("kernel.prx", PSP_MEMORY_PARTITION_KERNEL);
	if (mod < 0) {
		printf("Error: LoadStart() returned 0x%08x\n", mod);
		sceKernelDelayThread(3*1000*1000);
		sceKernelExitGame();
	}

	SceCtrlData pad;
	sceCtrlSetSamplingCycle(0);
	sceCtrlSetSamplingMode(PSP_CTRL_MODE_ANALOG);

	int flag = 0;
	int firmware = sceKernelDevkitVersion();
	char shippedfw[5]; memset(shippedfw, 0, strlen(shippedfw));
		prxIdStorageLookup(0x51, 0, shippedfw, 4);

	int baryon; prxSysconGetBaryonVersion(&baryon);
	unsigned int bromver = prxTachyonGetTimeStamp();
	int polestar; prxSysconGetPolestarVersion(&polestar);
	int pommel; prxSysconGetPommelVersion(&pommel);
	int tachyon = prxSysregGetTachyonVersion();
	char times[64] = "\0"; prxSysconGetTimeStamp(times);

	int fusecfg = prxSysregGetFuseConfig();
	long long fuseid = prxSysregGetFuseId();
	int generation = prxKernelGetModel() + 1;
	char kirk[4]; *(int*)kirk = prxSysregGetKirkVersion();
	int scramble = prxNandGetScramble();
	char spock[4]; *(int*)spock = prxSysregGetSpockVersion();

	char model[64] = "\0";
	char region[2]; memset(region, 0, strlen(region));
		prxIdStorageLookup(0x0100, 0x3D, &region, 1);
	char tlotr[64] = "\0";

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
					sprintf(model, "DEM-1000(?) TMU-001(?) [%02x]", (int)region[0]);
				break;
				case 0x00020601:
					flag = 1;
					sprintf(model, "DTP-T1000 TMU-001 [%02x]", (int)region[0]);
				break;
				case 0x00030601:
					flag = 1;
					sprintf(model, "DTP-H1500/DTP-L1500 TMU-002 [%02x]", (int)region[0]);
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
							/*if (shippedfw[0] == '3')
								strcat(model, "88v1");
							else*/if (shippedfw[0] == '4')
								strcat(model, "88v2");
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
				strcat(model, "/TA-097");
			/*sprintf(model, "PSP-E10%02i", ModelRegion[(int)region[0]]);
			if (shippedfw[2] == '5')
				strcat(model, " TA-096");
			else
				strcat(model, " TA-096/TA-097"); // TODO: proper detection*/
		break;

		default:
			flag = 1;
			strcpy(tlotr, "???");
			strcpy(model, "PSP-?000 TA-0??");
		break;
	}

	if ((generation == 4) && (baryon == 0x002E4000)) {
		strcat(model, " (fake 04g/real 09g)");
	} else {
		char real_gen[16] = "\0";
		sprintf(real_gen, " (%02ig)", generation);
		strcat(model, real_gen);
	}

	color(ORANGE); printf(" *"); color(WHITE);
	printf(" %-10s %x.%x%x (", "Firmware", firmware >> 24,
			(firmware >> 16) & 0xff, (firmware >> 8) & 0xff);
	printf("0x%08x)", firmware);
	if (shippedfw[0] != 0)
		printf(" [%s]", shippedfw);
	printf("\n\n");

	color(RED); printf(" *"); color(WHITE);
	printf(" %-10s 0x%08x [ROM:%08x]\n", "Tachyon", tachyon, bromver);
	color(RED); printf(" *"); color(WHITE);
	printf(" %-10s 0x%08x [%s]\n", "Baryon", baryon, times);
	color(RED); printf(" *"); color(WHITE);
	printf(" %-10s 0x%08x\n", "Pommel", pommel);
	color(RED); printf(" *"); color(WHITE);
	printf(" %-10s 0x%08x\n", "Polestar", polestar); // 01g Polestars are 0x8xxx
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
	color(GREEN); printf(" *"); color(WHITE);
	printf(" %-10s %02ig\n", "Generation", generation);
	printf("\n");

	color(BLUE); printf(" *"); color(WHITE);
	printf(" %s\n", model);
	color(BLUE); printf(" *"); color(WHITE);
	printf(" Call me ");
	color(ORANGE); printf(tlotr); color(WHITE);
	printf(", Gandalf!\n\n");

	if (flag)
		warn();

	sceKernelDelayThread(1*1000*1000);

	printf(" Press X to save screenshot\n");
	printf(" Press O to quit the program\n");
	for(;;) {
		sceCtrlReadBufferPositive(&pad, 1);
		if (pad.Buttons & PSP_CTRL_CROSS) {
			int i;
			pspDebugScreenSetXY(0, pspDebugScreenGetY()-2);
			for (i = 0; i < 32; i++)
				printf(" ");
			pspDebugScreenSetXY(0, pspDebugScreenGetY()+1);
			for (i = 0; i < 32; i++)
				printf(" ");
			pspDebugScreenSetXY(0, pspDebugScreenGetY()-1);

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
				sprintf(file, "%s/%04d%02d%02d_%02d%02d%02d.bmp", dir,
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

			printf(" Screenshot was saved to %s!\n", file);
			printf(" The program will automatically quit after 8 seconds\n");
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
