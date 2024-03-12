#include <pspge.h>
#include <psprtc.h>
#include <pspdisplay.h>
#include <stdio.h> // sprintf()
#include <stdlib.h> // malloc()
#include "../lodepng/lodepng.h"
#include "../kernel_lib/libpspexploit.h"

#define VER_MAJOR 3
#define VER_MINOR 2
#define VER_BUILD " \"Drambuie\""

PSP_MODULE_INFO("pspIdent", 0, VER_MAJOR, VER_MINOR);
PSP_MAIN_THREAD_ATTR(PSP_THREAD_ATTR_USER|PSP_THREAD_ATTR_VFPU);
PSP_HEAP_SIZE_KB(4*1024);

#include "main.h"

extern int firmware;
extern void kmain();

SceCtrlData pad;

u32*vramaddr(int x, int y) {
	u32*vram;

	vram = (void*)(0x40000000 | (u32)sceGeEdramGetAddr());
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

	// revert colors back
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

	u64 tick;
	sceRtcGetCurrentTick(&tick);
	srand(tick);
	if (rand() % 100 == 33)
		printf("\n pspIdent v%i.%i%s by Yoti and F.R.I.E.N.D.S.\n\n", VER_MAJOR, VER_MINOR, VER_BUILD);
	else
		printf("\n pspIdent v%i.%i%s by Yoti and friends\n\n", VER_MAJOR, VER_MINOR, VER_BUILD);

	firmware = sceKernelDevkitVersion();
	color(ORANGE); printf(" *"); color(WHITE);
	printf(" %-10s %x.%x%x (0x%08x)\n", "Firmware", firmware >> 24,
			(firmware >> 16) & 0xff, (firmware >> 8) & 0xff, firmware);

	if ((firmware >> 24) >= 5)
		version_txt();

	int ret;
	ret = pspXploitInitKernelExploit();
	if (ret == 0) {
		ret = pspXploitDoKernelExploit();
		if (ret == 0) {
			pspXploitExecuteKernel(kmain); // kernel.c

			printf(" Press X to save screenshot or O to quit the program\n");
			sceCtrlSetSamplingCycle(0);
			sceCtrlSetSamplingMode(PSP_CTRL_MODE_ANALOG);
			for(;;) {
				sceCtrlReadBufferPositive(&pad, 1);
				if ((pad.Buttons & PSP_CTRL_CROSS) == PSP_CTRL_CROSS) {
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
				} else if ((pad.Buttons & PSP_CTRL_CIRCLE) == PSP_CTRL_CIRCLE) {
					break;
				}
				sceDisplayWaitVblank();
			}
		} else {
			color(YELLOW);
			printf("\n Error: pspXploitDoKernelExploit = 0x%08x\n", ret);
			printf(" The program will automatically quit after 8 seconds...\n");
			color(WHITE);
			sceKernelDelayThread(8*1000*1000);
		}
	} else {
		color(YELLOW);
		printf("\n Error: pspXploitInitKernelExploit = 0x%08x\n", ret);
		printf(" The program will automatically quit after 8 seconds...\n");
		color(WHITE);
		sceKernelDelayThread(8*1000*1000);
	}

	sceKernelExitGame();
	return 0;
}
