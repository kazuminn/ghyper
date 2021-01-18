#include "Emulator.h"
#include "ModRM.h"

hinstruction_func_t* hinstructions16[0xffff];

namespace hinstruction16{

void push_es(Emulator *emu, sig_ucontext_t* uc){
	uint8_t * pc = (uint8_t *)uc->uc_mcontext.rip;
	emu->stack[0] = __builtin_bswap64(*(pc+ 1)); 
	emu->ESP++;
	printf("%x\n", (uint8_t *)uc->uc_mcontext.rsp);
	uc->uc_mcontext.rip++;
}

}

using namespace hinstruction16;

void InitHInstructions16(){
	hinstruction_func_t** func = hinstructions16;

    func[0x06] = push_es;
}

