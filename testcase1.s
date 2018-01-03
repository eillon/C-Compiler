.data
_prompt: .asciiz "Enter an integer:"
_ret: .asciiz "\n"
.globl main
.text

read:
 li $v0,4
 la $a0,_prompt
 syscall
 li $v0,5
 syscall
 jr $ra

write:
 li $v0,1
 syscall
 li $v0,4
 la $a0,_ret
 syscall
 move $v0,$0
 jr $ra

main:
 li $24, 0
 li $24, 1
 li $24, 0
 move $24,$24
 move $24,$24
label0:
 move $24,$24
 move $24,$24
 blt $24,$24,label1
 j label2
label1:
 move $24,$24
 move $24,$24
 add $24,$24,$24
 move $24,$24
 move $24,$24
 move $24,$24
 move $24,$24
 move $24,$24
 move $24,$24
 move $24,$24
 move $24,$24
 li $24, 1
 add $24,$24,$24
 move $24,$24
 move $24,$24
 j label0
label2:
 li $24, 0
 move $v0,$24
 jr $ra