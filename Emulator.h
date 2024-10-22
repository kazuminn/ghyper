#ifndef EMULATOR_H_
#define EMULATOR_H_

#include <stdio.h>
#include <stdint.h>
#include <unordered_map>
#include <ucontext.h>
#include <signal.h>

#define DEFAULT_BIT_MODE	16//16		//デフォルトの起動時のビット。本来は16。
#define REGISTERS_COUNT		8		//レジスタの本数(16/32bit)

#define VRAM_ADDR		0xa0000

#define LOW			0
#define HIGH			1

//instructions
typedef struct _sig_ucontext {
 	unsigned long     uc_flags;
 	struct ucontext   *uc_link;
 	stack_t           uc_stack;
 	struct sigcontext uc_mcontext;
 	sigset_t          uc_sigmask;
} sig_ucontext_t;

extern const char* registers_name16[];		//16bitレジスタの名前
extern const char* registers_name32[];		//32bitレジスタの名前

enum dtreg_t { GDTR, IDTR, LDTR, TR, DTREGS_COUNT};
//enum sgreg_t { ES, CS, SS, DS, FS, GS, SGREGS_COUNT };

struct SIB {
	uint8_t base :3;
	uint8_t index :3;
	uint8_t scale :2;
};


struct InstrData {
	int prefix;
	uint8_t Mod;
	union {
		uint8_t opecode;
		uint8_t reg_index;
	};
	uint8_t RM;
	uint8_t SIB;
	int SEGMENT;
	union {
		int8_t disp8;
		int16_t disp16;
		uint32_t disp32;
	};

	union {
	    uint8_t _SIB;
		struct SIB sib;
	};
};

struct LDTDesc {
    uint16_t limit_l;
    uint16_t base_l;
    uint8_t base_m;
    uint8_t type : 3;	// 2
    uint8_t D : 1;
    uint8_t S : 1;		// 0
    uint8_t DPL : 2;
    uint8_t P : 1;
    uint8_t limit_h : 4;
    uint8_t AVL : 1;
    uint8_t : 2;
    uint8_t G : 1;
    uint8_t base_h;
};

struct TSSDesc {
    uint16_t limit_l;
    uint16_t base_l;
    uint8_t base_m;
    union {
        struct {
            uint8_t : 1;
            uint8_t B : 1;
        };

        struct {
            uint8_t type : 3;	// 1 or 3
            uint8_t D : 1;		// 0:16bit, 1:32bit
            uint8_t S : 1;		// 0
            uint8_t DPL : 2;
            uint8_t P : 1;
        };
    };
    uint8_t limit_h : 4;
    uint8_t AVL : 1;
    uint8_t : 2;
    uint8_t G : 1;
    uint8_t base_h;
};

struct TSS {
    uint16_t prev_sel;
    uint16_t : 16;
    uint32_t esp0;
    uint16_t ss0;
    uint16_t : 16;
    uint32_t esp1;
    uint16_t ss1;
    uint16_t : 16;
    uint32_t esp2;
    uint16_t ss2;
    uint16_t : 16;
    uint32_t cr3;
    uint32_t eip;
    uint32_t eflags;
    uint32_t eax;
    uint32_t ecx;
    uint32_t edx;
    uint32_t ebx;
    uint32_t esp;
    uint32_t ebp;
    uint32_t esi;
    uint32_t edi;
    uint16_t es;
    uint16_t : 16;
    uint16_t cs;
    uint16_t : 16;
    uint16_t ss;
    uint16_t : 16;
    uint16_t ds;
    uint16_t : 16;
    uint16_t fs;
    uint16_t : 16;
    uint16_t gs;
    uint16_t : 16;
    uint16_t ldtr;
    uint16_t : 16;
    uint16_t T : 1;
    uint16_t : 15;
    uint16_t io_base;
};

#define REGISTERS_COUNT32 8

#define  AL reg[0].low8
#define  CL reg[1].low8
#define  DL reg[2].low8
#define  BL reg[3].low8
#define  AH reg[0].high8
#define  CH reg[1].high8
#define  DH reg[2].high8
#define  BH reg[3].high8

//16bitレジスタに簡単にアクセスするためのdefine
#define  AX	reg[0].reg16
#define  CX	reg[1].reg16
#define  DX	reg[2].reg16
#define  BX	reg[3].reg16
#define  SP	reg[4].reg16
#define  BP	reg[5].reg16
#define  SI	reg[6].reg16
#define  DI	reg[7].reg16

//32bitレジスタに簡単にアクセスするためのdefine
#define EAX	reg[0].reg32
#define ECX	reg[1].reg32
#define EDX	reg[2].reg32
#define EBX	reg[3].reg32
#define ESP	reg[4].reg32
#define EBP	reg[5].reg32
#define ESI	reg[6].reg32
#define EDI	reg[7].reg32

