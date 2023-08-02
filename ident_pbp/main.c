#include <pspsdk.h>
#include <pspkernel.h>
#include <pspctrl.h>
#include <pspge.h>
#include <stdio.h>
#include <string.h>

#define VER_MAJOR 1
#define VER_MINOR 0
#define VER_BUILD ""

PSP_MODULE_INFO("pspIdent", 0, VER_MAJOR, VER_MINOR);
PSP_MAIN_THREAD_ATTR(0);
PSP_HEAP_SIZE_KB(1024);

#include "../kernel_prx/kernel_prx.h"
#define printf pspDebugScreenPrintf

u32*vramaddr(int x, int y) {
	u32 *vram;

	vram = (void *)(0x40000000 | (u32)sceGeEdramGetAddr());
	vram += x;
	vram += y * 512;

	return vram;
}

u8 picthead[] = {
	0x42, 0x4D, 0x36, 0xFA, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x36, 0x00, 0x00, 0x00, 0x28, 0x00,
	0x00, 0x00, 0xE0, 0x01, 0x00, 0x00, 0x10, 0x01, 0x00, 0x00, 0x01, 0x00, 0x18, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

void savepict(const char*file) {
	int h, w;

	SceUID fd = sceIoOpen(file, PSP_O_CREAT | PSP_O_TRUNC | PSP_O_RDWR, 0777);
	sceIoWrite(fd, picthead, sizeof(picthead));

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

	u32 firmware = sceKernelDevkitVersion();

	u32 generation = prxKernelGetModel() + 1;
	u32 baryon; prxSysconGetBaryonVersion(&baryon);
	u32 pommel; prxSysconGetPommelVersion(&pommel);
	u32 fusecfg = prxSysregGetFuseConfig();
	u64 fuseid = prxSysregGetFuseId();
	char kirk[4]; *(u32*)kirk = prxSysregGetKirkVersion();
	char spock[4]; *(u32*)spock = prxSysregGetSpockVersion();
	u32 tachyon = prxSysregGetTachyonVersion();

	char model[64]; memset(model, 0, sizeof(model));
	char tlotr[64]; memset(tlotr, 0, sizeof(tlotr));
	char cxdgg[64]; memset(cxdgg, 0, sizeof(cxdgg));
	char bromv[64]; memset(bromv, 0, sizeof(bromv)); // TODO: proper detection
	char times[64]; memset(times, 0, sizeof(times)); // TODO: proper detection
/*
	switch(tachyon) {
		case 0x00140000:
			sprintf(tlotr, "First");
			sprintf(bromv, "v1 (2004-04-20)");
			sprintf(model, "%s", "PSP-1000 TA-079");

			switch(baryon) {
				case: 0x00010600:
					sprintf(model, "%s%s", model, "v1");
					sprintf(times, "200409230625");
				break;
				case: 0x00020600:
					sprintf(model, "%s%s", model, "v2");
					sprintf(times, "200410071128");
				break;
				case: 0x00030600:
					sprintf(model, "%s%s", model, "v3");
					sprintf(times, "200411290757");
				break;
				default:
					sprintf(model, "%s%s", model, "v?");
					sprintf(times, "YYYYMMDDHHMM");
				break;
			}

		break;
		case 0x00200000:
			sprintf(tlotr, "First");
			sprintf(bromv, "v1 (2004-04-20)");
			sprintf(model, "%s", "PSP-1000 TA-079");

			switch(baryon) {
				case: 0x00030600:
					sprintf(model, "%s%s", model, "v4");
					sprintf(times, "200411290757");
				break;
				case: 0x00040600:
					sprintf(model, "%s%s", model, "v5");
					sprintf(times, "200504040852");
				break;
				default:
					sprintf(model, "%s%s", model, "v?");
					sprintf(times, "YYYYMMDDHHMM");
				break;
			}

		break;
		case 0x00300000:
			sprintf(tlotr, "First");
			sprintf(bromv, "v1 (2004-04-20)");
			sprintf(model, "%s", "PSP-1000 TA-081");
			sprintf(times, "200504040852");

			switch(pommel) {
				case: 0x00000103:
					sprintf(model, "%s%s", model, "v1");
				break;
				case: 0x00000104:
					sprintf(model, "%s%s", model, "v2");
				break;
				case: 0x00000103:
					sprintf(model, "%s%s", model, "v?");
				break;
			}

		break;
		case 0x00400000:
			sprintf(tlotr, "Legolas");
			sprintf(bromv, "v2 (2005-01-04)");
			sprintf(model, "%s", "PSP-1000 TA-08");

			switch(baryon) {
				case: 0x00114000:
					sprintf(tlotr, "%s%s", tlotr, "1"); // Legolas1
					sprintf(model, "%s%s", model, "2"); // TA-082
					sprintf(times, "200509260441");
				break;
				case: 0x00121000:
					sprintf(tlotr, "%s%s", tlotr, "2"); // Legolas2
					sprintf(model, "%s%s", model, "6"); // TA-086
					sprintf(times, "200512200558");
				break;
				default:
					sprintf(tlotr, "%s%s", tlotr, "?"); // Legolas?
					sprintf(model, "%s%s", model, "?"); // TA-08?
					sprintf(times, "YYYYMMDDHHMM");
				break;
			}

		break;
		case 0x00500000:
			sprintf(tlotr, "Frodo");
			sprintf(bromv, "v2 (2005-01-04)");
			sprintf(model, "%s", "PSP-2000 TA-0");

			switch(baryon) {
				case: 0x0022B200:
					sprintf(model, "%s%s", model, "85v1"); // TA-085v1
					sprintf(times, "200704161420");
				break;
				case: 0x00234000:
					sprintf(model, "%s%s", model, "85v2"); // TA-085v2
					sprintf(times, "200710022249");
				break;
				case: 0x00243000:
					sprintf(times, "200711022212");
					switch(pommel) {
						case 0x00000123:
							sprintf(model, "%s%s", model, "88v1/v2"); // TA-088v1/v2
						break;
						case 0x00000132:
							sprintf(model, "%s%s", model, "90v1"); // TA-090v1
						break;
						default:
							sprintf(model, "%s%s", model, "??"); // TA-0??
						break;
					}
				break;
				default:
					sprintf(model, "%s%s", model, "??"); // TA-0??
					sprintf(times, "YYYYMMDDHHMM");
				break;
			}

		break;
		case 0x00600000:
			sprintf(model, "%s", "PSP-");
		break;
		case 0x00720000:
			sprintf(model, "%s", "PSP-N1000");
		break;
		case 0x00810000:
			sprintf(model, "%s", "PSP-3000");
		break;
		case 0x00820000:
			sprintf(model, "%s", "PSP-3000");
		break;
		case 0x00900000:
			sprintf(model, "%s", "PSP-E1000");
		break;
		default:
			sprintf(model, "%s", "PSP-?000");
		break;
	}
*/
	switch(tachyon) {
		case 0x00140000: {
			sprintf(bromv, "2004-04-20");
			sprintf(cxdgg, "CXD2962GG");
			sprintf(model, "PSP-1000 TA-079v%x", baryon >> 16); // v1/2/3
			sprintf(tlotr, "First");
			break;
		}
		case 0x00200000: {
			sprintf(bromv, "2004-04-20");
			sprintf(cxdgg, "CXD2962GG");
			sprintf(model, "PSP-1000 TA-079v%x", (baryon >> 16) + 1); // v4/5
			sprintf(tlotr, "First");
			break;
		}
		case 0x00300000: {
			sprintf(bromv, "2004-04-20");
			sprintf(cxdgg, "CXD2962BGG");
			sprintf(model, "PSP-1000 TA-081v%x", (pommel & 0xF) - 2); // v1/2
			sprintf(tlotr, "First");
			break;
		}
		case 0x00400000: {
			sprintf(bromv, "2005-01-04");
			sprintf(cxdgg, "CXD2967GG");
			sprintf(model, "PSP-1000 TA-08%s", baryon == 0x00114000 ? "2" : baryon == 0x00121000 ? "6" : "?");
			sprintf(tlotr, "Legolas%s", baryon == 0x00114000 ? "1" : baryon == 0x00121000 ? "2" : "?");
			break;
		}

		case 0x00500000: {
			sprintf(bromv, "2005-01-04");
			switch(baryon) {
				case 0x0022B200: {
					sprintf(cxdgg, "CXD2975[B|C]GG");
					sprintf(model, "PSP-2000 TA-085v1");
					sprintf(tlotr, "Frodo");
					break;
				}
				case 0x00234000: {
					sprintf(cxdgg, "CXD2975[B|C]GG");
					sprintf(model, "PSP-2000 TA-085v2");
					sprintf(tlotr, "Frodo");
					break;
				}
				case 0x00243000: {
					sprintf(cxdgg, "CXD2975C1GG");
					switch(pommel) {
						case 0x00000123: {
							sprintf(model, "PSP-2000 TA-088v1/v2"); // TODO: proper detection
							sprintf(tlotr, "Frodo");
							break;
						}
						case 0x00000132: {
							sprintf(model, "PSP-2000 TA-090v1");
							sprintf(tlotr, "Frodo");
							break;
						}
					}
					break;
				}
			}
			break;
		}

		case 0x00600000: {
			sprintf(bromv, "2005-01-04");
			sprintf(cxdgg, "CXD2988GG");
			switch(baryon) {
				case 0x00243000: {
					sprintf(model, "PSP-2000 TA-088v3");
					sprintf(tlotr, "Frodo");
					break;
				}
				case 0x00263100: {
					sprintf(model, "PSP-3000 TA-090v%x", pommel & 0xF); // v2/3
					sprintf(tlotr, "Samwise");
					break;
				}
				case 0x00285000: {
					sprintf(model, "PSP-3000 TA-092");
					sprintf(tlotr, "Samwise");
					break;
				}
			}
			break;
		}
		case 0x00810000: {
			sprintf(bromv, "2007-09-10");
			sprintf(cxdgg, "CXD2993GG");
			sprintf(tlotr, "Samwise VA2");
			switch(baryon) {
				case 0x002C4000: {
					sprintf(model, "PSP-3000 TA-093v%s", pommel == 0x00000141 ? "1" : pommel == 0x00000143 ? "2" : "?");
					break;
				}
				case 0x002E4000: {
					sprintf(model, "PSP-3000 TA-095v1"); // (tachyon >> 16) & 0xF == 1
					break;
				}
				case 0x012E4000: {
					sprintf(model, "PSP-3000 TA-095v3");
					break;
				}
			}
			break;
		}
		case 0x00820000: {
			sprintf(bromv, "2007-09-10");
			sprintf(cxdgg, "CXD2993GG");
			sprintf(tlotr, "Samwise VA2");
			switch(baryon) {
				case 0x002E4000: {
					sprintf(model, "PSP-3000 TA-095v2"); // (tachyon >> 16) & 0xF == 2
					break;
				}
				case 0x012E4000: {
					sprintf(model, "PSP-3000 TA-095v4");
					break;
				}
			}
			break;
		}

		case 0x00720000: {
			sprintf(bromv, "2007-09-10");
			sprintf(cxdgg, "CXD29??GG");
			sprintf(model, "PSP-N1000 TA-091");
			sprintf(tlotr, "Strider");
			break;
		}

		case 0x00900000: {
			sprintf(bromv, "2007-09-10");
			sprintf(cxdgg, "CXD2998GG");
			sprintf(model, "PSP-E1000 TA-096/TA-097");
			sprintf(tlotr, "Bilbo");
			break;
		}

		default: {
			sprintf(bromv, "200?-??-??");
			sprintf(cxdgg, "CXD29??GG");
			sprintf(model, "PSP-???? TA-0??");
			sprintf(tlotr, "???");
			break;
		}
	}

	if ((generation == 4) && (baryon == 0x002E4000)) {
		sprintf(model, "%s (fake 04g/real 09g)", model);
	} else {
		sprintf(model, "%s (%02ig)", model, generation);
	}

	printf(" * %-10s v%x.%x%x (", "Firmware", firmware >> 24, (firmware >> 16) & 0xff, (firmware >> 8) & 0xff);
	printf("0x%08x)\n", firmware);
	printf(" * %-10s 0x%08x [%s]\n", "Tachyon", tachyon, bromv);
	printf(" * %-10s 0x%08x [%s]\n", "Baryon", baryon, "times");
	printf(" * %-10s 0x%08x\n", "Pommel", pommel);
	printf(" * %-10s 0x%c%c%c%c\n", "Kirk", kirk[3], kirk[2], kirk[1], kirk[0]);
	if (generation != 5) {
		printf(" * %-10s 0x%c%c%c%c\n", "Spock", spock[3], spock[2], spock[1], spock[0]);
	}
	printf(" * %-10s 0x%llx\n", "FuseId", fuseid);
	printf(" * %-10s 0x%04x\n", "FuseCfg", fusecfg);
	printf(" * %-10s %02ig\n\n", "Generation", generation);

	printf(" * %s\n", model);
	printf(" * Call me [%s], Gandalf!\n", tlotr);

	sceKernelDelayThread(1*1000*1000);

	char dir[128] = "\0";
	sprintf(dir, "%c%c0:/PICTURE", argv[0][0], argv[0][1]);
	sceIoMkdir(dir, 0777);
	sprintf(dir, "%s/pspIdent", dir);
	sceIoMkdir(dir, 0777);

	int i;
	SceUID fd;
	char file[128] = "\0";
	for (i = 0; i < 999; i++) {
		sprintf(file, "%s/ident%03i.bmp", dir, i);
		fd = sceIoOpen(file, PSP_O_RDONLY, 0777);
		if (fd < 0) {
			savepict(file);
			break;
		} else {
			sceIoClose(fd);
		}
	}
	printf("\n Screenshot was saved to %s!\n", file);

	sceKernelDelayThread(4*1000*1000);
	sceKernelExitGame();

	return 0;
}
