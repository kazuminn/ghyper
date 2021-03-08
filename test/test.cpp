#include <gtest/gtest.h>
#include "../ghyper.h" 



/*
TEST(push, push_r32) {
    uint8_t asmb[11] = {0x55, 0x58, 0xc3};
    Ghyper *ghv = new Ghyper();
    ghv->hv(asmb);

    delete ghv;
}
*/

TEST(mov, mov_rm32_imm32) {
    uint8_t asmb[15] = {0x89, 0xe2, 0xb8, 0x03, 0x00, 0x00, 0x00, 0x50, 0x8b, 0x0a, 0x58, 0xc3};
    Ghyper *ghv = new Ghyper();
    ghv->hv(asmb);

    delete ghv;
}