#define ES	sreg[0].sreg;
#define CS	sreg[1].sreg;
#define SS	sreg[2].sreg;
#define DS	sreg[3].sreg;
#define FS	sreg[4].sreg;
#define GS	sreg[5].sreg;

//特殊なレジスタに簡単にアクセスするためのdefine
#define FLAGS	eflags.reg16
#define EFLAGS	eflags.reg32
#define IP	eip.reg16
#define EIP	eip.reg32

//GDT,IDT関連
//GDTR,IDTR用のレジスタ型(48bit)
struct DTRegister {
	uint16_t selector;
	uint16_t table_limit;
	uint32_t base_addr;
};

struct SGRegister {
	uint32_t base;
};


// EFLAGS
class Register {
public:
	struct {
		uint32_t reg32;
		uint16_t reg16;
		uint16_t sreg;
		struct {
			uint8_t low8;
			uint8_t high8;
		};
		struct {
			bool CF : 1;
			bool    : 1;
			bool PF : 1;
			bool    : 1;
			bool AF : 1;
			bool    : 1;
			bool ZF : 1;
			bool SF : 1;
			bool TF : 1;
			bool IF : 1;
			bool DF : 1;
			bool OF : 1;
			bool IOPL1 : 1;
			bool IOPL2 : 1;
			bool NT : 1;
			bool    : 1;
			bool RF : 1;
			bool VM : 1;
			bool AC : 1;
			bool VIF: 1;
			bool VIP: 1;
			bool ID : 1;
		};
	};
};

struct IntGateDesc {
    uint16_t offset_l;
    uint16_t seg_sel;
    uint8_t : 8;
    uint8_t type : 3;	// 6
    uint8_t D : 1;
    uint8_t S : 1;		// 0
    uint8_t DPL : 2;
    uint8_t P : 1;
    uint16_t offset_h;
};


class PortIO {
public:
    virtual void out8(uint16_t, uint8_t) = 0;
    virtual uint8_t in8(uint16_t) = 0;
};
//エミュレータクラス
class Emulator {
private:
	int BitMode;
	long long memory_size;
public:
    std::unordered_map<uint16_t, PortIO*> port_io;
    std::unordered_map<uint16_t, size_t> port_io_map;
    Register eflags;
    __uint64_t eeflags;
    Register sreg[6];
	Register CR[5];		// CR0 ~ CR4 制御レジスタ

	Register eip;
	DTRegister dtregs[DTREGS_COUNT];

	Register reg[REGISTERS_COUNT];
	SGRegister sgregs[6];
 
	uint8_t *memory;
    uint32_t ESP_top;
	uint8_t *stack;
	InstrData instr;
	bool is_16mode;   //true is 16mode, false is 32mode
public:				// member funcs
	Emulator();
	~Emulator();

	void InitRegisters();
    void set_portio(uint16_t addr,size_t len, PortIO *dev);
	int GetBitMode();
	bool IsReal(){	return !IsProtected();	}
	bool IsProtected();
	size_t GetMemSize(){	return memory_size;	}

	void set_interrupt(bool);

	size_t read_data(void *dst, uint32_t src_addr, size_t size);
    size_t write_data(uint32_t dst_addr, void *src, size_t size);

	void LoadBinary(const char* fname, uint32_t addr, int size);	//バイナリファイルを読み込んでメモリの指定番地に指定サイズだけ転送する

    uint16_t get_portio_base(uint16_t base);
	void io_out8(uint16_t,uint8_t);
	uint8_t in_io8(uint16_t addr);

	uint8_t GetCode8(int index);
	int8_t  GetSignCode8(int index);
	uint32_t GetCode32(int index);
	int32_t GetSignCode32(int index);

	int parse_prefix(Emulator *emu,  sig_ucontext_t* uc);

	void evacuateRegister(sig_ucontext_t* uc);
	void returnRegister(sig_ucontext_t* uc);

	uint8_t GetRegister8(int index);
    uint16_t GetRegister16(int index);
	uint32_t GetRegister32(int index);

	void SetRegister8(int index, uint8_t val);
	void SetRegister16(int index, uint16_t val);
	void SetRegister32(int index, uint32_t val);

	void set_dtreg(enum dtreg_t, uint16_t, uint32_t, uint16_t);

	uint32_t GetMemory8(uint32_t addr);
	uint32_t GetMemory16(uint32_t addr);
	uint32_t GetMemory32(uint32_t addr);
	void SetMemory8(uint32_t addr, uint32_t val);
	void SetMemory16(uint32_t addr, uint32_t val);
	void SetMemory32(uint32_t addr, uint32_t val);
	void SetStack32(uint32_t);
	void SetStack16(uint16_t);
	void SetStack8(int, uint32_t);

