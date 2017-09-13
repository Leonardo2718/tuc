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

use std::fmt;

type Symbol = String;

#[derive(Clone,Debug)]
pub enum Opcode {
    ConstInt32(i32),
    ConstFloat64(f64),
    Get(Symbol),
    Set(Symbol, Box<Opcode>),
    Add(Box<Opcode>, Box<Opcode>),
    Sub(Box<Opcode>, Box<Opcode>),
    Mul(Box<Opcode>, Box<Opcode>),
    Div(Box<Opcode>, Box<Opcode>),
}

impl fmt::Display for Opcode {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        write!(f, "{:?}", *self)
    }
}

pub type OpcodeList = Vec<Box<Opcode>>;

pub type BlockId = u32;

#[derive(Clone,Debug)]
pub struct Block {
    pub id: BlockId,
    pub opcodes: OpcodeList,
}

pub type BlockList = Vec<Block>;

pub type IR = BlockList;

pub struct Function {
    pub name: String,
    pub ir: IR,
}

type FunctionList = Vec<Function>;

fn gen_opcodes_from_expr(expr: &ast::ExprNode) -> Box<Opcode> {
    use ast::ExprNode;
    use ast::Literal;
    use ast::BinaryOperator;
    return match expr {
        &ExprNode::Literal(ref l, _) => {
            match l {
                &Literal::Integer(ref s) => Box::new(Opcode::ConstInt32(s.parse::<i32>().expect("Literal is not a valid Integer"))),
                &Literal::Decimal(ref s) => Box::new(Opcode::ConstFloat64(s.parse::<f64>().expect("Literal is not a decimal")))
            }
        },
        &ExprNode::BinaryOperator(ref op, ref l,ref r, _) => {
            let ln = match l.as_ref() {
                &ExprNode::Identifier(ref snippit) => Box::new(Opcode::Get(snippit.snippit.clone())),
                _ => gen_opcodes_from_expr(l.as_ref())
            };
            let rn = match r.as_ref() {
                &ExprNode::Identifier(ref snippit) => Box::new(Opcode::Get(snippit.snippit.clone())),
                _ => gen_opcodes_from_expr(l.as_ref())
            };
            match op {
                &BinaryOperator::Add => Box::new(Opcode::Add(ln, rn)),
                &BinaryOperator::Sub => Box::new(Opcode::Sub(ln, rn)),
                &BinaryOperator::Mul => Box::new(Opcode::Mul(ln, rn)),
                &BinaryOperator::Div => Box::new(Opcode::Div(ln, rn)),
            }
        }
        _ => panic!("Unhandled ExprNode")
    };
}

fn gen_opcodes(ast: &ast::ASTNode) -> Box<Opcode> {
    use ast::ASTNode;
    match ast {
        &ASTNode::Expression(ref expr) => return gen_opcodes_from_expr(expr),
        _ => panic!("Unexpected AST node.")
    }
}

fn gen_ir(ast_nodes: &Vec<ast::ASTNode>) -> IR {
    let mut id: BlockId = 0;
    let mut block: Block = Block{id: id, opcodes: Vec::new()};
    let mut blocks: BlockList = Vec::new();

    for ref node in ast_nodes {
        block.opcodes.push(gen_opcodes(node));
    }

    blocks.push(block);
    return blocks;
}

pub fn evaluate_ast(ast: &ast::ASTNode) -> FunctionList {
    use ast::ASTNode;
    let mut functions = FunctionList::new();

    match ast {
        &ASTNode::Proc(ref name, ref nodes) => functions.push(Function{name: name.to_string(), ir: gen_ir(nodes)}),
        _ => panic!("Can only evaluate AST for functions")
    }

    return functions;
}

fn print_blocks(blocks: &BlockList) {
    for ref block in blocks {
        println!("Block {}", block.id);
        for ref opcode in block.opcodes.clone() {
            println!("  {}", opcode);
        }
    }
}

pub fn print_ir(function: &Function) {
    println!("Function '{}':", function.name);
    print_blocks(&function.ir);
}
