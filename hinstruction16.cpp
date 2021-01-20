#include "Emulator.h"
#include "ModRM.h"

hinstruction_func_t* hinstructions16[0xffff];

namespace hinstruction16{

void push_es(Emulator *emu, sig_ucontext_t* uc){
	//uint32_t *rsp = (uint32_t *)uc->uc_mcontext.rsp;
		//printf("rsp: %lx\n", *rsp);
	if (emu->ESP_top != (uint32_t )uc->uc_mcontext.rsp){
		emu->ESP_top = (uint32_t )uc->uc_mcontext.rsp;
		emu->ESP = (uint32_t )uc->uc_mcontext.rsp;
	}
	uint32_t * pc = (uint32_t *)uc->uc_mcontext.rip;
	printf("cs : %x\n", (uint32_t )uc->uc_mcontext.cs);
	emu->Push16(emu->sgregs[0].base); 
    emu->ESP = emu->ESP - 2;
	uc->uc_mcontext.rip++;
}

}

using namespace hinstruction16;

void InitHInstructions16(){
	hinstruction_func_t** func = hinstructions16;

    func[0x06] = push_es;
}

