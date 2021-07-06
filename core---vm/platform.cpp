#include "pxt.h"

#ifdef _MSC_VER
#include <io.h>
#include <time.h>
#else
#include <unistd.h>
#endif
#include <stdio.h>
#include <fcntl.h>

namespace pxt {

static void initRandomSeed() {
#ifdef POSIX
    int seed = 0xC0DA1;
    int fd = open("/dev/urandom", O_RDONLY);
    read(fd, &seed, sizeof(seed));
    close(fd);
#else
    int seed = time(NULL);
#endif
    seedRandom(seed);
}

void sendSerial(const char *data, int len) {
    /*
    if (!serial) {
        serial = new codal::_mbed::Serial(USBTX, NC);
        serial->baud(9600);
    }
    serial->send((uint8_t*)data, len);
    */
}

extern "C" void drawPanic(int code)
{
    // TODO
}


extern "C" void target_init()
{
    initRandomSeed();
}

void updateScreen(Image_ img);

void screen_init() {
    updateScreen(NULL);
}

}

