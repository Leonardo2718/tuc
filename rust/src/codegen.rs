/*
Project: TUC
Author: Leonardo Banderali

Description:
    TUC is a simple, experimental compiler designed for learning and experimenting.
    It is not intended to have any useful purpose other than being a way to learn
    how compilers work.

Copyright (C) 2017 Leonardo Banderali

License:

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.

*/

use debug;
use ir;
use std::fmt;

macro_rules! traceln {
    ($($arg:tt)+) => { debug::trace_if("codegen", &(format!($($arg)+) + "\n")); }
}

pub type Symbol = String;

#[derive(Eq,PartialEq,Copy,Clone,Debug)]
pub enum PlatformRegister {
    rax,
    rcx,
    rdx,
    rbx,
    rsp,
    rbp,
    rsi,
    rdi,
    r4,
    r5,
    r6,
    r7,
    r8,
    r9,
    r10,
    r11,
    r12,
    r13,
    r14,
    r15,
}

#[derive(Eq,PartialEq,Copy,Clone,Debug)]
pub enum Register {
    Real(PlatformRegister),
    Virtual(u32)
}

impl fmt::Display for Register {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        match self {
            &Register::Real(ref reg)   => write!(f, "{:?}", reg),
            &Register::Virtual(ref id) => write!(f, "%{}", id)
        }

    }
}

struct VirtualRegisterProvider {
    index: u32
}

impl VirtualRegisterProvider {
    fn new() -> VirtualRegisterProvider {
        return VirtualRegisterProvider{index:0};
    }

    fn get(&mut self) -> Register {
        self.index += 1;
        return Register::Virtual(self.index);
    }

    fn reset(&mut self) {
        self.index = 0;
    }
}

#[derive(Eq,PartialEq,Clone,Debug)]
pub enum InstructionArgument {
    Register(Register),
    Int32Value(i32),
}

impl fmt::Display for InstructionArgument {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        match self {
            &InstructionArgument::Register(ref reg) => write!(f, "{}", reg),
            &InstructionArgument::Int32Value(ref i) => write!(f, "{}", i),
        }
    }
}

#[derive(Eq,PartialEq,Clone,Debug)]
pub enum Instruction {
    LABEL(String),
    mov(Register, InstructionArgument),
    add(Register, InstructionArgument),
    sub(Register, InstructionArgument),
    imul(Register, InstructionArgument),
    div(Register),
    push(Register),
    pop(Register),
}

impl fmt::Display for Instruction {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        match self {
            &Instruction::mov(ref a, ref b) => write!(f, "mov {}, {}", a, b),
            &Instruction::add(ref a, ref b) => write!(f, "add {}, {}", a, b),
            &Instruction::sub(ref a, ref b) => write!(f, "sub {}, {}", a, b),
            &Instruction::imul(ref a, ref b) => write!(f, "imul {}, {}", a, b),
            &Instruction::div(ref r) => write!(f, "div {}", r),
            ref i => panic!("Don't know how to print instruction {:?}", i)
        }
    }
}

type InstructionList = Vec<Instruction>;
struct InstructionSequence {
    dest: Register,
    instructions: InstructionList,
}

fn select_instructions(opcode: &ir::Opcode, vrp: &mut VirtualRegisterProvider) -> InstructionSequence {
    use ir::Opcode;
    //let dest = vrp.get();
    let mut instructions: InstructionList = Vec::new();
    let dest = match opcode {
        &Opcode::ConstInt32(ref i) => {
            let d = vrp.get();
            instructions.push(Instruction::mov(d, InstructionArgument::Int32Value(*i)));
            d
        },
        &Opcode::ConstFloat64(_) => panic!("Floating point constants are not currently handled."),
        &Opcode::Add(ref l, ref r) => {
            let mut left = select_instructions(l, vrp);
            let mut right = select_instructions(r, vrp);
            instructions.append(&mut left.instructions);
            instructions.append(&mut right.instructions);
            instructions.push(Instruction::add(left.dest, InstructionArgument::Register(right.dest)));
            left.dest
        },
        &Opcode::Sub(ref l, ref r) => {
            let mut left = select_instructions(l, vrp);
            let mut right = select_instructions(r, vrp);
            instructions.append(&mut left.instructions);
            instructions.append(&mut right.instructions);
            instructions.push(Instruction::sub(left.dest, InstructionArgument::Register(right.dest)));
            left.dest
        },
        &Opcode::Mul(ref l, ref r) => {
            let mut left = select_instructions(l, vrp);
            let mut right = select_instructions(r, vrp);
            instructions.append(&mut left.instructions);
            instructions.append(&mut right.instructions);
            instructions.push(Instruction::imul(left.dest, InstructionArgument::Register(right.dest)));
            left.dest
        },
        &Opcode::Div(ref l, ref r) => {
            let mut left = select_instructions(l, vrp);
            let mut right = select_instructions(r, vrp);
            instructions.append(&mut left.instructions);
            instructions.append(&mut right.instructions);
            instructions.push(Instruction::mov(Register::Real(PlatformRegister::rax), InstructionArgument::Register(left.dest)));
            instructions.push(Instruction::div(right.dest));
            let d = vrp.get();
            instructions.push(Instruction::mov(d, InstructionArgument::Register(Register::Real(PlatformRegister::rax))));
            d
        },
        ref op => panic!("Cannot generate code for unhandled opcode {}", op),
    };
    return InstructionSequence{dest:dest, instructions:instructions};
}

fn gen_from_block(block: &ir::Block) -> InstructionList {
    let mut instructions: InstructionList = Vec::new();
    //let mut op_stack = Vec::new();

    traceln!("generating code for block {}", block.id);
    for ref op in &block.opcodes {
        let mut vrp = VirtualRegisterProvider::new();
        let ref mut insts = select_instructions(op,&mut vrp).instructions;
        instructions.append(insts);
    }

    return instructions;
}

pub fn gen_instructions(function: &ir::Function) -> InstructionList {
    let mut instructions: InstructionList = Vec::new();

    traceln!("Generating code for function {}", function.name);
    instructions.push(Instruction::LABEL(function.name.clone()));
    for ref block in &function.ir {
        let mut insts = gen_from_block(block);
        instructions.append(&mut insts);
    }

    return instructions;
}

pub fn print_instructions(instructions: &InstructionList) {
    for i in instructions {
        match i {
            &Instruction::LABEL(ref s) => println!("{}:", s),
            ref inst => println!("    {}", inst),
        }
    }
}
