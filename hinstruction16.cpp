#include "Emulator.h"
#include "ModRM.h"

hinstruction_func_t* hinstructions16[0xffff];

namespace hinstruction16{

void push_es(Emulator *emu, sig_ucontext_t* uc){
	uc->uc_mcontext.rip++;
}

}

using namespace hinstruction16;

void InitHInstructions16(){
	hinstruction_func_t** func = hinstructions16;

    func[0x06] = push_es;
}

