/*
 *  M68K helper routines
 *
 *  Copyright (c) 2007 CodeSourcery
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, see <http://www.gnu.org/licenses/>.
 */
#include "cpu.h"
#include "exec/helper-proto.h"
#include "exec/cpu_ldst.h"

#if defined(CONFIG_USER_ONLY)

void m68k_cpu_do_interrupt(CPUState *cs)
{
    cs->exception_index = -1;
}

static inline void do_interrupt_m68k_hardirq(CPUM68KState *env)
{
}

#else

extern int semihosting_enabled;

/* Try to fill the TLB and return an exception if error. If retaddr is
   NULL, it means that the function was called in C code (i.e. not
   from generated code or from helper.c) */
void tlb_fill(CPUState *cs, target_ulong addr, int is_write, int mmu_idx,
              uintptr_t retaddr)
{
    int ret;

    ret = m68k_cpu_handle_mmu_fault(cs, addr, is_write, mmu_idx);
    if (unlikely(ret)) {
        if (retaddr) {
            /* now we have a real cpu fault */
            cpu_restore_state(cs, retaddr);
        }
        cpu_loop_exit(cs);
    }
}

static void do_rte(CPUM68KState *env)
{
    uint32_t sp;
    uint32_t fmt;
    int offset = 4;

    sp = env->aregs[7];
    if (m68k_feature(env, M68K_FEATURE_M68000)) {
        fmt = cpu_lduw_kernel(env, sp);
        offset = 2;
    } else {
        fmt = cpu_ldl_kernel(env, sp);
    }
    env->pc = cpu_ldl_kernel(env, sp + offset);
    if (!m68k_feature(env, M68K_FEATURE_M68000)) {
        sp |= (fmt >> 28) & 3;
    }
    env->sr = fmt & 0xffff;
    m68k_switch_sp(env);
    env->aregs[7] = sp + 4 + offset;
}

static void do_interrupt_all(CPUM68KState *env, int is_hw)
{
    CPUState *cs = CPU(m68k_env_get_cpu(env));
    uint32_t sp;
    uint32_t fmt;
    uint32_t retaddr;
    uint32_t vector;

    fmt = 0;
    retaddr = env->pc;

    if (!is_hw) {
        switch (cs->exception_index) {
        case EXCP_RTE:
            /* Return from an exception.  */
            do_rte(env);
            return;
        case EXCP_HALT_INSN:
            if (semihosting_enabled
                    && (env->sr & SR_S) != 0
                    && (env->pc & 3) == 0
                    && cpu_lduw_code(env, env->pc - 4) == 0x4e71
                    && cpu_ldl_code(env, env->pc) == 0x4e7bf000) {
                env->pc += 4;
                do_m68k_semihosting(env, env->dregs[0]);
                return;
            }
            cs->halted = 1;
            cs->exception_index = EXCP_HLT;
            cpu_loop_exit(cs);
            return;
        }
        if (cs->exception_index >= EXCP_TRAP0
            && cs->exception_index <= EXCP_TRAP15) {
            /* Move the PC after the trap instruction.  */
            retaddr += 2;
        }
    }

    vector = cs->exception_index << 2;

    if (cs->exception_index != EXCP_RESET) {
        sp = env->aregs[7];

        fmt |= 0x40000000;
        fmt |= (sp & 3) << 28;
        fmt |= vector << 16;
        fmt |= env->sr;

        env->sr |= SR_S;
        if (is_hw) {
            env->sr = (env->sr & ~SR_I) | (env->pending_level << SR_I_SHIFT);
            env->sr &= ~SR_M;
        }
        m68k_switch_sp(env);

        /* ??? This could cause MMU faults.  */
        if (!m68k_feature(env, M68K_FEATURE_M68000)) {
            sp &= ~3;
        }
        sp -= 4;
        cpu_stl_kernel(env, sp, retaddr);
        if (m68k_feature(env, M68K_FEATURE_M68000)) {
            /* Store only SR for 68000 */
            sp -= 2;
            cpu_stw_kernel(env, sp, fmt);
        } else {
            sp -= 4;
            cpu_stl_kernel(env, sp, fmt);
        }
    } else {
        sp = cpu_ldl_kernel(env, env->vbr + vector - 4);
    }
    env->aregs[7] = sp;
    /* Jump to vector.  */
    env->pc = cpu_ldl_kernel(env, env->vbr + vector);
}