	void Push16(uint16_t val);
	void Push32(uint32_t val);
	uint32_t Pop32();


	void DumpRegisters(int bit);		//各レジスタの値を標準入出力に書き込む。引数はビットモード。
	void DumpRegisters();
	void DumpMemory(const char *fname, uint32_t addr, uint32_t size);
	void DumpMemory(const char *fname, uint32_t size){	DumpMemory(fname, 0x00, size);	}
	void DumpMemory(const char *fname){	DumpMemory(fname, memory_size);	}
public:
    uint32_t get_eflags() { return eflags.reg32; }
	void set_eflags(uint32_t val) { eflags.reg32 = val; }
	inline int IsAjust() { return eflags.AF; }
	inline int IsCarry() { return eflags.CF; }
	inline int IsParity() { return eflags.PF; }
	inline int IsZero() { return eflags.ZF; }
	inline int IsSign() { return eflags.SF; }
	inline int IsOverflow() { return eflags.OF; }
	inline int IsInterrupt() { return eflags.IF; }
	inline int IsDirection() { return eflags.DF; }
	inline void SetAjust(bool ajust) { eflags.AF = ajust; }
	inline void SetCarry(bool carry) { eflags.CF = carry; }
	inline void SetParity(bool parity) { eflags.PF = parity; }
	inline void SetZero(bool zero) { eflags.ZF = zero; }
	inline void SetSign(bool sign) { eflags.SF = sign; }
	inline void SetOverflow(bool of) { eflags.OF = of; }
	inline void SetInterrupt(bool intr) { eflags.IF = intr; }
	inline void SetDirection(bool dir) { eflags.DF = dir; }

	void set_gdtr(uint32_t base, uint16_t limit);
    void set_idtr(uint32_t base, uint16_t limit);
    void set_ldtr(uint16_t sel);
	inline void UpdateXor(){
		eflags.CF = eflags.OF = 0;
	}

	inline void UpdateOr(){
		eflags.CF = eflags.OF = 0;
	}

	template <class T> uint32_t update_eflags_add(T v1, uint32_t v2);
    template <class T> uint32_t update_eflags_and(T v1, uint32_t v2);
    template <class T> uint32_t update_eflags_shl(T v, uint8_t c);
	template <class T> uint32_t update_eflags_imul(T v, int32_t c);
	template <class T> uint32_t update_eflags_shr(T v, uint8_t c);
    template <class T> uint32_t update_eflags_sub(T v, uint32_t c);

	void set_ES(uint16_t v) { sreg[0].sreg = v;}
	void set_CS(uint16_t v) { sreg[1].sreg = v;}
	void set_SS(uint16_t v) { sreg[2].sreg = v;}
	void set_DS(uint16_t v) { sreg[3].sreg = v;}
	void set_FS(uint16_t v) { sreg[4].sreg = v;}
	void set_GS(uint16_t v) { sreg[5].sreg = v;}

	uint16_t get_ES() {return ES;}
	uint16_t get_CS() {return CS;}
	uint16_t get_SS() {return SS;}
	uint16_t get_DS() {return DS;}
	uint16_t get_FS() {return FS;}
    void SetTR(uint16_t sel);

	void set_AL(uint8_t v) { reg[0].low8 = v;}
	void set_CL(uint8_t v) { reg[1].low8 = v;}
	void set_DL(uint8_t v) { reg[2].low8 = v;}
	void set_BL(uint8_t v) { reg[3].low8 = v;}
	void set_AH(uint8_t v) { reg[0].high8 = v;}
	void set_CH(uint8_t v) { reg[1].high8 = v;}
	void set_DH(uint8_t v) { reg[2].high8 = v;}
	void set_BH(uint8_t v) { reg[3].high8 = v;}

	uint8_t get_AL() {return AL;}
	uint8_t get_CL() {return CL;}
	uint8_t get_DL() {return DL;}
	uint8_t get_BL() {return BL;}
	uint8_t get_AH() {return AH;}
	uint8_t get_CH() {return CH;}
	uint8_t get_DH() {return DH;}
	uint8_t get_BH() {return BH;}
private:
	bool chk_parity(uint8_t v);
};


typedef void instruction_func_t(Emulator*);	//各命令に対応した関数の型
typedef void hinstruction_func_t(Emulator*, sig_ucontext_t*);	//各命令に対応した関数の型

void InitInstructions16(void);			//16bit命令の初期化
void InitHInstructions16(void);			//hypervisor16bit命令の初期化
void InitInstructions32(void);			//32bit

extern instruction_func_t* instructions16[0xffff];	//16bit命令の関数の配列
extern hinstruction_func_t* hinstructions16[0xffff];	//hypervisor16bit命令の関数の配列
extern instruction_func_t* instructions32[0xffff];	//32bit

//eflgas

#endif //EMULATOR_H_
