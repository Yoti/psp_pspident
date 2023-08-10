	.set noreorder

#include "pspstub.s"

	STUB_START "kernel_prx",0x40090000,0x000A0005
	STUB_FUNC  0x06E85124,prxKernelGetModel
	STUB_FUNC  0xFD5B43C6,prxKernelGetTimeStamp
	STUB_FUNC  0x20C62D28,prxSysconGetBaryonVersion
	STUB_FUNC  0xE743F67D,prxSysconGetPommelVersion
	STUB_FUNC  0x7BCC5EAE,sceSysconGetTimeStamp
	STUB_FUNC  0x2EA693FE,prxSysregGetFuseConfig
	STUB_FUNC  0x06C91BFC,prxSysregGetFuseId
	STUB_FUNC  0xC0F82A17,prxSysregGetKirkVersion
	STUB_FUNC  0x7B5682E6,prxSysregGetSpockVersion
	STUB_FUNC  0x71AD80A7,prxSysregGetTachyonVersion
	STUB_END
