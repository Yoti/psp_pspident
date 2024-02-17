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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../lodepng/lodepng.h"
#include "../kernel_lib/libpspexploit.h"

#define VER_MAJOR 3
#define VER_MINOR 0
#define VER_BUILD " \"Drambuie\""

PSP_MODULE_INFO("pspIdent", 0, VER_MAJOR, VER_MINOR);
PSP_MAIN_THREAD_ATTR(PSP_THREAD_ATTR_USER|PSP_THREAD_ATTR_VFPU);
PSP_HEAP_SIZE_KB(4*1024);

#include "main.h"

extern int firmware;
extern void kmain();

SceCtrlData pad;

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

int main(int argc, char* argv[]) {
	pspDebugScreenInit();
	pspDebugScreenClear();
	printf("\n pspIdent v%i.%i%s by Yoti and friends\n\n", VER_MAJOR, VER_MINOR, VER_BUILD);

	firmware = sceKernelDevkitVersion();

	int res = pspXploitInitKernelExploit();
    if (res == 0){
        res = pspXploitDoKernelExploit();
        if (res == 0){
            pspXploitExecuteKernel(kmain);
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
		}
	}

	sceKernelExitGame();
	return 0;
}