void m68k_cpu_do_interrupt(CPUState *cs)
{
    M68kCPU *cpu = M68K_CPU(cs);
    CPUM68KState *env = &cpu->env;

    do_interrupt_all(env, 0);
}

static inline void do_interrupt_m68k_hardirq(CPUM68KState *env)
{
    do_interrupt_all(env, 1);
}
#endif

bool m68k_cpu_exec_interrupt(CPUState *cs, int interrupt_request)
{
    M68kCPU *cpu = M68K_CPU(cs);
    CPUM68KState *env = &cpu->env;

    if (interrupt_request & CPU_INTERRUPT_HARD
        && ((env->sr & SR_I) >> SR_I_SHIFT) < env->pending_level) {
        /* Real hardware gets the interrupt vector via an IACK cycle
           at this point.  Current emulated hardware doesn't rely on
           this, so we provide/save the vector when the interrupt is
           first signalled.  */
        cs->exception_index = env->pending_vector;
        do_interrupt_m68k_hardirq(env);
        return true;
    }
    return false;
}

static void raise_exception(CPUM68KState *env, int tt, uintptr_t pc)
{
    CPUState *cs = CPU(m68k_env_get_cpu(env));

    if (
        (cpu_lduw_kernel(env, env->pc) != 0x4e73) /* rte */
        ) {
        printf("0x%x val:%x\n", env->pc, cpu_lduw_kernel(env, env->pc));
        qemu_log("0x%x val:%x a0=%x a1=%x d0=%x\n", env->pc, cpu_lduw_kernel(env, env->pc), env->aregs[0], env->aregs[1], env->dregs[0]);
    }

    cs->exception_index = tt;
    if (pc) {
        /* now we have a real cpu fault */
        cpu_restore_state(cs, pc);
    }

    cpu_loop_exit(cs);
}

void HELPER(raise_exception)(CPUM68KState *env, uint32_t tt)
{
    raise_exception(env, tt, GETPC());
}

enum ioParams {
    ioCompletion = 12,
    ioResult     = 16,
    ioVRefNum    = 22,
    ioRefNum     = 24,
    ioBuffer     = 32,
    ioReqCount   = 36,
    ioActCount   = 40,
    ioPosMode    = 44,
    ioPosOffset  = 46
};

enum resultCodes {
    noErr    = -0,
    eofErr   = -39,
    extFSErr = -58,
    fnOpnErr = -38,
    ioErr    = -36,
    paramErr = -50,
    rfNumErr = -51
};

