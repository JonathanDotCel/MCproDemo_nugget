
@echo off

del mcprotest.exe
del mcprotest.elf
del mcprotest.o
del mcprotest.sym
del mcprotest.map
del *.temp

REM Simplify the .mk a bit lol
cp cyblib\ELF\MCRDPRO.ASM MCRDPRO.s
cp cyblib\ELF\REGS.ASM REGS.ASM

call dockermake -f buildme.mk all


