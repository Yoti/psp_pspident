#include <pspsdk.h>
#include <pspsyscon.h>
#include <pspsysmem_kernel.h> // sceKernelGetModel()

PSP_MODULE_INFO("kernel_prx", 0x1006, 1, 0);
PSP_MAIN_THREAD_ATTR(0);

int module_start(SceSize args, void *argp) {
	return 0;
}

int prxKernelGetModel(void) {
	int k1 = pspSdkSetK1(0);
	int g = sceKernelGetModel();
	pspSdkSetK1(k1);
	return g;
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

u64 sceSysreg_driver_4F46EEDE(void);
u64 prxSysregGetFuseId(void) {
	int k1 = pspSdkSetK1(0);
	u64 fi = sceSysreg_driver_4F46EEDE();
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

int sceSysregGetTachyonVersion(void);
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
