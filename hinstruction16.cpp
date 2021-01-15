#include "Emulator.h"
#include "ModRM.h"

instruction_func_t* hinstructions16[0xffff];

namespace hinstruction16{

void push_es(Emulator *emu){
}

}

using namespace hinstruction16;

void InitHInstructions16(){
	instruction_func_t** func = hinstructions16;

    func[0x06] = push_es;
}

