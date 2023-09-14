#include <pspsdk.h>
#include <pspkernel.h> // sceKernelUtilsSha1Digest()
#include <pspsyscon.h>
#include <pspsysreg.h>
#include <pspidstorage.h> // sceIdStorageLookup()
#include <pspsysmem_kernel.h> // sceKernelGetModel()

PSP_MODULE_INFO("kernel_prx", 0x1006, 1, 1);
PSP_MAIN_THREAD_ATTR(0);

int module_start(SceSize args, void *argp) {
	return 0;
}

int prxIdStorageLookup(u16 key, u32 offset, void*buf, u32 len) {
	int k1 = pspSdkSetK1(0);
	int ret = sceIdStorageLookup(key, offset, buf, len);
	pspSdkSetK1(k1);
	return ret;
}

int prxIdStorageReadLeaf(u16 key, void*buf) {
	int k1 = pspSdkSetK1(0);
	int ret = sceIdStorageReadLeaf(key, buf);
	pspSdkSetK1(k1);
	return ret;
}

int prxKernelGetModel(void) {
	int k1 = pspSdkSetK1(0);
	int g = sceKernelGetModel();
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
	sceKernelUtilsSha1Digest((u8*)buf, sizeof(buf), (u8*)sha);
	value = (sha[0] ^ sha[3]) + sha[2];
	pspSdkSetK1(k1);
	return value;
}

int prxSysconGetBaryonVersion(int*baryon) {
	int k1 = pspSdkSetK1(0);
	int bv = sceSysconGetBaryonVersion(baryon);
	pspSdkSetK1(k1);
	return bv;
}

// https://uofw.github.io/uofw/group__Syscon.html
int sceSyscon_driver_FB148FB6(int*polestar);
int prxSysconGetPolestarVersion(int*polestar) {
	int k1 = pspSdkSetK1(0);
	int pv = sceSyscon_driver_FB148FB6(polestar);
	pspSdkSetK1(k1);
	return pv;
}

int prxSysconGetPommelVersion(int*pommel) {
	int k1 = pspSdkSetK1(0);
	int pv = sceSysconGetPommelVersion(pommel);
	pspSdkSetK1(k1);
	return pv;
}

// https://github.com/galaxyhaxz/psp-detect
int sceSyscon_driver_7BCC5EAE(char*ts);
int prxSysconGetTimeStamp(char*ts) {
	int k1 = pspSdkSetK1(0);
	int ret = sceSyscon_driver_7BCC5EAE(ts);
	pspSdkSetK1(k1);
	return ret;
}

int sceSysreg_driver_8F4F4E96(void);
int prxSysregGetFuseConfig(void) {
	int k1 = pspSdkSetK1(0);
	int fc = sceSysreg_driver_8F4F4E96();
	pspSdkSetK1(k1);
	return fc;
}

long long sceSysreg_driver_4F46EEDE(void);
long long prxSysregGetFuseId(void) {
	int k1 = pspSdkSetK1(0);
	long long fi = sceSysreg_driver_4F46EEDE();
	pspSdkSetK1(k1);
	return fi;
}

int sceSysregKirkBusClockEnable(void);
int prxSysregGetKirkVersion(void) {
	int k1 = pspSdkSetK1(0);
	sceSysregKirkBusClockEnable();
	sceKernelDelayThread(1000);
	int kv = *(int*)0xBDE00004;
	pspSdkSetK1(k1);
	return kv;
}

int sceSysregAtaBusClockEnable(void);
int prxSysregGetSpockVersion(void) {
	int k1 = pspSdkSetK1(0);
	sceSysregAtaBusClockEnable();
	sceKernelDelayThread(1000);
	int sv = *(int*)0xBDF00004;
	pspSdkSetK1(k1);
	return sv;
}

int prxSysregGetTachyonVersion(void) {
	int k1 = pspSdkSetK1(0);
	int tv = sceSysregGetTachyonVersion();
	pspSdkSetK1(k1);
	return tv;
}

// Draan
int prxTachyonGetTimeStamp(void) {
	int ts;
	asm volatile("cfc0 %0, $17" : "=r" (ts));
	return ts;
}

int module_stop(void) {
	return 0;
}