void HELPER(read_disk)(CPUM68KState *env, uint32_t tt)
{
    //CPUState *cs = CPU(m68k_env_get_cpu(env));
    FILE * disk;
    int ReqCount  = cpu_ldl_kernel(env, env->aregs[0] + ioReqCount);
    int ActCount  = 0;
    //int PosOffset = cpu_ldl_kernel(env, env->aregs[0] + ioPosOffset);
    int Buffer    = cpu_ldl_kernel(env, env->aregs[0] + ioBuffer);
    int PosOffset = cpu_ldl_kernel(env, env->aregs[0] + ioPosOffset);
    int Completion = cpu_ldl_kernel(env, env->aregs[0] + ioCompletion);
    int Result = noErr;
    char buffer[1];
    char file_name[] = "2.0 System Disk.dsk";

    env->cc_dest = 0;
    qemu_log("mac_read at 0x%x refnum=%x\n", env->pc, cpu_lduw_kernel(env, env->aregs[0] + ioRefNum));
    printf("pc = %x\n", env->pc);
    printf("a[0] = %x\n", env->aregs[0]);
    printf("ioCompletion = %x\n",  cpu_ldl_kernel(env, env->aregs[0] + ioCompletion));
    printf("ioResult     = %x\n", cpu_lduw_kernel(env, env->aregs[0] + ioResult));
    printf("ioVRefNum    = %x\n", cpu_lduw_kernel(env, env->aregs[0] + ioVRefNum));
    printf("ioRefNum     = %x\n", cpu_lduw_kernel(env, env->aregs[0] + ioRefNum));
    printf("ioBuffer     = %x\n",  cpu_ldl_kernel(env, env->aregs[0] + ioBuffer));
    printf("ioReqCount   = %x\n",  cpu_ldl_kernel(env, env->aregs[0] + ioReqCount));
    printf("ioActCount   = %x\n",  cpu_ldl_kernel(env, env->aregs[0] + ioActCount));
    printf("ioPosMode    = %x\n", cpu_lduw_kernel(env, env->aregs[0] + ioPosMode));
    printf("ioPosOffset  = %x\n",  cpu_ldl_kernel(env, env->aregs[0] + ioPosOffset));

    if (cpu_lduw_kernel(env, env->aregs[0] + ioRefNum) != 0xfffb) {
        printf("dfgdfgdfg\n");
        raise_exception(env, tt, GETPC());
    } else {
        if ((disk = fopen (file_name, "rb")) == NULL) {
            qemu_log("Error open disk file %s\n", file_name);
            Result = fnOpnErr;
        } else {
            fseek(disk , PosOffset, SEEK_SET);
            while (!feof(disk) & (ActCount != ReqCount)) {
                if (!fread(buffer, 1, 1, disk)) {
                    qemu_log("Error read byte from file %s\n", file_name);
                    Result = eofErr;
                } else {
                    cpu_stb_kernel(env, Buffer + ActCount, buffer[0] & 0xff);
                    ActCount++;
                    printf("%02x", buffer[0] & 0xff);
                }
            }
            printf("\n");
            fclose(disk);
        }
        printf("Actually read bytes: %x\n", ActCount);
        cpu_stl_kernel(env, env->aregs[0] + ioPosOffset, ActCount + PosOffset);
        cpu_stl_kernel(env, env->aregs[0] + ioActCount,  ActCount);
        cpu_stl_kernel(env, env->aregs[0] + ioResult,    Result);
        env->dregs[0] = Result;

        if (Completion) {
            //cs->exception_index = EXCP_TRAP0;
            //do_interrupt_all(env, false);
            //env->pc = 0x401116;
            env->pc = Completion;
        } else {
            env->pc = env->pc + 2;
        }

    }
}

/*
void Sony_Return (int iErr, int SaveD0, int Mode) 
{ 
    put_word(ParamBlk + kioResult, iErr); // Update ioResult 
    if (SaveD0 == 0) { // Don't Save D0 
        if ((iErr & 0x8000) == 0x8000) 
            m68k_dreg(regs, 0) = 0xFFFF0000 | iErr; // Mask D0 properly 
        else 
            m68k_dreg(regs, 0) = 0x00000000 | iErr; // Clear D0 properly 
    } 
    // Synchronous, Asynchronous or Immediate 
    if (((get_word(ParamBlk + kioTrap) & 0x0200) == 0x0200) || (Mode == 1)) 
        m68k_setpc(m68k_getpc() + 2); // Immediate Return (RTS) 
    else 
        m68k_setpc(get_long(0x08FC)); // Jump to JIODone (always done) 
}*/

void HELPER(divu)(CPUM68KState *env, uint32_t word)
{
    uint32_t num;
    uint32_t den;
    uint32_t quot;
    uint32_t rem;
    uint32_t flags;

    num = env->div1;
    den = env->div2;
    /* ??? This needs to make sure the throwing location is accurate.  */
    if (den == 0) {
        raise_exception(env, EXCP_DIV0, GETPC());
    }
    quot = num / den;
    rem = num % den;
    flags = 0;
    /* Avoid using a PARAM1 of zero.  This breaks dyngen because it uses
       the address of a symbol, and gcc knows symbols can't have address
       zero.  */
    if (word && quot > 0xffff) {
        /* real 68040 keep Z and N on overflow,
         * whereas documentation says "undefined"
         */
        flags |= CCF_V | (env->cc_dest & (CCF_Z|CCF_N));
    } else {
        if (quot == 0) {
            flags |= CCF_Z;
        } else if ((int16_t)quot < 0) {
            flags |= CCF_N;
        }
    }

    env->div1 = quot;
    env->div2 = rem;
    env->cc_dest = flags;
}

