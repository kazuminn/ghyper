#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <linux/kernel.h>
#include <ucontext.h>
#include <signal.h>
#include <sys/wait.h>
#include <iostream>
#include <cstdint>
#include <cstring>
#include <cstdio>

#include "Emulator.h"
#include "interrupt.h"
#include "device/PIC.h"
#include "GUI.h"
#include "device/Device.h"
#include "device/keyboard.h"
#include "queue"

#define DEBUG

#define QUIET

#define INTERNAL_BOXFILL
//#define TEST_VRAM
#define HARIBOTE_UI

using namespace std;


Emulator	*emu;
PIC	*pic;
keyboard *kb;
Interrupt	*inter;
GUI		*gui;
Display		*disp;


extern "C" void _pc(uintptr_t, int);



int osType = 0;

void debugtrap(int sig_num, siginfo_t * info, void * ucontext){
		printf("x = %d\n", sig_num);
		exit (1);
}

void trap(int sig_num, siginfo_t * info, void * ucontext){
        emu->AX = emu->EAX;
	    emu->AL = emu->EAX;
        emu->AH = emu->EAX;
        emu->DX = emu->EDX;
        emu->DL = emu->EDX;
        emu->DH = emu->EDX;
        emu->CX = emu->ECX;
        emu->CL = emu->ECX;
        emu->CH = emu->ECX;

        //like irq hardware polling
	    pic->chk_irq(emu);

	    //exec INT xx instruction
        inter->exec_interrupt(pic, emu);



		hinstruction_func_t* func;
		sig_ucontext_t* uc = (sig_ucontext_t *) ucontext;
		uint8_t * pc = (uint8_t *)uc->uc_mcontext.rip;
		uint32_t * rsp = (uint32_t *)uc->uc_mcontext.rsp;
		////////////////printf("rsp: %lx\n", *rsp);
		printf("opecode : %x\n", *pc);
		printf("opecode : %x\n", *(pc + 1));
		printf("rip: %p\n", (void *)uc->uc_mcontext.rip);
		printf("emu->memory   : %p\n", emu->memory);

		emu->instr.prefix = emu->parse_prefix(emu, uc);
		
		func = hinstructions16[*pc];


#ifndef QUIET
		cout<<"emu: ";
		cout<<"EIP = "<<hex<<showbase<<emu->EIP<<", ";
		cout<<"Code = "<<(uint32_t)emu->instr.opcode<<endl;
#endif

		emu->evacuateRegister(uc);

		if(func == NULL){
			cout<<"命令("<<showbase<<__builtin_bswap64(*pc)<<")は実装されていません。"<<endl;
			exit(1);
		}else{
		//execute
		func(emu, uc);
		}


		
		if(emu->EIP > emu->GetMemSize()){
			cout<<"out of memory."<<endl;
		}

	emu->returnRegister(uc);
	//emu->DumpRegisters(32);
	//emu->DumpMemory("memdump.bin");

}

int main(int argc, char **argv){

	 
	int opt;

	opterr = 1;

	bool hypervisor = false;
	while ((opt = getopt(argc, argv, "ho:")) != -1){
		switch (opt) {
			case 'o':
				if(*optarg == 'x'){
					osType = 1;
				} else if(*optarg == 'h'){
					osType = 0;
				}
				break;
			case 'h':
				hypervisor = true;
				break;
			default:
				printf("Usage: ./x86 ./path/to/img -o x|h \n");
				break;
		}
	}
	


if(hypervisor) {
        cout << "プロセス生成" << endl;

		emu = new Emulator();
    	pic = new PIC();
    	//kb = new keyboard();

		//init ES
		emu->sgregs[0].base = 0x820; //0 == Es

    	inter = new Interrupt();
		cout<<"emulator created."<<endl;
//home/a/haribote/30_day/haribote7f/haribote/haribote.img
//../xv6-public/xv6.img
    	emu->LoadBinary("/home/a/haribote/harib27f/haribote.img", 0x7c00, 1024 * 1024 * 1024);

        // 16bit ------------------------------------------------------------
		std::memcpy(&emu->memory[0x280000], &emu->memory[0x4390 + 0x7c00], 512 * 1024);

		uint32_t source;
		uint32_t dest;
		uint32_t interval;
		std::memcpy(&source, &emu->memory[0x280014], 4);
		std::memcpy(&dest, &emu->memory[0x28000c], 4 ); source += 0x280000;
		std::memcpy(&interval, &emu->memory[0x280010], 4);

		std::memcpy(&emu->memory[dest], &emu->memory[source], interval);

		emu->EIP = 0x28001b;
		printf("dest : %x\n", dest);
		emu->ESP = dest;


		emu->memory[0xff2] = 8;
		emu->SetMemory16(0xff4, 320);
    	emu->SetMemory16(0xff6, 200);
		emu->SetMemory32(0xff8, 0xa0000);
        // end 16bit -------------------------------------------------------------


		printf("emu->memory : %p \n", emu->memory);

		struct sigaction sigact;
		int fill = sigfillset(&sigact.sa_mask);
		if(fill < 0){
			return 1; 
		}
		 
		static char handler_stack[0xffff];
        stack_t ss ; 
        ss.ss_flags = 0;
        ss.ss_size = 0xffff;
        ss.ss_sp = handler_stack;
        sigaltstack(&ss, 0);

		//sigprocmask(SIG_BLOCK, &sigact.sa_mask, NULL);
		memset(&sigact, 0, sizeof(sigact));
		sigact.sa_sigaction = trap;
		sigact.sa_flags = SA_RESTART | SA_SIGINFO | SA_NODEFER | SA_ONSTACK;
		int rc = sigaction(SIGILL, &sigact, (struct sigaction *)NULL);
		if(rc < 0){
			return 1; 
		}
		int sc = sigaction(SIGSEGV, &sigact, (struct sigaction *)NULL);
		if(sc < 0){
			return 1; 
		}
		_pc((uintptr_t)emu->memory + emu->EIP, 0x7c00);

		delete emu;
		delete pic;
		delete inter;

}
}
