cc65 -t nes CowboyGuns.c -o build/CowboyGuns.s --add-source 

ca65 crt0.s -o build/crt0.o
ca65 build/CowboyGuns.s -l build/labels.txt -o build/CowboyGuns.o
ld65 -C nrom_32k_horz.cfg build/crt0.o build/CowboyGuns.o nes.lib -o build/CowboyGuns.nes
CD build
CowboyGuns.NES

pause