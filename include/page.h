
#ifndef PAGE_H_
#define PAGE_H_

#define PAGE_SHIFT 12
#define HEXAGON_L1_PTE_SIZE __HVM_PDE_S_4KB

#define PAGE_SIZE  (1UL << PAGE_SHIFT)
#define PAGE_MASK  (~((1 << PAGE_SHIFT) - 1))

#endif
