#include <stdio.h>
#include <stdlib.h>
#include "vm.h"
#include "API.h"
#include "list.h"

int fifo_tail = 0;
struct Node *head = NULL;

int fifo()
{
    // store previous index for replace
	int PFN = fifo_tail;

	// update tail ie queue
    fifo_tail = (fifo_tail + 1) % MAX_PFN;
	//printf("fifo tail = %d\n", fifo_tail);
    return PFN; 
}

int lru()
{
    // use double linked
	int PFN;
	struct Node* prev = head;
	if(head != NULL) {
		// get least recently used
		while(prev->next != NULL) {
			prev = prev->next;
		}
		PFN = prev->data;

		// update list
		head = list_remove_tail(head);
		head = list_insert_head(head, PFN);
	}
    return PFN;
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
            // get attr of page that will be swap out
            ipte = read_IPTE(PFN);
            pte = read_PTE(ipte.pid, ipte.VPN);

			//printf("pagefault_handler dirty bit: %d", pte.dirty);

			// swap out only if dirty ie save the state
            if (pte.dirty) {
                pte.dirty = false;
                swap_out(ipte.pid, ipte.VPN, PFN);
            }

			// update page table of previous
			pte.valid = false;
            write_PTE(ipte.pid, ipte.VPN, pte);
		} else if (replacementPolicy == LRU) {
			// only occur at beginning when there is free frame
			head = list_insert_head(head, PFN);
		}

		// update the page table of new process to be added
        pte = read_PTE(pid, VPN);
        pte.valid = true;
		if(reqType == 'W') {
			pte.dirty = true;
		} else {
			pte.dirty = false;
		}
        pte.PFN = PFN;
        write_PTE(pid, VPN, pte);

		// update inverted page table
        ipte.pid = pid;
        ipte.VPN = VPN;
        write_IPTE(PFN, ipte);

		// swap in
        swap_in(pid, VPN, PFN);
		
        //ipte = read_IPTE(PFN);
        //printf("pagefault handler ipte vpn = %d\n", VPN);
		/* ---- */
		return PFN;
}

int get_PFN(int pid, int VPN, char reqType)
{
		/* Read page table entry for (pid, VPN) */
		PTE pte = read_PTE(pid, VPN);
        //printf("\nget_PFN PFN = %d, valid = %d, dirty = %d\n", pte.PFN, pte.valid, pte.dirty);

		/* if PTE is valid, it is a page hit. Return physical frame number (PFN) */
		if(pte.valid) {
		/* Mark the page dirty, if it is a write request */
				if(reqType == 'W') {
						pte.dirty = true;
						write_PTE(pid, VPN, pte);
				}

				if(replacementPolicy == LRU) {
					head = list_remove(head, pte.PFN);
					head = list_insert_head(head, pte.PFN);
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

