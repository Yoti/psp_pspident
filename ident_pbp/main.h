#define printf pspDebugScreenPrintf

#define color(x) pspDebugScreenSetTextColor(x)
#define RED 0xff0000ff
#define BLUE 0xffff0000
#define GREEN 0xff00ff00
#define WHITE 0xffffffff
#define YELLOW 0xff00ffff
#define ORANGE 0xff007fff

int ModelRegion[16] = {0, 0, 0, 0, 1, 4, 5, 3, 10, 2, 6, 7, 8, 9, 0, 0};

typedef struct __attribute__((packed)) tagBITMAPFILEHEADER {
	short	bfType;
	int		bfSize;
	short	bfReserved1;
	short	bfReserved2;
	int		bfOffBits;
} BITMAPFILEHEADER;
typedef struct __attribute__((packed)) tagBITMAPINFOHEADER {
	int		biSize;
	int		biWidth;
	int		biHeight;
	short	biPlanes;
	short	biBitCount;
	int		biCompression;
	int		biSizeImage;
	int		biXPelsPerMeter;
	int		biYPelsPerMeter;
	int		biClrUsed;
	int		biClrImportant;
} BITMAPINFOHEADER;

