int prxIdStorageLookup(u16 key, u32 offset, void*buf, u32 len);
int prxIdStorageReadLeaf(u16 key, void*buf);
int prxKernelGetModel(void);
int prxNandGetScramble(void);
int prxSysconGetBaryonVersion(int*baryon);
int prxSysconGetPolestarVersion(int*polestar);
int prxSysconGetPommelVersion(int*pommel);
int prxSysconGetTimeStamp(char*ts);
int prxSysregGetFuseConfig(void);
long long prxSysregGetFuseId(void);
int prxSysregGetKirkVersion(void);
int prxSysregGetSpockVersion(void);
int prxSysregGetTachyonVersion(void);
unsigned int prxTachyonGetTimeStamp(void);
