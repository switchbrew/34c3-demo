#include <switch.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>

#include "draw.h"
#include "fast_math.h"

extern u8* g_framebuf;

typedef union {
    uint32_t abgr;
    struct {
        uint8_t r,g,b,a;
    };
} color_t;

static inline color_t MakeColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
    color_t clr;
    clr.r = r;
    clr.g = g;
    clr.b = b;
    clr.a = a;
    return clr;
}

static inline void DrawPixel(uint32_t x, uint32_t y, color_t clr)
{
    if (x >= 1280 || y >= 720)
        return;

    u32 off = (y * 1280 + x)*4;
    g_framebuf[off] = clr.r; off++;
    g_framebuf[off] = clr.g; off++;
    g_framebuf[off] = clr.b; off++;
    g_framebuf[off] = 0xff;
}

typedef struct {
    int x;
    int y;
} point_t;

typedef struct {
    point_t center;
    int r;
} circle_t;

static circle_t c1;
static circle_t c2;
static circle_t c3;


static const char three[] = {
    0, 1, 1, 0,
    0, 0, 0, 1,
    0, 1, 1, 1,
    0, 0, 0, 1,
    0, 1, 1, 0,
};
static const char four[] = {
    0, 1, 0, 1,
    0, 1, 0, 1,
    0, 1, 1, 1,
    0, 0, 0, 1,
    0, 0, 0, 1,

};
static const char cee[] = {
    0, 0, 1, 1,
    0, 1, 0, 0,
    0, 1, 0, 0,
    0, 1, 0, 0,
    0, 0, 1, 1,
};

static inline int dist_sq(point_t a, point_t b)
{
    int dx = a.x - b.x;
    int dy = a.y - b.y;

    return dx * dx + dy * dy;
}

static inline void rotate(point_t* p, point_t center, float deg)
{
    float x = p->x - center.x;
    float y = p->y - center.y;

    float d = deg * M_TAU / 360;

    float nx, ny;
    float cosd = fast_cosf(d);
    float sind = fast_sinf(d);
    nx = cosd * x - sind * y;
    ny = sind * x + cosd * y;

    p->x = nx + center.x;
    p->y = ny + center.y;
}

static float sq(float x) {
    return x*x;
}

void draw()
{
    int x;
    int y;

    static int time=0;
    time++;
    float sine_x = 48.0 * fast_sinf(time * M_TAU / 60.0);

    float Mlimit =  1.0 / powf(30000, 2);

    for (x = 0; x < 1280; x+=2)
    {
        for (y = 0; y < 720; y+=2)
        {
            point_t p;
            p.x = x;
            p.y = y;

            float M =
                (1.0 / sq(dist_sq(c1.center, p))
                 + 1.0 / sq(dist_sq(c2.center, p))
                 + 1.0 / sq(dist_sq(c3.center, p))) / 3;

            color_t clr = MakeColor(0, 0, 0, 0xff);

            if (M >= Mlimit) {
                clr.r = fmin(255.0 * sq(dist_sq(c1.center, p)) / powf(30000, 2), 255);
                clr.g = fmin(255.0 * sq(dist_sq(c2.center, p)) / powf(30000, 2), 255);
                clr.b = fmin(255.0 * sq(dist_sq(c3.center, p)) / powf(30000, 2), 255);
            }
            else {
                float rx = x + sine_x;

                if ((rx >= 128 && (rx < (1280-128))) &&
                    (y >= 168 && (y < (720-168-64))))
                {
                    int tx = rx - 128;
                    int ty = y - 168;

                    int chr_index = tx / 256;
                    const char *chr;
                    switch (chr_index) {
                    case 0:
                        chr = three;
                        clr = MakeColor(66, 66, 66, 0xff);
                        break;
                    case 1:
                        chr = four;
                        clr = MakeColor(66, 66, 66, 0xff);
                        break;
                    case 2:
                        chr = cee;
                        clr = MakeColor(66, 66, 66, 0xff);
                        break;
                    case 3:
                        chr = three;
                        clr = MakeColor(66, 66, 66, 0xff);
                        break;
                    }

                    tx = (tx % 256) / 64;
                    ty = ty / 64;

                    if (!chr[(ty%6)*4 + (tx%4)]) {
                        clr = MakeColor(0, 0, 0, 0xff);
                    }
                }
                else {
                    clr = MakeColor(0, 0, 0, 0xff);
                }
            }

            DrawPixel(x,   y,   clr);
            DrawPixel(x,   y+1, clr);
            DrawPixel(x+1, y,   clr);
            DrawPixel(x+1, y+1, clr);
        }
    }

    static point_t r1 = { .x = 600, .y = 400 };
    static point_t r2 = { .x = 300, .y = 300 };
    static point_t r3 = { .x = 800, .y = 300 };
    rotate(&c1.center, r1, 1);
    rotate(&c2.center, r2, 2);
    rotate(&c3.center, r3, 3);
}

void init()
{
    c1.center.x = 500;
    c1.center.y = 300;

    c2.center.x = 800;
    c2.center.y = 300;

    c3.center.x = 600;
    c3.center.y = 400;
}
