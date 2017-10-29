
:make
	@pause
	@nasm "boot(floppy disk).asm" -o "boot(floppy disk).bin"
	@goto make