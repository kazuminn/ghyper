#include "Emulator.h"
#include "ModRM.h"

hinstruction_func_t* hinstructions16[0xffff];

namespace hinstruction16{

/*
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
*/
void push_r32(Emulator *emu, sig_ucontext_t* uc){

	uint8_t * pc = (uint8_t *)uc->uc_mcontext.rip;
	uint8_t reg = *pc & ((1<<3)-1);
	emu->memory[emu->ESP] = emu->GetRegister32(reg);
	uc->uc_mcontext.rip++; 
}

void pop_r32(Emulator *emu, sig_ucontext_t* uc){

	uint8_t * pc = (uint8_t *)uc->uc_mcontext.rip;
	uint8_t reg = *pc & ((1<<3)-1);
	emu->SetRegister32(reg, emu->memory[emu->ESP]);
	uc->uc_mcontext.rip++; 
}

void mov_rm32_imm32(Emulator *emu, sig_ucontext_t* uc){
	uc->uc_mcontext.rip++; 

	ModRM modrm(emu, uc);
	uint8_t * pc = (uint8_t *)uc->uc_mcontext.rip;
	uint32_t imm32 = *pc + (*(pc + 1) << 8) + (*(pc + 1) << 16) + (*(pc + 1) << 24);
	modrm.SetRM32(imm32);		
	uc->uc_mcontext.rip = uc->uc_mcontext.rip + 4;
}

void nop(Emulator *emu, sig_ucontext_t* uc){
	uc->uc_mcontext.rip++; 
}

void mov_al_moffs8(Emulator *emu, sig_ucontext_t* uc){
	uc->uc_mcontext.rip++; 
	uc->uc_mcontext.rip++; 
	uc->uc_mcontext.rip++; 
	uc->uc_mcontext.rip++; 
	uc->uc_mcontext.rip++; 
}

void mov_rm32_r32(Emulator *emu, sig_ucontext_t* uc){	//cout<<"mov2"<<endl;
	uc->uc_mcontext.rip++; 
	
	emu->instr.opecode = *(uint8_t *)uc->uc_mcontext.rip;
	ModRM modrm(emu, uc);
	uint32_t r32 = modrm.GetR32();
	modrm.SetRM32(r32);
}
/*

void ltr_rm16(Emulator *emu, ModRM *modrm){
	uint16_t rm16 = modrm->GetRM16();
	emu->SetTR(rm16);
}

void lgdt_m32(Emulator *emu, ModRM *modrm) {
	uint32_t m48 = modrm->get_m();
	uint16_t limit = emu->GetMemory16(m48);
	uint32_t base = emu->GetMemory32(m48 + 2);

    emu->set_gdtr(base, limit);
}

void lidt_m32(Emulator *emu, ModRM *modrm) {
	uint32_t m48 = modrm->get_m();
	uint16_t limit = emu->GetMemory16(m48);
	uint32_t base = emu->GetMemory32(m48 + 2);

    emu->set_idtr(base, limit);
}

void push_es(Emulator *emu){
	emu->Push32(emu->get_ES());
	emu->EIP++;
}
void pop_es(Emulator *emu){
	emu->set_ES(emu->Pop32());
	emu->EIP++;
}

void push_ss(Emulator *emu){
	emu->Push32(emu->get_SS());
	emu->EIP++;
}

void pop_ss(Emulator *emu){
	emu->set_SS(emu->Pop32());
	emu->EIP++;
}

void push_ds(Emulator *emu){
    emu->Push32(emu->get_DS());
    emu->EIP++;
}

void pop_ds(Emulator *emu){
	emu->set_DS(emu->Pop32());
	emu->EIP++;
}

void add_rm32_r32(Emulator *emu){
	emu->EIP++;
	ModRM modrm(emu);
	uint32_t rm32 = modrm.GetRM32();
	uint32_t r32 = modrm.GetR32();
	modrm.SetRM32(rm32 + r32);
	emu->update_eflags_add(rm32, r32);
}

void add_r32_rm32(Emulator *emu){
    emu->EIP++;
    ModRM modrm(emu);
    uint32_t r32 = modrm.GetR32();
    uint32_t rm32 = modrm.GetRM32();
    modrm.SetR32(rm32 + r32);
	emu->update_eflags_add(r32, rm32);
}

void add_eax_imm32(Emulator *emu){
    uint32_t imm32 = emu->GetSignCode32(1);
    uint32_t eax = emu->EAX;
	emu->EAX = eax + imm32;
	emu->EIP += 5;
	emu->update_eflags_add(eax, imm32);
}

void add_rm32_imm32(Emulator *emu, ModRM *modrm){
	uint32_t rm32 = modrm->GetRM32();
	uint32_t imm32 = (int32_t)emu->GetSignCode32(0);
	emu->EIP += 4;
	modrm->SetRM32(rm32 + imm32);
	emu->update_eflags_add(rm32, imm32);
}

//void add_al_imm8(Emulator *emu){
//	uint8_t	al = emu->AL;
//	uint8_t imm8 = emu->GetSignCode8(1);
//	emu->AL = al + imm8;
//}

void dec_r32(Emulator *emu){
	uint8_t reg = emu->GetSignCode8(0) & ((1<<3)-1);
	uint32_t r32 = emu->reg[reg].reg32;
	emu->reg[reg].reg32 = r32 - 1;
	emu->EIP++;
	emu->update_eflags_sub(r32, 1);
}

void mov_rm8_r8(Emulator *emu){
	emu->EIP++;
	ModRM modrm(emu);
	uint8_t r8 = modrm.GetR8();
	modrm.SetRM8(r8);
}

void mov_r8_imm8(Emulator *emu){
	uint8_t reg = emu->GetCode8(0) - 0xB0;
	emu->reg[reg].reg32 = emu->GetCode8(1);
	emu->EIP += 2;
//	std::cout<<"a";
}

void mov_r32_imm32(Emulator *emu){
	uint8_t reg	= emu->GetCode8(0) - 0xB8;
	uint32_t val = emu->GetCode32(1);
	printf("reg %x \n", val);
	emu->reg[reg].reg32 = val;
	emu->EIP += 5;
}

void mov_rm8_imm8(Emulator *emu){
	emu->EIP++;
	ModRM modrm(emu);
	uint8_t val = emu->GetCode8(0);
	modrm.SetRM8(val);
	emu->EIP++;
}

void mov_r8_rm8(Emulator *emu){//		cout<<"mov_r8_rm8"<<endl;
	emu->EIP++;
	ModRM modrm(emu);
	uint8_t rm8 = modrm.GetRM8();
	modrm.SetR8(rm8);
}

void out_dx_al(Emulator *emu){
	emu->io_out8(emu->DX, emu->AL);
	emu->EIP++;
}

void sti(Emulator *emu){
	emu->set_interrupt(true);
	emu->EIP++;
}

void mov_r32_rm32(Emulator *emu){
	emu->EIP++;
	ModRM modrm(emu);
	uint32_t rm32 = modrm.GetRM32();
	modrm.SetR32(rm32);
}

void inc_r32(Emulator *emu){
	uint8_t reg = emu->GetCode8(0) - 0x40;
	emu->reg[reg].reg32++;
	emu->EIP++;
}

void lea_r32_m32(Emulator *emu){
    emu->EIP++;
	ModRM modrm(emu);
    uint32_t m32 = modrm.get_m();
    modrm.SetR32(m32);
}

void mov_al_moffs8(Emulator *emu) {
	emu->EIP++;
	uint8_t moffs = emu->memory[emu->sgregs[1].base + emu->GetSignCode32(0)];
	emu->AL = moffs;
	emu->EAX = (emu->EAX & ~0xff) | emu->AL;
	emu->EIP += 4;
}


void add_rm32_imm8(Emulator *emu, ModRM *modrm){
	uint32_t rm32 = modrm->GetRM32();
	int32_t imm8 = (int32_t)emu->GetSignCode8(0);
	if(imm8 >= 0x80){
		imm8 -= 0x100;
	}
	emu->EIP++;
	modrm->SetRM32(rm32 + imm8);
	emu->update_eflags_add(rm32, imm8);
}


void short_jump(Emulator *emu){
    uint32_t diff = emu->GetSignCode8(1);
	emu->EIP += diff + 2;
}

void near_jump(Emulator *emu){
	int32_t diff = emu->GetSignCode32(1);
	emu->EIP += (diff + 5);
}

void mov_moffs32_eax(Emulator *emu){
	emu->EIP++;
    uint32_t moffs = emu->GetSignCode32(0);
	emu->SetMemory32(moffs, emu->EAX);
	emu->EIP += 4;
}

void in_al_dx(Emulator *emu){
	uint16_t dx = emu->DX;
	emu->AL = emu->in_io8(dx);
	emu->EAX = (emu->EAX & ~0xff) | emu->AL;
	emu->EIP++;
}

void pushfd(Emulator *emu){
    emu->Push32(emu->get_eflags());
    emu->EIP++;
}

void sub_rm32_imm32(Emulator *emu, ModRM *modrm){
	uint32_t rm32 = modrm->GetRM32();
	uint32_t imm32 = (int32_t)emu->GetSignCode32(0);
	emu->EIP += 4;
	modrm->SetRM32(rm32 - imm32);
	emu->update_eflags_sub(rm32, imm32);
}
void push_imm32(Emulator *emu) {
	uint32_t imm32 = (int32_t)emu->GetSignCode32(1);
	emu->Push32(imm32);
	emu->EIP += 5;
}

void sub_rm32_imm8(Emulator *emu, ModRM *modrm){
    uint32_t rm32 = modrm->GetRM32();
    int32_t imm8 = (int32_t)emu->GetSignCode8(0);
	if(imm8 >= 0x80){
		imm8 -= 0x100;
	}
    emu->EIP++;
    modrm->SetRM32(rm32 - imm8);
	emu->update_eflags_sub(rm32, imm8);
}


void sub_rm32_r32(Emulator *emu){
	emu->EIP++;
	ModRM modrm(emu);
	uint32_t rm32 = modrm.GetRM32();
	uint32_t r32 = modrm.GetR32();
	modrm.SetRM32(rm32 - r32);
	emu->update_eflags_sub(rm32, r32);
}

void sub_r32_rm32(Emulator *emu){
	emu->EIP++;
	ModRM modrm(emu);
	uint32_t r32 = modrm.GetR32();
	uint32_t rm32 = modrm.GetRM32();
	modrm.SetR32(rm32 - r32);
	emu->update_eflags_sub(r32, rm32);
}

void sub_eax_imm32(Emulator *emu) {
    uint32_t imm32 = emu->GetSignCode32(1); uint32_t eax = emu->reg[0].reg32;
	emu->reg[0].reg32 = emu->reg[0].reg32 - imm32;
	emu->EIP += 5;
	emu->update_eflags_sub(eax , imm32);
}

void xor_rm32_r32(Emulator *emu) {
	emu->EIP++;
	ModRM modrm(emu);
	uint32_t rm32 = modrm.GetRM32();
	uint32_t r32 = modrm.GetR32();
	modrm.SetRM32(r32 ^ rm32);
	emu->UpdateXor();
}

void xor_r32_rm32(Emulator *emu) {
	emu->EIP++;
	ModRM modrm(emu);
	uint32_t r32 = modrm.GetR32();
	uint32_t rm32 = modrm.GetRM32();
	modrm.SetR32(r32 ^ rm32);
	emu->UpdateXor();
}

void xor_rm32_imm32(Emulator *emu, ModRM *modrm) {
	uint32_t rm32 = modrm->GetRM32();
	uint32_t imm32 = emu->GetSignCode32(0);
	emu->EIP += 4;
	modrm->SetRM32(imm32 ^ rm32);
	emu->UpdateXor();
}

void xor_rm32_imm8(Emulator *emu, ModRM *modrm) {
	uint32_t rm32 = modrm->GetRM32();
	int32_t imm8 = (int32_t)emu->GetSignCode8(0);
	if(imm8 >= 0x80){
		imm8 -= 0x100;
	}
	emu->EIP++;
	modrm->SetRM32(imm8 ^ rm32);
	emu->UpdateXor();
}

void xor_eax_imm32(Emulator *emu) {
    emu->reg[0].reg32 = emu->reg[0].reg32 ^ emu->GetSignCode32(1);
    emu->EIP += 5;
	emu->UpdateXor();
}

void or_rm32_imm32(Emulator *emu, ModRM *modrm) {
	uint32_t rm32 = modrm->GetRM32();
	uint8_t imm32 = emu->GetSignCode32(0);
	emu->EIP += 4;
	modrm->SetRM32(imm32 | rm32);
	emu->UpdateOr();
}

void or_rm32_imm8(Emulator *emu, ModRM *modrm) {
	uint32_t rm32 = modrm->GetRM32();
	int32_t imm8 = (int32_t)emu->GetSignCode8(0);
	if(imm8 >= 0x80){
		imm8 -= 0x100;
	}
	emu->EIP += 1;
	modrm->SetRM32(imm8 | rm32);
	emu->UpdateOr();
}

void or_rm32_r32(Emulator *emu) {
	emu->EIP++;
	ModRM modrm(emu);
	uint32_t rm32 = modrm.GetRM32();
	uint32_t r32 = modrm.GetR32();
	modrm.SetRM32(r32 | rm32);
	emu->UpdateOr();
}

void or_r32_rm32(Emulator *emu) {
	emu->EIP++;
	ModRM modrm(emu);
	uint32_t r32 = modrm.GetR32();
	uint32_t rm32 = modrm.GetRM32();
	modrm.SetR32(r32 | rm32);
	emu->UpdateOr();
}
void or_eax_imm32(Emulator *emu) {
	emu->reg[0].reg32 = emu->reg[0].reg32 | emu->GetSignCode32(1);
	emu->EIP += 5;
	emu->UpdateOr();
}

void cmp_rm32_imm32(Emulator *emu, ModRM *modrm){
	uint32_t rm32= modrm->GetRM32();
	uint32_t imm32 = emu->GetCode32(0);
	emu->update_eflags_sub(rm32, imm32);
	emu->EIP += 4;
}

void cmp_rm32_imm8(Emulator *emu, ModRM *modrm){
	uint32_t rm32= modrm->GetRM32();
	int32_t imm8 = (int32_t)emu->GetCode8(0);
	if(imm8 >= 0x80){
		imm8 -= 0x100;
	}
	emu->update_eflags_sub(rm32, imm8);
	emu->EIP++;
}

void shl_rm32_imm8(Emulator *emu, ModRM *modrm){
	uint32_t rm32 = modrm->GetRM32();
	uint32_t imm8 = emu->GetCode8(0);
	modrm->SetRM32(rm32<<imm8);
	emu->update_eflags_shl(rm32, imm8);
	emu->EIP++;
}

void shl_rm32_cl(Emulator *emu, ModRM *modrm){
	uint32_t rm32 = modrm->GetRM32();
	uint8_t cl = emu->get_CL();
	modrm->SetRM32(rm32<<cl);
	emu->update_eflags_shl(rm32, cl);
}

void shr_rm32_imm8(Emulator *emu, ModRM *modrm){
	uint32_t rm32 = modrm->GetRM32();
	uint32_t imm8 = emu->GetCode8(0);
	modrm->SetRM32(rm32>>imm8);
	emu->EIP++;
	emu->update_eflags_shr(rm32, imm8);
}

void shr_rm32_cl(Emulator *emu, ModRM *modrm){
	uint32_t rm32 = modrm->GetRM32();
	uint8_t cl = emu->get_CL();
	modrm->SetRM32(rm32>>cl);
	emu->update_eflags_shr(rm32, cl);
}

void sal_rm32_imm8(Emulator *emu, ModRM *modrm){
	uint32_t rm32 = modrm->GetRM32();
	uint32_t imm8 = emu->GetCode8(0);
	modrm->SetRM32(rm32<<imm8);
	//emu->update_eflags_shr(rm32, imm8);
}

void sal_rm32_cl(Emulator *emu, ModRM *modrm){
	uint32_t rm32 = modrm->GetRM32();
	uint8_t cl = emu->get_CL();
	modrm->SetRM32(rm32<<cl);
	//emu->update_eflags_shr(rm32, cl);
}

void sar_rm32_imm8(Emulator *emu, ModRM *modrm){
	int32_t rm32 = modrm->GetRM32();
	int32_t imm8 = emu->GetCode8(0);
	modrm->SetRM32(rm32>>imm8);
	emu->EIP++;
	//emu->update_eflags_shr(rm32, imm8);
}

void sar_rm32_cl(Emulator *emu, ModRM *modrm){
	uint32_t rm32 = modrm->GetRM32();
	uint8_t cl = emu->get_CL();
	modrm->SetRM32(rm32>>cl);
	//emu->update_eflags_shr(rm32, cl);
}

void sar_rm8_imm8(Emulator *emu, ModRM *modrm){
	int8_t rm8_s = modrm->GetRM8();
	uint8_t imm8 = emu->GetSignCode8(0);
	modrm->SetRM8(rm8_s>>imm8);
	emu->EIP += 1;
}

void shr_rm8_imm8(Emulator *emu, ModRM *modrm){
	uint8_t rm8_s = modrm->GetRM8();
	uint8_t imm8 = emu->GetSignCode8(0);
	modrm->SetRM8(rm8_s>>imm8);
	emu->EIP += 1;
}

void and_rm32_imm8(Emulator *emu, ModRM *modrm) {
	uint32_t rm32 = modrm->GetRM32();
	int32_t imm8 = (int32_t)emu->GetSignCode8(0);
	if(imm8 >= 0x80){
		imm8 -= 0x100;
	}
	modrm->SetRM32(rm32&imm8);
	emu->EIP++;
}

void cmp_rm8_imm8(Emulator *emu, ModRM *modrm) {
	uint8_t rm8 = modrm->GetRM8();
	uint8_t imm8 = emu->GetSignCode8(0);
	emu->update_eflags_sub(rm8, imm8);
	emu->EIP++;
}

void movzs_r32_rm8(Emulator *emu){
	emu->EIP++;
	ModRM *modrm = new ModRM(emu);
	uint8_t rm8 = modrm->GetRM8();
	modrm->SetR32(rm8);
}

void idiv_edx_eax_rm32(Emulator *emu, ModRM *modrm){
    int32_t rm32_s = modrm->GetRM32();
    int64_t val_s = emu->EDX;
    val_s <<= 32;
    val_s |= emu->EAX;

    emu->EAX = val_s/rm32_s;
    emu->EDX = val_s%rm32_s;
}

void test_rm8_imm8(Emulator *emu, ModRM *modrm){
	uint8_t	rm8 = modrm->GetRM8();
	uint8_t imm8 = emu->GetCode8(0);
	emu->EIP++;
	emu->update_eflags_and(rm8, imm8);
}

void and_rm32_imm32(Emulator *emu, ModRM *modrm){
	uint32_t rm32 = modrm->GetRM32();
	uint32_t imm32 = emu->GetCode32(0);

	modrm->SetRM32(rm32&imm32);
	emu->update_eflags_add(rm32, imm32);
	emu->EIP += 4;
}

void cmp_al_imm8(Emulator *emu){
	uint8_t	al = emu->AL;
	uint8_t imm8 = emu->GetCode8(1);

	emu->update_eflags_sub(al, imm8);
	emu->EIP += 2;
}

void test_rm32_imm32(Emulator *emu, ModRM *modrm){
	uint32_t rm32 = modrm->GetRM32();
	uint32_t imm32 = emu->GetCode32(0);

	emu->update_eflags_and(rm32, imm32);
	emu->EIP += 4;
}

void code_f6(Emulator *emu){
	emu->EIP++;
	ModRM *modrm = new ModRM(emu);

	switch(emu->instr.opecode){
		case 0: test_rm8_imm8(emu, modrm);  break;
		default:
			cout<<"not implemented: f6 "<<(uint32_t)emu->instr.opecode<<endl;
	}
	delete modrm;
}

void code_f7(Emulator *emu){
	emu->EIP++;
	ModRM *modrm = new ModRM(emu);

	switch(emu->instr.opecode){
		case 0: test_rm32_imm32(emu, modrm);  break;
		case 7: idiv_edx_eax_rm32(emu, modrm);  break;
		default:
			cout<<"not implemented: f7 "<<(uint32_t)emu->instr.opecode<<endl;
	}
	delete modrm;
}

void code_80(Emulator *emu){
	emu->EIP++;
	ModRM *modrm = new ModRM(emu);

	switch(emu->instr.opecode){
		case 7: cmp_rm8_imm8(emu, modrm);  break;
		default:
			cout<<"not implemented: 80 "<<(uint32_t)emu->instr.opecode<<endl;
	}
	delete modrm;
}

void code_81(Emulator *emu){
	emu->EIP++;
	ModRM *modrm = new ModRM(emu);

	switch(emu->instr.opecode){
		case 0: add_rm32_imm32(emu, modrm);  break;
		case 1: or_rm32_imm32(emu, modrm);   break;
		case 4: and_rm32_imm32(emu, modrm);  break;
		case 5: sub_rm32_imm32(emu, modrm);  break;
		case 6: xor_rm32_imm32(emu, modrm);  break;
		case 7: cmp_rm32_imm32(emu, modrm);  break;
		default:
			cout<<"not implemented: 81 "<<(uint32_t)emu->instr.opecode<<endl;
	}
	delete modrm;
}

void code_83(Emulator *emu){
	emu->EIP++;
	ModRM *modrm = new ModRM(emu);
	printf("REG :%x \n", emu->instr.opecode);

	switch(emu->instr.opecode){
		case 0: add_rm32_imm8(emu, modrm); break;
		case 1: or_rm32_imm8(emu, modrm);  break;
		case 4: and_rm32_imm8(emu, modrm);  break;
		case 5: sub_rm32_imm8(emu, modrm); break;
        case 6: xor_rm32_imm8(emu, modrm); break;
		case 7: cmp_rm32_imm8(emu, modrm); break;
		default:
			cout<<"not implemented: 83 "<<(uint32_t)emu->instr.opecode<<endl;
	}
	delete modrm;
}

void code_c0(Emulator *emu){
	emu->EIP++;
	ModRM *modrm = new ModRM(emu);

	switch(emu->instr.opecode){
		case 5: shr_rm8_imm8(emu, modrm); break;
		case 7: sar_rm8_imm8(emu, modrm); break;
		default:
			cout<<"not implemented: c0 "<<(uint32_t)emu->instr.opecode<<endl;
	}
	delete modrm;
}

void code_0f00(Emulator *emu){
	emu->EIP++;
	ModRM *modrm = new ModRM(emu);

	switch(emu->instr.opecode){
		case 3: ltr_rm16(emu, modrm);  break;
		default:
			cout<<"not implemented: 0f00 "<<(uint32_t)emu->instr.opecode<<endl;
	}
	delete modrm;
}

void code_c1(Emulator *emu){
	emu->EIP++;
	ModRM *modrm = new ModRM(emu);
	cout<<"opecode "<<(uint32_t)emu->instr.opecode<<endl;

	switch(emu->instr.opecode){
		case 4: shl_rm32_imm8(emu, modrm); break;
		case 5: shr_rm32_imm8(emu, modrm); break;
		case 6: sal_rm32_imm8(emu, modrm); break;
		case 7: sar_rm32_imm8(emu, modrm); break;
		default:
			cout<<"not implemented: c1 "<<(uint32_t)emu->instr.opecode<<endl;
	}
	delete modrm;
}

void code_d3(Emulator *emu){
	emu->EIP++;
	ModRM *modrm = new ModRM(emu);

	switch(emu->instr.opecode){
		case 4: shl_rm32_cl(emu, modrm); break;
		case 5: shr_rm32_cl(emu, modrm); break;
		case 6: sal_rm32_cl(emu, modrm); break;
		case 7: sar_rm32_cl(emu, modrm); break;
		default:
			cout<<"not implemented: d3 "<<(uint32_t)emu->instr.opecode<<endl;
	}
	delete modrm;
}

void code_0f01(Emulator *emu){
	emu->EIP++;
	ModRM *modrm = new ModRM(emu);

	switch(emu->instr.opecode){
		case 2: lgdt_m32(emu, modrm); break;
		case 3: lidt_m32(emu, modrm); break;
		default:
			cout<<"not implemented: 0f01 "<<(uint32_t)emu->instr.opecode<<endl;
	}
	delete modrm;
}

void push_r32(Emulator *emu){
	uint8_t reg = emu->GetCode8(0) & ((1<<3)-1);
	emu->Push32(emu->GetRegister32(reg));
	emu->EIP++;
}

void pop_r32(Emulator *emu){
	uint8_t reg = emu->GetCode8(0) & ((1<<3)-1);
	uint32_t val = emu->Pop32();
	printf("%x \n", val);
	emu->SetRegister32(reg, val);
	emu->EIP++;
}

void push_imm8(Emulator *emu){
	uint32_t val = emu->GetCode8(1);
	printf("val %x \n", val);
	emu->Push32(val);
	emu->EIP += 2;
}

void inc_rm32(Emulator *emu, ModRM *modrm){
	uint32_t val = modrm->GetRM32();
	modrm->SetRM32(val + 1);
}

void push_rm32(Emulator *emu, ModRM *modrm){
	emu->Push32(modrm->GetRM32());
}

void dec_rm32(Emulator *emu, ModRM *modrm){
	uint32_t val = modrm->GetRM32();
	modrm->SetRM32(val - 1);
	emu->update_eflags_sub(val, 1);
}

void task_switch(Emulator *emu, uint16_t cs) {
    TSS oldTSS, newTSS;
    uint32_t prev = emu->dtregs[TR].selector;
    uint32_t base = emu->dtregs[TR].base_addr;
    emu->read_data(&oldTSS, base, sizeof(TSS));//
    oldTSS.eflags = emu->get_eflags();
    oldTSS.eip = emu->EIP;
    oldTSS.eax = emu->EAX;
    oldTSS.ecx = emu->ECX;
    oldTSS.edx = emu->EDX;
    oldTSS.ebx = emu->EBX;
    oldTSS.esp = emu->ESP;
    oldTSS.ebp = emu->EBP;
    oldTSS.esi = emu->ESI;
    oldTSS.edi = emu->EDI;
    oldTSS.es = emu->sreg[0].sreg;
    oldTSS.cs = emu->sreg[1].sreg;
    oldTSS.ss = emu->sreg[2].sreg;
    oldTSS.ds = emu->sreg[3].sreg;
    oldTSS.fs = emu->sreg[4].sreg;
    oldTSS.gs = emu->sreg[5].sreg;
    emu->write_data(base, &oldTSS, sizeof(TSS));

    emu->SetTR(cs);
    base = emu->dtregs[TR].base_addr;
    emu->read_data(&newTSS, base, sizeof(TSS));//


    newTSS.prev_sel = prev;
    emu->write_data(base, &newTSS, sizeof(TSS));
    emu->set_eflags(newTSS.eflags);
    emu->EIP = newTSS.eip;
    emu->EAX = newTSS.eax;
    emu->ECX = newTSS.ecx;
    emu->EDX = newTSS.edx;
    emu->EBX = newTSS.ebx;
    emu->ESP = newTSS.esp;
    emu->EBP = newTSS.ebp;
    emu->ESI = newTSS.esi;
    emu->EDI = newTSS.edi;
    emu->sreg[0].sreg = newTSS.es;
    emu->sreg[1].sreg = newTSS.cs;
    emu->sreg[2].sreg = newTSS.ss;
    emu->sreg[3].sreg = newTSS.ds;
    emu->sreg[4].sreg = newTSS.fs;
    emu->sreg[5].sreg = newTSS.gs;
    emu->set_ldtr(newTSS.ldtr);
}

void farjump(Emulator *emu, ModRM *modrm){
    uint32_t m48 = modrm->get_m();
    uint32_t eip = emu->GetMemory32(m48);
    uint16_t cs = emu->GetMemory16(m48 + 4);

    cs = cs & 0xfffc;
    if (cs == 0) {
       emu->EIP = 0;
    }

    uint16_t bit = (cs >> 2) & 1;
    if ( bit == 0){

    }else if (bit == 1) {
        emu->EIP = 0;
        //LDT
    }

    uint32_t smtype = emu->memory[emu->dtregs[GDTR].base_addr + cs + 5] & 0x9f;
    switch(smtype) {
        case 0x89:
        case 0x8b:
            task_switch(emu, cs);
            break;
        case 0x98:
        case 0x99:
        case 0x9a:
        case 0x9b:
        case 0x9c:
        case 0x9d:
        case 0x9e:
        case 0x9f:
            emu->EIP = eip;
            emu->sreg[1].sreg = cs;
            break;
    }

}

void iret(Emulator *emu){
    Register eflags;
    uint16_t cs;

    emu->EIP = emu->Pop32();
    cs = emu->Pop32();
    eflags.reg32 = emu->Pop32();
    emu->set_eflags(eflags.reg32);

    if(eflags.NT){
        uint32_t base;
        TSS tss;

        base = emu->dtregs[TR].base_addr;
        emu->read_data(&tss, base, sizeof(TSS));
        task_switch(emu, tss.prev_sel);
    }
    else{
            uint32_t esp;
            uint16_t ss;

            esp = emu->Pop32();
            ss = emu->Pop32();
            emu->ESP = esp;
            emu->sreg[2].sreg = ss;
    }

    emu->sreg[1].sreg = cs;
}


void code_ff(Emulator *emu){
	emu->EIP++;
	ModRM *modrm = new ModRM(emu);
	
	switch(emu->instr.opecode){
	case 0: inc_rm32(emu, modrm); break;
    case 1: dec_rm32(emu, modrm); break;
    case 5: farjump(emu, modrm); break;
	case 6: push_rm32(emu, modrm);break;
	default:
		cout<<"not implemented: 0xFF /"<<(int)emu->instr.opecode<<endl;
	}
	
	delete modrm;
}

void movsx_r32_rm8(Emulator *emu){
	emu->EIP++;
	ModRM modrm(emu);
	int8_t rm8_s = modrm.GetRM8();
	modrm.SetR32(rm8_s);
}

void call_rel32(Emulator *emu){
	int32_t diff = emu->GetSignCode32(1);
	printf("%x \n", emu->EIP);
	emu->Push32(emu->EIP + 5);	//call命令は全体で5バイト
	emu->EIP += (diff + 5);
}

void ret(Emulator *emu){//	cout<<"ret"<<endl;
	emu->EIP = emu->Pop32();
	printf("%x \n", emu->EIP);
}

void leave(Emulator *emu){
//	uint32_t ebp = EBP;
	emu->ESP = emu->EBP;
	emu->EBP = emu->Pop32();
	emu->EIP++;
}

void pushad(Emulator *emu) {
    uint32_t esp;
    esp = emu->ESP;

    emu->Push32(emu->EAX);
	emu->Push32(emu->ECX);
	emu->Push32(emu->EDX);
	emu->Push32(emu->EBX);
	emu->Push32(esp);
	emu->Push32(emu->EBP);
	emu->Push32(emu->ESI);
	emu->Push32(emu->EDI);

	emu->EIP++;
}

void popad(Emulator *emu) {
	uint32_t esp;

	emu->EDI = emu->Pop32();
	emu->ESI = emu->Pop32();
	emu->EBP = emu->Pop32();
	esp = emu->Pop32();
	emu->EBX = emu->Pop32();
	emu->EDX = emu->Pop32();
	emu->ECX = emu->Pop32();
	emu->EAX = emu->Pop32();

	emu->ESP = esp;
	emu->EIP++;
}
void jc(Emulator *emu){
	int diff = emu->IsCarry() ? emu->GetSignCode8(1) : 0;
	emu->EIP += (diff + 2);
}
void jz_rel8(Emulator *emu){
	int diff = emu->IsZero() ? emu->GetSignCode8(1) : 0;
	emu->EIP += (diff + 2);
}
void jz_rel32(Emulator *emu){
	int diff = emu->IsZero() ? emu->GetSignCode32(1) : 0;
	emu->EIP += (diff + 5);
}
void js(Emulator *emu){
	int diff = emu->IsSign() ? emu->GetSignCode8(1) : 0;
	emu->EIP += (diff + 2);
}
void jo(Emulator *emu){
	int diff = emu->IsOverflow() ? emu->GetSignCode8(1) : 0;
	emu->EIP += (diff + 2);
}
void jnc(Emulator *emu){
	int diff = !emu->IsCarry() ? emu->GetSignCode8(1) : 0;
	emu->EIP += (diff + 2);
}
void jnz(Emulator *emu){
	int diff = !emu->IsZero() ? emu->GetSignCode8(1) : 0;
	emu->EIP += (diff + 2);
}
void jns(Emulator *emu){
	int diff = !emu->IsSign() ? emu->GetSignCode8(1) : 0;
	emu->EIP += (diff + 2);
}
void jno(Emulator *emu){
	int diff = !emu->IsOverflow() ? emu->GetSignCode8(1) : 0;
	emu->EIP += (diff + 2);
}
void jbe(Emulator *emu){
	int diff = emu->IsCarry() || emu->IsZero() ? emu->GetSignCode8(1) : 0;
	emu->EIP += (diff + 2);
}

void jnl(Emulator *emu){
	int diff = emu->IsSign() == emu->IsOverflow() ? emu->GetSignCode8(1) : 0;
	emu->EIP += (diff + 2);
}

void jnle(Emulator *emu){
	int diff = (!emu->IsZero() && (emu->IsSign() == emu->IsOverflow())) ? emu->GetSignCode8(1) : 0;
	emu->EIP += (diff + 2);
}

void ja(Emulator *emu){
	int diff = !(emu->IsCarry() || emu->IsZero()) ? emu->GetSignCode8(1) : 0;
	emu->EIP += (diff + 2);
}

void jle_rel32(Emulator *emu){
	int diff = emu->IsZero() || (emu->IsSign() != emu->IsOverflow()) ? emu->GetSignCode32(1) : 0;
	emu->EIP += (diff + 5);
}

void js_rel32(Emulator *emu){
	int diff = emu->IsSign() ? emu->GetSignCode32(1) : 0;
	emu->EIP += (diff + 5);
}

void jnle_rel32(Emulator *emu){
	int diff = (!emu->IsZero() && (emu->IsSign() == emu->IsOverflow())) ? emu->GetSignCode32(1) : 0;
	emu->EIP += (diff + 5);
}

void jnz_rel32(Emulator *emu){
	int diff = !emu->IsZero() ? emu->GetSignCode32(1) : 0;
	emu->EIP += (diff + 5);
}

void jbe_rel32(Emulator *emu){
    int diff = emu->IsCarry() || emu->IsZero() ? emu->GetSignCode32(1) : 0;
    emu->EIP += (diff + 5);
}

void jnl_rel32(Emulator *emu){
	int diff = emu->IsSign() == emu->IsOverflow() ? emu->GetSignCode32(1) : 0;
	emu->EIP += (diff + 5);
}

void jl_rel32(Emulator *emu){
	int diff = emu->IsSign() != emu->IsOverflow() ? emu->GetSignCode32(1) : 0;
	emu->EIP += (diff + 5);
}

void setnz_rm8(Emulator *emu){
    emu->EIP++;
    ModRM *modrm = new ModRM(emu);
	emu->reg[emu->instr.RM].reg32 = !emu->IsZero();
    emu->EIP += 2;
}

void sub_rm8_r8(Emulator *emu){
	emu->EIP++;
	ModRM *modrm = new ModRM(emu);
	uint8_t rm8 = modrm->GetRM8();
	uint8_t r8 = modrm->GetR8();
	modrm->SetRM8(rm8-r8);
	emu->update_eflags_sub(rm8, r8);
}

void cmp_rm8_r8(Emulator *emu){
	emu->EIP++;
	ModRM *modrm = new ModRM(emu);
	uint8_t	rm8 = modrm->GetRM8();
	uint8_t r8 = modrm->GetR8();
	emu->update_eflags_sub(rm8, r8);
}

void cmp_rm32_r32(Emulator *emu){
	emu->EIP++;
	ModRM *modrm = new ModRM(emu);
	uint32_t rm32 = modrm->GetRM32();
	uint32_t r32 = modrm->GetR32();
	emu->update_eflags_sub(rm32, r32);
}

void cmp_r32_rm32(Emulator *emu){
	emu->EIP++;
	ModRM *modrm = new ModRM(emu);
	uint32_t r32 = modrm->GetR32();
	uint32_t rm32= modrm->GetRM32();
	emu->update_eflags_sub(r32, rm32);
}

void cmp_eax_imm32(Emulator *emu){
	uint32_t eax = emu->reg[0].reg32;
	uint32_t imm32 = emu->GetCode32(1);
	emu->update_eflags_sub(eax, imm32);
	emu->EIP += 5;
}

void jl(Emulator *emu){
	int diff = (emu->IsSign() != emu->IsOverflow()) ? emu->GetSignCode8(1) : 0;
	emu->EIP += (diff + 2);
}

void jle(Emulator *emu){
	int diff = (emu->IsZero() || (emu->IsSign() != emu->IsOverflow())) ? emu->GetSignCode8(1) : 0;
	emu->EIP += (diff + 2);
}

void popfd(Emulator *emu){
	emu->set_eflags(emu->Pop32());
	emu->EIP++;
}

void test_eax_imm32(Emulator *emu){
	emu->EIP++;
	emu->update_eflags_and(emu->EAX, emu->GetSignCode32(0));
	emu->EIP += 4;

}

void and_eax_imm32(Emulator *emu){
	emu->EIP++;
	uint32_t imm32 = emu->GetSignCode32(0);
	uint32_t eax = emu->EAX;
	emu->EAX = eax & imm32;
	emu->update_eflags_and(eax,imm32);
	emu->EIP += 4;
}

void test_rm8_r8(Emulator *emu){
    emu->EIP++;
	ModRM modrm(emu);
	uint32_t rm8 = modrm.GetRM8();
	uint32_t r8 = modrm.GetR8();
	emu->update_eflags_and(rm8, r8);
}

void test_rm32_r32(Emulator *emu){
	emu->EIP++;
	ModRM modrm(emu);
	uint32_t rm32 = modrm.GetRM32();
	uint32_t r32 = modrm.GetR32();
	emu->update_eflags_and(rm32, r32);
}

void cli(Emulator *emu){
	emu->set_interrupt(false);
	emu->EIP++;
}

void imul_r32_rm32_imm8(Emulator *emu){
	emu->EIP++;
	ModRM modrm(emu);
	int32_t rm32_s = modrm.GetRM32();
	int32_t imm8 = emu->GetSignCode8(0);
	modrm.SetR32(rm32_s * imm8);
	emu->update_eflags_imul(rm32_s, imm8);
	emu->EIP++;
}

void movsx_r32_rm16(Emulator *emu){
    emu->EIP++;
    ModRM modrm(emu);
	int16_t	rm16_s = modrm.GetRM16();
	modrm.SetR32(rm16_s);
}

void mov_eax_moffs32(Emulator *emu){
	emu->EIP++;
	emu->EAX = emu->GetMemory32(emu->GetSignCode32(0));
	emu->EIP += 4;
}
void imul_r32_rm32_imm32(Emulator *emu){
	emu->EIP++;
	ModRM modrm(emu);
	int32_t rm32_s = modrm.GetRM32();
	int32_t imm32 = emu->GetSignCode32(0);
	modrm.SetR32(rm32_s * imm32);
	emu->update_eflags_imul(rm32_s, imm32);
	emu->EIP += 4;
}

void imul_r32_rm32(Emulator *emu){
    emu->EIP++;
    ModRM modrm(emu);
   	int16_t r32_s = modrm.GetR32();
   	int16_t rm32_s = modrm.GetRM32();
   	modrm.SetR32(r32_s * rm32_s);
   	emu->update_eflags_imul((int32_t)r32_s, (int32_t)rm32_s);
}
void mov_sreg_rm16(Emulator *emu){
    emu->EIP++;
    ModRM modrm(emu);
    uint16_t rm16 = modrm.GetRM16();
    emu->sreg[emu->instr.reg_index].sreg = rm16;
}

void movzs_r32_rm16(Emulator *emu){
    emu->EIP++;
    ModRM modrm(emu);
    uint16_t rm16 = modrm.GetRM16();
    modrm.SetR32(rm16);
}
*/

}

using namespace hinstruction16;

void InitHInstructions16(){
	hinstruction_func_t** func = hinstructions16;

	func[0xc7] = mov_rm32_imm32;
	func[0x89] = mov_rm32_r32;
	func[0xA0] = mov_al_moffs8;
	for (int i =0; i< 8; i++) func[0x50+i] = push_r32;
	for (int i =0; i< 8; i++) func[0x58+i] = pop_r32;
}

