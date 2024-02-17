#include <pspsdk.h>
#include <pspkernel.h>
#include <pspsyscon.h>
#include <pspsysreg.h>
#include <pspidstorage.h>
#include <pspsysmem_kernel.h>
#include <pspctrl.h>
#include <pspdisplay.h>
#include <pspge.h>
#include <psprtc.h>
#include <pspumd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../lodepng/lodepng.h"
#include "../kernel_lib/libpspexploit.h"

#include "main.h"

int firmware;
char shippedfw[5];
int baryon; 
unsigned int bromver;
int polestar;
int pommel;
int tachyon;
char times[64]; 
int fusecfg;
long long fuseid;
int generation;
char kirk[4];
int scramble;
char spock[4];
char model[64];
unsigned char idsbtmac[7];
char idswfreg[2];
char c2dreg[2];
char region[2];
char c2dqaf[2];
char qaflag[2];
char tlotr[64]; 
int flag = 0;

static char outtxt[0x12]; 
typedef struct 
{
	unsigned char peripheral_device_type;
	unsigned char removable;
	unsigned char standard_ver;
	unsigned char atapi_response;
	unsigned int additional;
	char vendor_id[8];
	char product_id[16];
	char product_rev[4];
	char sony_spec[0x14];
}ATAPI_INQURIY;

ATAPI_INQURIY ai;
u8 buf2[0x38];
u8 param[4] = { 0, 0, 0x38, 0 };


static KernelFunctions _ktbl; KernelFunctions* k_tbl = &_ktbl;
int (*_sceSysregGetTachyonVersion)(void) = NULL;
int (*_sceSysconGetBaryonVersion)(int*) = NULL;
int (*_sceSysconGetPommelVersion)(int*) = NULL;
int (*_sceSysregGetFuseConfig)(void) = NULL;
long long (*_sceSysregGetFuseId)(void) = NULL;
int (*_sceSysconGetPolestarVersion)(int*) = NULL;
int (*_sceSysconGetTimeStamp)(int*) = NULL;
int (*_sceSysregAtaBusClockEnable)(void) = NULL;
int (*_sceSysregKirkBusClockEnable)(void) = NULL;
int (*_sceKernelGetModel)(void) = NULL;
int (*_sceIdStorageLookup)(int, int, void*, int) = NULL;
int (*_sceKernelDelayThread)(int) = NULL;
int (*_sceKernelExitDeleteThread)(int) = NULL;
int (*_sceRtcGetCurrentTick)(void*) = NULL;
int (*_sceKernelUtilsSha1Digest)(u8*, int, u8*) = NULL;
void* (*_sceUmdManGetUmdDrive)(int) = NULL;
int (*_sceUmdExecInquiryCmd)(void*, int*, int*) = NULL;

