// // This file is part of www.nand2tetris.org
// // and the book "The Elements of Computing Systems"
// // by Nisan and Schocken, MIT Press.
// // File name: projects/7/MemoryAccess/BasicTest/BasicTest.vm
// 
// // Executes pop and push commands.
// 
// push constant 10
@10
D=A
@SP
A=M
M=D
@SP
M=M+1
