#include <pspsdk.h>
#include <pspkernel.h>
#include <pspctrl.h>
#include <pspdisplay.h>
#include <pspge.h>
#include <psprtc.h>
#include <stdio.h>
#include <string.h>

#define VER_MAJOR 1
#define VER_MINOR 0
#define VER_BUILD "-beta"

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
	struct tagBITMAPFILEHEADER fhead = {0x4d42, 391734, 0, 0, 54};
	struct tagBITMAPINFOHEADER ihead = {40, 480, 272, 1, 24, 0, 0, 0, 0, 0, 0};

	SceUID fd = sceIoOpen(file, PSP_O_CREAT | PSP_O_TRUNC | PSP_O_RDWR, 0777);
	sceIoWrite(fd, &fhead, sizeof(fhead));
	sceIoWrite(fd, &ihead, sizeof(ihead));

	u32*vptr0 = vramaddr(0, 272 - 1);

	for (h = 0; h < 272; h++) {
		u32*vptr = vptr0;
		u8 buffer[512 * 3];
		int bufidx = 0;

		for (w = 0; w < 480; w++) {
			u32 p = *vptr;

			u8 r = (p & 0x000000FFL);
			u8 g = (p & 0x0000FF00L) >> 8;
			u8 b = (p & 0x00FF0000L) >> 16;

			buffer[bufidx] = b; bufidx++;
			buffer[bufidx] = g; bufidx++;
			buffer[bufidx] = r; bufidx++;

			vptr++;
		}

		sceIoWrite(fd, buffer, 480 * 3);
		vptr0 -= 512;
	}

	sceIoClose(fd);
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

	int generation = prxKernelGetModel() + 1;
	int baryon; prxSysconGetBaryonVersion(&baryon);
	int bromver = prxTachyonGetTimeStamp();
	int pommel; prxSysconGetPommelVersion(&pommel);
	int polestar; prxSysconGetPolestarVersion(&polestar);
	int fusecfg = prxSysregGetFuseConfig();
	u64 fuseid = prxSysregGetFuseId();
	unsigned int scramble = prxNandGetScramble();
	char kirk[4]; *(int*)kirk = prxSysregGetKirkVersion();
	char spock[4]; *(int*)spock = prxSysregGetSpockVersion();
	int tachyon = prxSysregGetTachyonVersion();

	char model[64] = "\0";
	char tlotr[64] = "\0";
	char times[64] = "\0"; prxSysconGetTimeStamp(times);

	switch(tachyon) {
		case 0x00140000:
			sprintf(tlotr, "First");
			sprintf(model, "%s", "PSP-1000 TA-079v");

			switch(baryon) {
				case 0x00010600:
					sprintf(model, "%s1", model);
				break;
				case 0x00020600:
					sprintf(model, "%s2", model);
				break;
				case 0x00030600:
					sprintf(model, "%s3", model);
				break;
				default:
					flag = 1;
					sprintf(model, "%s?", model);
				break;
			}
		break;

		case 0x00200000:
			sprintf(tlotr, "First");
			sprintf(model, "%s", "PSP-1000 TA-079v");

			switch(baryon) {
				case 0x00030600:
					sprintf(model, "%s4", model);
				break;
				case 0x00040600:
					sprintf(model, "%s5", model);
				break;
				default:
					flag = 1;
					sprintf(model, "%s?", model);
				break;
			}
		break;

		case 0x00300000:
			sprintf(tlotr, "First");
			sprintf(model, "%s", "PSP-1000 TA-081v");

			switch(pommel) {
				case 0x00000103:
					sprintf(model, "%s1", model);
				break;
				case 0x00000104:
					sprintf(model, "%s2", model);
				break;
				default:
					flag = 1;
					sprintf(model, "%s?", model);
				break;
			}
		break;

		case 0x00400000:
			sprintf(tlotr, "Legolas");
			sprintf(model, "%s", "PSP-1000 TA-08");

			switch(baryon) {
				case 0x00114000:
					sprintf(tlotr, "%s1", tlotr); // Legolas1
					sprintf(model, "%s2", model); // TA-082
				break;
				case 0x00121000:
					sprintf(tlotr, "%s2", tlotr); // Legolas2
					sprintf(model, "%s6", model); // TA-086
				break;
				default:
					flag = 1;
					sprintf(tlotr, "%s?", tlotr); // Legolas?
					sprintf(model, "%s?", model); // TA-08?
				break;
			}
		break;

		case 0x00500000:
			sprintf(tlotr, "Frodo");
			sprintf(model, "%s", "PSP-2000 TA-0");

			switch(baryon) {
				case 0x0022B200:
					sprintf(model, "%s%s", model, "85v1");
				break;
				case 0x00234000:
					sprintf(model, "%s%s", model, "85v2");
				break;
				case 0x00243000:
					switch(pommel) {
						case 0x00000123:
							sprintf(model, "%s%s", model, "88v1/v2"); // TODO: proper detection
						break;
						case 0x00000132:
							flag = 1; // TODO: remove after proof
							sprintf(model, "%s%s", model, "90v1");
						break;
						default:
							flag = 1;
							sprintf(model, "%s%s", model, "??"); // TA-0??
						break;
					}
				break;
				default:
					flag = 1;
					sprintf(model, "%s%s", model, "??"); // TA-0??
				break;
			}
		break;

		case 0x00600000:
			sprintf(model, "%s", "PSP-");
			switch(baryon) {
				case 0x00243000:
					sprintf(tlotr, "Frodo");
					sprintf(model, "%s%s", model, "2000 TA-088v3");
				break;
				case 0x00263100:
					sprintf(tlotr, "Samwise");
					sprintf(model, "%s%s", model, "3000 TA-090v");
					switch(pommel) {
						case 0x00000132:
							sprintf(model, "%s%s", model, "2");
						break;
						case 0x00000133:
							sprintf(model, "%s%s", model, "3");
						break;
						default:
							flag = 1;
							sprintf(model, "%s%s", model, "?"); // TA-090v?
						break;
					}
				break;
				case 0x00285000:
					sprintf(tlotr, "Samwise");
					sprintf(model, "%s%s", model, "3000 TA-092");
				break;
				default:
					flag = 1;
					sprintf(tlotr, "???");
					sprintf(model, "%s%s", model, "?000 TA-0??");
				break;
			}
		break;

		case 0x00720000:
			strcpy(tlotr, "Strider");
			strcpy(model, "PSP-N1000 TA-091");
		break;

		case 0x00810000:
			sprintf(tlotr, "Samwise VA2");
			sprintf(model, "%s", "PSP-3000 TA-09");
			switch(baryon) {
				case 0x002C4000:
					sprintf(model, "%s%s", model, "3v"); // TA-093v
					switch(pommel) {
						case 0x00000141:
							sprintf(model, "%s%s", model, "1"); // TA-093v1
						break;
						case 0x00000143:
							sprintf(model, "%s%s", model, "2"); // TA-093v2
						break;
						default:
							flag = 1;
							sprintf(model, "%s%s", model, "?"); // TA-093v?
						break;
					}
				break;
				case 0x002E4000:
					sprintf(model, "%s%s/v1a", model, "5v1"); // TA-095v1
				break;
				case 0x012E4000:
					sprintf(model, "%s%s/v1b", model, "5v3"); // TA-095v3 [07g]
				break;
				default:
					flag = 1;
					sprintf(model, "%s%s", model, "?"); // TA-09?
				break;
			}
		break;

		case 0x00820000:
			sprintf(tlotr, "Samwise VA2");
			sprintf(model, "%s", "PSP-3000 TA-095v");
			switch(baryon) {
				case 0x002E4000:
					sprintf(model, "%s%s/v2a", model, "2"); // TA-095v2
				break;
				case 0x012E4000:
					sprintf(model, "%s%s/v2b", model, "4"); // TA-095v4 [07g]
				break;
				default:
					flag = 1;
					sprintf(model, "%s%s", model, "?"); // TA-095v?
				break;
			}
		break;

		case 0x00900000:
			strcpy(tlotr, "Bilbo");
			strcpy(model, "PSP-E1000 TA-096/TA-097");
		break;

		default:
			flag = 1;
			strcpy(tlotr, "???");
			strcpy(model, "PSP-?000 TA-0??");
		break;
	}

	if ((generation == 4) && (baryon == 0x002E4000)) {
		sprintf(model, "%s (fake 04g/real 09g)", model);
	} else {
		sprintf(model, "%s (%02ig)", model, generation);
	}

	color(ORANGE); printf(" *"); color(WHITE);
	printf(" %-10s %x.%x%x (", "Firmware", firmware >> 24,
			(firmware >> 16) & 0xff, (firmware >> 8) & 0xff);
	printf("0x%08x)\n", firmware);
	printf("\n");

	color(RED); printf(" *"); color(WHITE);
	printf(" %-10s 0x%08x [ROM:%08x]\n", "Tachyon", tachyon, bromver);
	color(RED); printf(" *"); color(WHITE);
	printf(" %-10s 0x%08x [%s]\n", "Baryon", baryon, times);
	color(RED); printf(" *"); color(WHITE);
	printf(" %-10s 0x%08x\n", "Pommel", pommel);
	color(RED); printf(" *"); color(WHITE);
	//if (polestar && 0x???? != 0x????) {...}
	printf(" %-10s 0x%08x\n", "Polestar", polestar);
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
			printf(" The program will automatically quit after 4 seconds\n");
			sceKernelDelayThread(4*1000*1000);
			break;
		} else if (pad.Buttons & PSP_CTRL_CIRCLE) {
			break;
		}
		sceDisplayWaitVblank();
	}

	sceKernelExitGame();
	return 0;
}
