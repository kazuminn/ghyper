#include <gtest/gtest.h>
#include "../ghyper.h" 


TEST(mov, mov_rm32_imm32) {
    uint8_t asmb[11] = {0xc7, 0x85, 0xe0, 0xfb, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xc3};
    Ghyper *ghv = new Ghyper();
    ghv->hv(asmb);

    delete ghv;
}
