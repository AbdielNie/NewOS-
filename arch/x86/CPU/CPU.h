
#ifndef CPU_H_
#define CPU_H_

#define		FAMILY ((CPUID_model & 0xF00) >> 8)
#define		EXTENDED_FAMILY ((CPUID_model & 0xFF00000) >> 20)

#define		MODEL ((CPUID_model & 0xF0) >> 4)
#define		EXTENDED_MODEL ((CPUID_model & 0xF0000) >> 16)


/**CPU family*/
unsigned char CPU_family_06H[] = "Pentium 4";
unsigned char CPU_family_0FH[] = "P6";
unsigned char CPU_family_AMD[] = "Unknown AMD Processer";
unsigned char CPU_family_AMDK5[] = "AMD K5";
unsigned char CPU_family_VIA[] = "VIA Processer";
unsigned char CPU_family_SiS[] = "SiS Processer";
unsigned char CPU_family_VMware[] = "Vmware Virtual Processor";
unsigned char CPU_family_vpc[] = "Hyper-v Virtual Processor";
unsigned char CPU_family_Unknown[] = "Unknown";

/**CPU model*/
unsigned char CPU_model_06_2AH[] = "Intel Core i7 i5 i3 2xxx";
unsigned char CPU_model_06_0FH[] = "Intel Dual-core processor";
unsigned char CPU_model_Unknown[] = "Unknown";


void init_CPU(void);
void output_CPU_info(void);

#endif