void HELPER(divs)(CPUM68KState *env, uint32_t word)
{
    int32_t num;
    int32_t den;
    int32_t quot;
    int32_t rem;
    int32_t flags;

    num = env->div1;
    den = env->div2;
    if (den == 0) {
        raise_exception(env, EXCP_DIV0, GETPC());
    }
    quot = num / den;
    rem = num % den;
    flags = 0;
    if (word && quot != (int16_t)quot) {
        /* real 68040 keep Z and N on overflow,
         * whereas documentation says "undefined"
         */
        flags |= CCF_V | (env->cc_dest & (CCF_Z|CCF_N));
    } else {
        if (quot == 0) {
            flags |= CCF_Z;
        } else if ((int16_t)quot < 0) {
            flags |= CCF_N;
        }
    }

    env->div1 = quot;
    env->div2 = rem;
    env->cc_dest = flags;
}

void HELPER(divu64)(CPUM68KState *env)
{
    uint32_t num;
    uint32_t den;
    uint64_t quot;
    uint32_t rem;
    uint32_t flags;
    uint64_t quad;

    num = env->div1;
    den = env->div2;
    /* ??? This needs to make sure the throwing location is accurate.  */
    if (den == 0) {
        raise_exception(env, EXCP_DIV0, GETPC());
    }
    quad = num | ((uint64_t)env->quadh << 32);
    quot = quad / den;
    rem = quad % den;
    if (quot > 0xffffffffULL) {
        flags = (env->cc_dest & ~CCF_C) | CCF_V;
    } else {
        flags = 0;
        if (quot == 0) {
            flags |= CCF_Z;
        } else if ((int32_t)quot < 0) {
            flags |= CCF_N;
        }
        env->div1 = quot;
        env->quadh = rem;
    }
    env->cc_dest = flags;
}

void HELPER(divs64)(CPUM68KState *env)
{
    uint32_t num;
    int32_t den;
    int64_t quot;
    int32_t rem;
    int32_t flags;
    int64_t quad;

    num = env->div1;
    den = env->div2;
    if (den == 0) {
        raise_exception(env, EXCP_DIV0, GETPC());
    }
    quad = num | ((int64_t)env->quadh << 32);
    quot = quad / (int64_t)den;
    rem = quad % (int64_t)den;

    if ((quot & 0xffffffff80000000ULL) &&
        (quot & 0xffffffff80000000ULL) != 0xffffffff80000000ULL) {
        flags = (env->cc_dest & ~CCF_C) | CCF_V;
    } else {
        flags = 0;
        if (quot == 0) {
            flags |= CCF_Z;
        } else if ((int32_t)quot < 0) {
            flags |= CCF_N;
        }
        env->div1 = quot;
        env->quadh = rem;
    }
    env->cc_dest = flags;
}

uint32_t HELPER(mulu32_cc)(CPUM68KState *env, uint32_t op1, uint32_t op2)
{
    uint64_t res = (uint32_t)op1 * op2;
    uint32_t flags;

    flags = 0;
    if (res >> 32) {
        flags |= CCF_V;
    }
    if ((uint32_t)res == 0) {
        flags |= CCF_Z;
    }
    if ((int32_t)res < 0) {
        flags |= CCF_N;
    }
    env->cc_dest = flags;

    return res;
}

uint32_t HELPER(muls32_cc)(CPUM68KState *env, uint32_t op1, uint32_t op2)
{
    int64_t res = (int32_t)op1 * (int32_t)op2;
    uint32_t flags;

    flags = 0;
    if (res != (int64_t)(int32_t)res) {
        flags |= CCF_V;
    }
    if ((uint32_t)res == 0) {
        flags |= CCF_Z;
    }
    if ((int32_t)res < 0) {
        flags |= CCF_N;
    }
    env->cc_dest = flags;

    return res;
}

uint32_t HELPER(mulu64)(CPUM68KState *env, uint32_t op1, uint32_t op2)
{
    uint64_t res = (uint64_t)op1 * op2;
    uint32_t flags;

    env->quadh = res >> 32;
    flags = 0;
    if (res == 0) {
        flags |= CCF_Z;
    }
    if ((int64_t)res < 0) {
        flags |= CCF_N;
    }
    env->cc_dest = flags;

    return res;
}

uint32_t HELPER(muls64)(CPUM68KState *env, uint32_t op1, uint32_t op2)
{
    int64_t res = (uint64_t)(int32_t)op1 * (int32_t)op2;
    uint32_t flags;

    env->quadh = res >> 32;
    flags = 0;
    if (res == 0) {
        flags |= CCF_Z;
    }
    if (res < 0) {
        flags |= CCF_N;
    }
    env->cc_dest = flags;

    return res;
}
