#include <stdio.h>
#include <stdlib.h>
#include "vm.h"
#include "API.h"
#include "list.h"

int fifo()
{
		return 0;
}

int lru()
{
		return 0;
}

int clock()
{
		return 0;
}

/*========================================================================*/

int find_replacement()
{
		int PFN;
		if(replacementPolicy == ZERO)  PFN = 0;
		else if(replacementPolicy == FIFO)  PFN = fifo();
		else if(replacementPolicy == LRU) PFN = lru();
		else if(replacementPolicy == CLOCK) PFN = clock();

		return PFN;
}

int pagefault_handler(int pid, int VPN, char reqType)
{
		int PFN;
        IPTE ipte;
        PTE pte;

		// find a free PFN.
		PFN = get_freeframe();
		
		// no free frame available. find a victim using a page replacement algorithm. ;
		if(PFN < 0) {
			PFN = find_replacement();
			/* ---- */
            // check if dirty bit is set
            ipte = read_IPTE(PFN);
            pte = read_PTE(ipte.pid, ipte.VPN);

            // set valid bit to false
            pte.valid = false;

            if (pte.dirty) {
                // swap out
                pte.dirty = false;
                swap_out(ipte.pid, ipte.VPN, PFN);
            }
            write_PTE(ipte.pid, ipte.VPN, pte);
		}

        // swap in
        swap_in(pid, VPN, PFN);
        pte = read_PTE(pid, VPN);
        pte.valid = true;
        pte.PFN = PFN;
        write_PTE(pid, VPN, pte);
        ipte.pid = pid;
        ipte.VPN = VPN;
        write_IPTE(PFN, ipte);
		
        //ipte = read_IPTE(PFN);
        //printf("pagefault handler ipte vpn = %d\n", VPN);
		/* ---- */
		return PFN;
}

int get_PFN(int pid, int VPN, char reqType)
{
		/* Read page table entry for (pid, VPN) */
		PTE pte = read_PTE(pid, VPN);
        //printf("PFN = %d, valid = %d, dirty = %d\n", pte.PFN, pte.valid, pte.dirty);

		/* if PTE is valid, it is a page hit. Return physical frame number (PFN) */
		if(pte.valid) {
		/* Mark the page dirty, if it is a write request */
				if(reqType == 'W') {
						pte.dirty = true;
						write_PTE(pid, VPN, pte);
				}
				return pte.PFN;
		}
		
		/* PageFault, if the PTE is invalid. Return -1 */
		return -1;
}

int MMU(int pid, int virtAddr, char reqType, bool *hit)
{
		int PFN, physicalAddr;
		int VPN = 0, offset = 0, mask = 0x00FF;
		
		/* calculate VPN and offset */
        // Get first 2 hex to get index. ie 0x12A3 -> 0x0012
        VPN = virtAddr >> 8;

        // Get last 2 hex bits to get offset. ie 0x12A3 -> 0x00A3
        offset = virtAddr & mask;

        // if index is >= max index or offset > 256, return -1
        // including max since index start at 0
        if (VPN >= MAX_VPN || offset > 256) {
            return -1;
        }
		
		// read page table to get Physical Frame Number (PFN)
		PFN = get_PFN(pid, VPN, reqType);

        //printf("VPN = %d, offset = %d, PFN = %d, MAX_VPN = %d\n", VPN, offset, PFN, MAX_VPN);
		if(PFN >= 0) { // page hit
				stats.hitCount++;
				*hit = true;
		} else { // page miss
				stats.missCount++;
				*hit = false;
				PFN = pagefault_handler(pid, VPN, reqType);
		}

		physicalAddr = (PFN << 8) + offset;
		return physicalAddr;
}

