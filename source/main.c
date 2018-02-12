#include <switch.h>
#include "draw.h"

u8* g_framebuf;

int main(int argc, char **argv)
{
    init();
    gfxInitDefault();

    while (appletMainLoop())
    {
        hidScanInput();

        if (hidKeysDown(CONTROLLER_P1_AUTO) & KEY_PLUS)
            break;

        g_framebuf = gfxGetFramebuffer(NULL, NULL);
        draw();

        gfxFlushBuffers();
        gfxSwapBuffers();
        gfxWaitForVsync();
    }

    gfxExit();
    return 0;
}
