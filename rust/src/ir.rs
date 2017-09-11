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
use ast;

use std::collections::HashMap;
use std::fmt;

type Symbol = String;

#[derive(Debug)]
pub enum Value {
    Integer(String),
    Decimal(String)
}

impl fmt::Display for Value {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        match self {
            &Value::Integer(ref s) => write!(f, "I({})", s),
            &Value::Decimal(ref s) => write!(f, "D({})", s)
        }
    }
}

#[derive(Debug)]
pub enum SymVal {
    Symbol(Symbol),
    Value(Value)
}

impl fmt::Display for SymVal {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        match self {
            &SymVal::Symbol(ref s) => write!(f, "{}", s),
            &SymVal::Value(ref v) => write!(f, "{}", v),
        }
    }
}

#[derive(Debug)]
pub enum Instruction {
    Add(Symbol, SymVal, SymVal),
    Sub(Symbol, SymVal, SymVal),
    Mul(Symbol, SymVal, SymVal),
    Div(Symbol, SymVal, SymVal),
    Move(Symbol, SymVal)
}

impl fmt::Display for Instruction {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        match self {
            &Instruction::Add(ref s, ref sv1, ref sv2) => write!(f, "Add {}, {}, {}", s, sv1, sv2),
            &Instruction::Sub(ref s, ref sv1, ref sv2) => write!(f, "Sub {}, {}, {}", s, sv1, sv2),
            &Instruction::Mul(ref s, ref sv1, ref sv2) => write!(f, "Mul {}, {}, {}", s, sv1, sv2),
            &Instruction::Div(ref s, ref sv1, ref sv2) => write!(f, "Div {}, {}, {}", s, sv1, sv2),
            &Instruction::Move(ref s, ref sv) => write!(f, "Move {}, {}", s, sv)
        }
    }
}

type SymbolCounterTable = HashMap<Symbol,u32>;
type InstructionList = Vec<Instruction>;

pub struct IR {
    pub instructions : InstructionList
}

fn gen_symbol(base_name: &str, sym_counters: &mut SymbolCounterTable) -> Symbol {
    let mut counter = sym_counters.entry(base_name.to_string()).or_insert(0);
    *counter += 1;
    return base_name.to_string() + "$" + counter.to_string().as_str();
}

fn gen_operand(node: &ast::ExprNode, instructions: &mut InstructionList, sym_counters: &mut SymbolCounterTable) -> SymVal {
    use ast::ExprNode;
    use ast::Literal;
    match node {
        &ExprNode::Literal(Literal::Integer(ref val), _) => SymVal::Value(Value::Integer(val.clone())),
        &ExprNode::Literal(Literal::Decimal(ref val), _) => SymVal::Value(Value::Decimal(val.clone())),
        &ExprNode::Identifier(ref s) => SymVal::Symbol(gen_symbol(&s.snippit, sym_counters)),
        op @ &ExprNode::BinaryOperator(_, _, _, _) => {
            let d = gen_symbol("", sym_counters);
            let mut insts = insts_for_expr(&op, &d, sym_counters);
            instructions.append(&mut insts);
            SymVal::Symbol(d)
        }
    }
}

fn insts_for_expr(expr: &ast::ExprNode, dest: &str, sym_counters: &mut SymbolCounterTable) -> InstructionList {
    use ast::ExprNode;
    use ast::BinaryOperator;

    let mut instructions: InstructionList = Vec::new();
    match expr {
        &ExprNode::BinaryOperator(ref op, ref l, ref r, _) => {
            let left = gen_operand(l.as_ref(), &mut instructions, sym_counters);
            let right = gen_operand(r.as_ref(), &mut instructions, sym_counters);
            let d = dest.to_string().clone();
            let inst = match op {
                &BinaryOperator::Add => Instruction::Add(d, left, right),
                &BinaryOperator::Sub => Instruction::Sub(d, left, right),
                &BinaryOperator::Mul => Instruction::Mul(d, left, right),
                &BinaryOperator::Div => Instruction::Div(d, left, right)
            };
            instructions.push(inst);
        },
        _ => panic!("Expected to generate an instruction for an Expression node")
    };

    return instructions;
}

fn gen_instructions(ast_root: &ast::ASTNode) -> IR {
    use ast::ASTNode;

    let mut instructions: InstructionList = Vec::new();
    let mut sym_counters: HashMap<String,u32> = HashMap::new();

    let program = match ast_root { &ASTNode::Proc(_, ref p) => p, _ => panic!("Expected AST root to be Proc node")};

    for node in program {
        let expr = match node { &ASTNode::Expression(ref e) => e, _ => panic!("Can only generate IR for Expression Nodes") };
        let d = gen_symbol("", &mut sym_counters);
        let mut insts = insts_for_expr(expr, &d, &mut sym_counters);
        instructions.append(&mut insts);
    }

    instructions.push(Instruction::Move(String::from("rax"), SymVal::Value(Value::Integer(String::from("0")))));
    let ir = IR {instructions: instructions};
    return ir;
}

pub fn gen_ir(ast_root: &ast::ASTNode) -> IR {
    let ir = gen_instructions(ast_root);
    return ir;
}