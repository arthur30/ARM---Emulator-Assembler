#ifndef TYPEDEFS
#define TYPEDEFS

typedef struct {
	int x;
	int y;
} vector_t;

typedef struct {
	vector_t origin;
	vector_t size;
} rect_t;

typedef struct {
	float r;
	float g;
	float b;
	float a;
} colour_t;

typedef struct {
	int height;
	int width;
	int bpp;
	int pitch;

	volatile unsigned char *fb;
} screen_t;

#endif
