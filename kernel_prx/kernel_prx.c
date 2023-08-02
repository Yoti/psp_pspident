#include <pspsdk.h>
#include <pspkernel.h>
#include <pspsysmem_kernel.h>

PSP_MODULE_INFO("kernel_prx", 0x1006, 1, 0);
PSP_MAIN_THREAD_ATTR(0);

int module_start(SceSize args, void *argp) {
	return 0;
}

u32 prxKernelGetModel(void) {
	int k1 = pspSdkSetK1(0);
	u32 g = sceKernelGetModel();
	pspSdkSetK1(k1);
	return g;
}

u32 sceSyscon_driver_7EC5A957(u32*baryon);
u32 prxSysconGetBaryonVersion(u32*baryon) {
	int k1 = pspSdkSetK1(0);
	u32 bv = sceSyscon_driver_7EC5A957(baryon);
	pspSdkSetK1(k1);
	return bv;
}

u32 sceSyscon_driver_E7E87741(u32*pommel);
u32 prxSysconGetPommelVersion(u32*pommel) {
	int k1 = pspSdkSetK1(0);
	u32 pv = sceSyscon_driver_E7E87741(pommel);
	pspSdkSetK1(k1);
	return pv;
}

// https://github.com/galaxyhaxz/psp-detect
s32 sceSyscon_driver_7BCC5EAE(char*ts);
s32 sceSysconGetTimeStamp(char*ts) {
	int k1 = pspSdkSetK1(0);
	s32 ret = sceSyscon_driver_7BCC5EAE(ts);
	pspSdkSetK1(k1);
	return ret;
}

u32 sceSysreg_driver_8F4F4E96(void);
u32 prxSysregGetFuseConfig(void) {
	int k1 = pspSdkSetK1(0);
	u32 fc = sceSysreg_driver_8F4F4E96();
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
u32 prxSysregGetKirkVersion(void) {
	int k1 = pspSdkSetK1(0);
	sceSysregKirkBusClockEnable();
	sceKernelDelayThread(1000);
	u32 kv = *(u32*)0xBDE00004;
	pspSdkSetK1(k1);
	return kv;
}

int sceSysregAtaBusClockEnable(void);
u32 prxSysregGetSpockVersion(void) {
	int k1 = pspSdkSetK1(0);
	sceSysregAtaBusClockEnable();
	sceKernelDelayThread(1000);
	u32 sv = *(u32*)0xBDF00004;
	pspSdkSetK1(k1);
	return sv;
}

u32 sceSysreg_driver_E2A5D1EE(void);
u32 prxSysregGetTachyonVersion(void) {
	int k1 = pspSdkSetK1(0);
	u32 tv = sceSysreg_driver_E2A5D1EE();
	pspSdkSetK1(k1);
	return tv;
}

int module_stop(void) {
	return 0;
}
