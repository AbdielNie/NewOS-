
	%INCLUDE	"hdd definition.inc"
;This file contains the defInition of disk-related information

	ORG		0X7C00
	JMP		BOOT_ENTRY			;JMP instruction
	DB		"GBLOADER"			;OEM name
	DW		0X0200				;the number of byte per sector
	DB		0X01				;the number of sector per cluster
	DW		0X18DE				;sectors which received
	DB		0X02				;the number of FAT
	DW		0X0000				;max root folder
	DW		0X0000				;sector number in total,use offset 0X20 value if it is equal to 0
	DB		0XF8				;media description
	DW		0X0000				;FAT16
	DW		0X003F				;每磁道的扇区
	DW		0X00FF				;the number of head
	DD		0X00000080			;hide sector
	DD		0X0001E800			;总扇区数(如果超过65535,参见0X13)
	DD		0X00000391			;每个文件分配表的扇区
	DW		0X0000				;Flags
	DW		0X0000				;version value
	DD		0X00000002			;root start cluster
	DW		0X0001				;FSInfo sector
	DW		0X0006				;启动扇区备份
	RESB	12					;保留未使用
	RESB	8					;FAT文件系统类型FAT\FAT12\FAT16
	DW		0X0000				;OS boot by self code
	DB		0X80				;BIOS device code
	DB		0X00				;use for nothing
	DB		0X29				;mark
	DD		0X8CC2493E			;SN
	DB		"VOLUME     "		;volume
	DB		"FAT32   "			;type of FAT file system
	
BOOT_ENTRY:
;Initialization stack
	MOV		AX,CS
	MOV		SS,AX
	MOV		SP,$$-1
;get partition start in LBA
	MOV		AX,MBRSEGMENT
	MOV		ES,AX
	MOV		EDX,[ES:PARTITION0.STRLBA]
	
	ADD		EDX,
	