u64 tick;

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

	fd = k_tbl->KernelIOOpen("flash0:/vsh/etc/version.txt", PSP_O_RDONLY, 0777);
	if (fd >= 0) {
		size = k_tbl->KernelIORead(fd, version, sizeof(version));
		k_tbl->KernelIOClose(fd);
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


int prxKernelGetModel(void) {
	int k1 = pspSdkSetK1(0);
	int g = _sceKernelGetModel();
	pspSdkSetK1(k1);
	return g;
}

// raing3
int prxNandGetScramble(void) {
	int k1 = pspSdkSetK1(0);
	u32 value;
	u32 buf[4];
	u32 sha[5];
	buf[0] = *(vu32*)(0xBC100090); // volatile?..
	buf[1] = *(vu32*)(0xBC100094);
	buf[2] = *(vu32*)(0xBC100090)<<1;
	buf[3] = 0xD41D8CD9;
	_sceKernelUtilsSha1Digest((u8*)buf, sizeof(buf), (u8*)sha);
	value = (sha[0] ^ sha[3]) + sha[2];
	pspSdkSetK1(k1);
	return value;
}

int prxSysregGetKirkVersion(void) {
	int k1 = pspSdkSetK1(0);
	_sceSysregKirkBusClockEnable();
	_sceKernelDelayThread(1000);
	int kv = *(int*)0xBDE00004;
	pspSdkSetK1(k1);
	return kv;
}

int prxSysregGetSpockVersion(void) {
	int k1 = pspSdkSetK1(0);
	_sceSysregAtaBusClockEnable();
	_sceKernelDelayThread(1000);
	int sv = *(int*)0xBDF00004;
	pspSdkSetK1(k1);
	return sv;
}

int prxSysregGetTachyonVersion(void) {
	int k1 = pspSdkSetK1(0);
	int sv = _sceSysregGetTachyonVersion();
	pspSdkSetK1(k1);
	return sv;
}


// Draan, GalaXyHaXz, Yoti
unsigned int prxTachyonGetTimeStamp(void) {
	unsigned int ts;
	asm volatile("cfc0 %0, $17" : "=r" (ts));
	if (ts & 0x80000000)
		ts -= 0x80000000;
	return ts;
}

int kthread(){

	memset(shippedfw, 0, sizeof(shippedfw));
	_sceIdStorageLookup(0x51, 0, shippedfw, 4);

	_sceSysconGetBaryonVersion(&baryon);
	bromver = prxTachyonGetTimeStamp();
	_sceSysconGetPolestarVersion(&polestar);
	_sceSysconGetPommelVersion(&pommel);
	tachyon = prxSysregGetTachyonVersion();
	memset(times, 0, sizeof(times));
	_sceSysconGetTimeStamp(times);

	fusecfg = _sceSysregGetFuseConfig();
	fuseid = _sceSysregGetFuseId();
	generation = prxKernelGetModel() + 1;
	*(int*)kirk = prxSysregGetKirkVersion();
	scramble = prxNandGetScramble();
	*(int*)spock = prxSysregGetSpockVersion();

	memset(model, 0, sizeof(model));
	memset(idsbtmac, 0, sizeof(idsbtmac));
	_sceIdStorageLookup(0x0050, 0x41, &idsbtmac, 6);
	memset(idswfreg, 0, sizeof(idswfreg));
	_sceIdStorageLookup(0x0045, 0x00, &idswfreg, 1);
	if ((int)idswfreg[0] < 0)
		idswfreg[0] = '\1';
	else if ((int)idswfreg[0] > ((sizeof(WiFiRegion) / sizeof(WiFiRegion[0])) - 1))
		idswfreg[0] = '\1';
	if ((int)idswfreg[0] == 1)
		flag = 1;
	memset(c2dreg, 0, sizeof(c2dreg));
	_sceIdStorageLookup(0x0100, 0x3D, &c2dreg, 1);
	memset(region, 0, sizeof(region));
	_sceIdStorageLookup(0x0100, 0xF5, &region, 1);
	memset(c2dqaf, 0, sizeof(c2dqaf));
	_sceIdStorageLookup(0x0100, 0x40, &c2dqaf, 1);
	memset(qaflag, 0, sizeof(qaflag));
	_sceIdStorageLookup(0x0100, 0xF8, &qaflag, 1);
	memset(tlotr, 0, sizeof(tlotr));

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
			switch(baryon) {
				case 0x002C4000:
					strcpy(tlotr, "Samwise VA2");
					sprintf(model, "PSP-30%02i TA-09", ModelRegion[(int)region[0]]);
					switch(pommel) {
						case 0x00000141:
							strcat(model, "3v1"); // TA-093v1
						break;
						case 0x00000143:
							strcat(model, "3v2"); // TA-093v2
						break;
						default:
							flag = 1;
							strcat(model, "?"); // TA-093v?
						break;
					}
				break;
				case 0x002E4000:
					strcpy(tlotr, "Samwise VA2");
					sprintf(model, "PSP-30%02i TA-09", ModelRegion[(int)region[0]]);
					strcat(model, "5v1"); // TA-095v1 [09g]
				break;
				case 0x012E4000:
					strcpy(tlotr, "Samwise VA2");
					sprintf(model, "PSP-30%02i TA-09", ModelRegion[(int)region[0]]);
					strcat(model, "5v3"); // TA-095v3 [07g]
				break;
				case 0x00323100:
					strcpy(tlotr, "Strider2");
					sprintf(model, "PSP-N10%02i TA-09", ModelRegion[(int)region[0]]);
					strcat(model, "4v1"); // TA-094v1 [06g]
				break;
				case 0x00324000:
					strcpy(tlotr, "Strider2");
					sprintf(model, "PSP-N10%02i TA-09", ModelRegion[(int)region[0]]);
					strcat(model, "4v2"); // TA-094v2 [06g]
				break;
				default:
					flag = 1;
					strcpy(tlotr, "Unknown");
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
			_sceRtcGetCurrentTick(&tick);
			srand(tick);
			printf("%s", wiki[rand() % (sizeof(wiki) / sizeof(wiki[0]))]);
			_sceKernelDelayThread(8*1000*1000);
			_sceKernelExitDeleteThread(0);
			return 0;
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
	color(GREEN); printf(" *"); color(WHITE);
	if(generation == 5 || generation == 6)
		printf(" %-10s %c\n", "Spock", 0x01);
	else	
		printf(" %-10s 0x%c%c%c%c\n", "Spock", spock[3], spock[2], spock[1], spock[0]);
	
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
	else {
        int foundUMD = _sceUmdExecInquiryCmd(_sceUmdManGetUmdDrive(0), param, buf2);
		memset(outtxt, 0, sizeof(outtxt));
		memcpy(&ai, buf2, sizeof(ATAPI_INQURIY));
		strncpy(outtxt, ai.sony_spec, 6);
		strcat(outtxt, " ");
		strncat(outtxt, &ai.sony_spec[6], 11);
		if (foundUMD>=0)
			printf(" UMD drive FW: %s\n", outtxt);
		else
			printf(" UMD drive FW: ?????\n");
    }
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

	_sceKernelDelayThread(1*1000*1000);

	_sceKernelExitDeleteThread(0);
	return 0;
}

u32 findGetModel(){
	u32 addr;
	for (addr = 0x88000000; addr<0x88400000; addr+=4){
		u32 data = _lw(addr);
		if (data == 0x3C03BC10){
			return pspXploitFindFirstJALReverse(addr);
		}
	}
	return 0;
}

u32 findKirkBusClockEnable(char* modname){
	u32 addr = pspXploitFindTextAddrByName(modname);
	u32 topaddr = addr+(128*1024);
	int count = 1;
	for (;addr<topaddr; addr+=4){
		if (_lw(addr)==0x24040080 && _lw(addr+8)==0x24050001){
			if (count) count--;
			else return addr;
		}
	}
	return 0;
}

void kmain(){
	int k1 = pspSdkSetK1(0);
	int userlevel = pspXploitSetUserLevel(8);


	// Setup all the functions
	pspXploitRepairKernel();
	pspXploitScanKernelFunctions(k_tbl);
	

	char* sysreg_mod = (pspXploitFindTextAddrByName("sceLowIO_Driver") == NULL)? "sceSYSREG_Driver" : "sceLowIO_Driver";

	_sceSysregGetTachyonVersion = pspXploitFindFunction(sysreg_mod, "sceSysreg_driver", 0xE2A5D1EE);
	_sceSysconGetBaryonVersion = pspXploitFindFunction("sceSYSCON_Driver", "sceSyscon_driver", 0x7EC5A957);
	_sceSysconGetPolestarVersion = pspXploitFindFunction("sceSYSCON_Driver", "sceSyscon_driver", 0xFB148FB6);
	_sceIdStorageLookup = pspXploitFindFunction("sceIdStorage_Service", "sceIdStorage_driver", 0x6FE062D1);
	_sceSysconGetPommelVersion = pspXploitFindFunction("sceSYSCON_Driver", "sceSyscon_driver", 0xE7E87741);
	_sceSysconGetTimeStamp = pspXploitFindFunction("sceSYSCON_Driver", "sceSyscon_driver", 0x7BCC5EAE);
	_sceSysregGetFuseConfig = pspXploitFindFunction(sysreg_mod, "sceSysreg_driver", 0x8F4F4E96);
	_sceSysregGetFuseId = pspXploitFindFunction(sysreg_mod, "sceSysreg_driver", 0x4F46EEDE);
	_sceSysregAtaBusClockEnable = pspXploitFindFunction(sysreg_mod, "sceSysreg_driver", 0x16909002);
	_sceSysregKirkBusClockEnable = findKirkBusClockEnable(sysreg_mod); // pspXploitFindFunction(sysreg_mod, "sceSysreg_driver", /*0x4F46EEDE*/ 0xBBC721EA);
	_sceKernelGetModel = findGetModel(); //pspXploitFindFunction("sceSystemMemoryManager", "SysMemForKernel", /*0x6373995D*/ 0x07C586A1);

	_sceKernelDelayThread = pspXploitFindFunction("sceThreadManager", "ThreadManForUser", 0xCEADEB47);
	_sceKernelExitDeleteThread = pspXploitFindFunction("sceThreadManager", "ThreadManForUser", 0x809CE29B);
	_sceRtcGetCurrentTick = pspXploitFindFunction("sceRTC_Service", "sceRtc", 0x3F7AD767);
	_sceKernelUtilsSha1Digest = pspXploitFindFunction("sceSystemMemoryManager", "UtilsForUser", 0x840259F1);

	// Get UMD Firmware
	_sceUmdManGetUmdDrive = pspXploitFindFunction("sceUmdMan_driver", "sceUmdMan_driver", 0x47E2B6D8);
	_sceUmdExecInquiryCmd = pspXploitFindFunction("sceUmdMan_driver", "sceUmdMan_driver", 0x1B19A313);

	// init kernel thread
	SceUID kthreadID = k_tbl->KernelCreateThread("ident_kthread", (void*)KERNELIFY(&kthread), 1, 0x20000, PSP_THREAD_ATTR_VFPU, NULL);
    if (kthreadID >= 0){
        // start thread and wait for it to end
        k_tbl->KernelStartThread(kthreadID, 0, NULL);
        k_tbl->waitThreadEnd(kthreadID, NULL);
    }
	pspXploitSetUserLevel(userlevel);
	pspSdkSetK1(k1);
}
