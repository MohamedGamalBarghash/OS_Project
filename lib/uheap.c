#include <inc/lib.h>

//==================================================================================//
//============================== GIVEN FUNCTIONS ===================================//
//==================================================================================//

int FirstTimeFlag = 1;
void InitializeUHeap()
{
	if(FirstTimeFlag)
	{
#if UHP_USE_BUDDY
		initialize_buddy();
		cprintf("BUDDY SYSTEM IS INITIALIZED\n");
#endif
		FirstTimeFlag = 0;
	}
}

//==================================================================================//
//============================ REQUIRED FUNCTIONS ==================================//
//==================================================================================//

//=============================================
// [1] CHANGE THE BREAK LIMIT OF THE USER HEAP:
//=============================================
/*2023*/
void* sbrk(int increment)
{
	return (void*) sys_sbrk(increment);
}
struct U_init u_init [NUM_OF_UHEAP_PAGES];
uint32* find_consecutive_user_pages(uint32 num_pages) {
	uint32 start_address = sys_get_hard_limit()+PAGE_SIZE;
	uint32 npages = num_pages;
	uint32 *ptr_page_table = NULL;
	uint32 permission;

	while (start_address < USER_HEAP_MAX) {
		if (npages == 0) {
			return(void*) start_address - (num_pages * PAGE_SIZE);
		}
		permission = sys_perm_user(start_address);
		if ((permission & PERM_MARKED) | (permission * PERM_PRESENT)) {
			npages = num_pages;
		} else {
			npages--;
		}
		start_address += PAGE_SIZE;
	}
	if (npages == 0) {
		return (void*) start_address - (num_pages * PAGE_SIZE);
	} else {
		return NULL;
	}
}
//=================================
// [2] ALLOCATE SPACE IN USER HEAP:
//=================================
void* malloc(uint32 size)
{
	//==============================================================
	//DON'T CHANGE THIS CODE========================================
	InitializeUHeap();
	if (size == 0) return NULL ;
	//==============================================================
	//TODO: [PROJECT'23.MS2 - #09] [2] USER HEAP - malloc() [User Side]
	// Write your code here, remove the panic and write your code
	//panic("malloc() is not implemented yet...!!");
	//return NULL;
	//Use sys_isUHeapPlacementStrategyFIRSTFIT() and sys_isUHeapPlacementStrategyBESTFIT()
	//to check the current strategy

	// Check if the User heap placement strategy is FIRSTFIT
	if (sys_isUHeapPlacementStrategyFIRSTFIT() == 0) {
		return NULL;  // Return NULL if not using FIRSTFIT strategy
	}

		//start of user page allocator
	//	uint32 start = 0;
	//	uint32 sizeOfBlockAlloc = SYS_Get_Hard_Limit - USER_HEAP_START;

	//	if (size > SYS_Get_Hard_Limit - USER_HEAP_START) {
	//		return NULL;
	//	}

	//	struct Env * e = NULL;
	//	LIST_FOREACH(envs)


	//	// Check if the requested size exceeds the available space in the User heap
	//	if (size > sizeOfBlockAlloc) {
	//		return NULL;  // Return NULL if insufficient space in the User heap
	//	}

		// Check if the requested size is within the range that can be handled by the FIRSTFIT strategy
	if (size <= DYN_ALLOC_MAX_BLOCK_SIZE) {
		// If the size is within the range, allocate a block using the FIRSTFIT strategy
		void* block = alloc_block_FF(size);

		if (block != NULL) {
			return block;  // Return the allocated block if successful
		}
	}
	else {

		uint32 num_pages = ROUNDUP(size, PAGE_SIZE) / PAGE_SIZE;
		uint32 total_size = num_pages * PAGE_SIZE;
		cprintf("this is my num of pages %d \n", num_pages);
		cprintf("this is my total size %d \n", total_size);
		cprintf("this is my start %p \n", sys_get_hard_limit());
		cprintf("this is my end %p \n", USER_HEAP_MAX);
		cprintf("this is where I finish %p \n", sys_get_hard_limit() + (PAGE_SIZE * num_pages));

		int count = 0;
		int index = 0;

		// Assuming SYS_Get_Hard_Limit is a variable or function that returns the hard limit of the system
		uint32 addr = USER_HEAP_START + DYN_ALLOC_MAX_SIZE + sys_get_hard_limit() + PAGE_SIZE;

		for (int i = 0; i < NUM_OF_UHEAP_PAGES; i++) {
			if (count == num_pages) {
				index = i - num_pages;  // Adjust index to the starting index of the contiguous block
				break;  // Break the loop if enough space is found
			}
			if (u_init[i].va == (uint32*)0) {
//				cprintf("\n\nfound one\n\n");
				count++;
			}

			addr += PAGE_SIZE;
		}
//		cprintf("finished executing\n");

		// Check if enough space is found
		if (count < num_pages) {
			// Handle insufficient space error
			return NULL;
		}

		u_init[index].va = (uint32*) (addr - (PAGE_SIZE * num_pages));
		u_init[index].size = count;
		cprintf("This is my initial stuff %p, this is my index: %d and this is my size: %d \n", u_init[index].va, index, u_init[index].size);
		uint32 temp = (uint32)u_init[index].va;
		uint32 perms = sys_perm_user(temp);
		cprintf("Thsese are ny permissions %p \n", perms);
		if ((perms & PERM_MARKED)) {
			cprintf("The page already exists");
			return (void*)u_init[index].va;
		}
		sys_allocate_user_mem((uint32) u_init[index].va, total_size);
		return (void*) u_init[index].va;

	}
	return NULL;

}


//=================================
// [3] FREE SPACE FROM USER HEAP:
//=================================
void free(void* virtual_address)
{
	//TODO: [PROJECT'23.MS2 - #11] [2] USER HEAP - free() [User Side]
	// Write your code here, remove the panic and write your code
	panic("free() is not implemented yet...!!");
}


//=================================
// [4] ALLOCATE SHARED VARIABLE:
//=================================
void* smalloc(char *sharedVarName, uint32 size, uint8 isWritable)
{
	//==============================================================
	//DON'T CHANGE THIS CODE========================================
	InitializeUHeap();
	if (size == 0) return NULL ;
	//==============================================================
	panic("smalloc() is not implemented yet...!!");
	return NULL;
}

//========================================
// [5] SHARE ON ALLOCATED SHARED VARIABLE:
//========================================
void* sget(int32 ownerEnvID, char *sharedVarName)
{
	//==============================================================
	//DON'T CHANGE THIS CODE========================================
	InitializeUHeap();
	//==============================================================
	// Write your code here, remove the panic and write your code
	panic("sget() is not implemented yet...!!");
	return NULL;
}


//==================================================================================//
//============================== BONUS FUNCTIONS ===================================//
//==================================================================================//

//=================================
// REALLOC USER SPACE:
//=================================
//	Attempts to resize the allocated space at "virtual_address" to "new_size" bytes,
//	possibly moving it in the heap.
//	If successful, returns the new virtual_address, in which case the old virtual_address must no longer be accessed.
//	On failure, returns a null pointer, and the old virtual_address remains valid.

//	A call with virtual_address = null is equivalent to malloc().
//	A call with new_size = zero is equivalent to free().

//  Hint: you may need to use the sys_move_user_mem(...)
//		which switches to the kernel mode, calls move_user_mem(...)
//		in "kern/mem/chunk_operations.c", then switch back to the user mode here
//	the move_user_mem() function is empty, make sure to implement it.
void *realloc(void *virtual_address, uint32 new_size)
{
	//==============================================================
	//DON'T CHANGE THIS CODE========================================
	InitializeUHeap();
	//==============================================================

	// Write your code here, remove the panic and write your code
	panic("realloc() is not implemented yet...!!");
	return NULL;

}


//=================================
// FREE SHARED VARIABLE:
//=================================
//	This function frees the shared variable at the given virtual_address
//	To do this, we need to switch to the kernel, free the pages AND "EMPTY" PAGE TABLES
//	from main memory then switch back to the user again.
//
//	use sys_freeSharedObject(...); which switches to the kernel mode,
//	calls freeSharedObject(...) in "shared_memory_manager.c", then switch back to the user mode here
//	the freeSharedObject() function is empty, make sure to implement it.

void sfree(void* virtual_address)
{
	// Write your code here, remove the panic and write your code
	panic("sfree() is not implemented yet...!!");
}


//==================================================================================//
//========================== MODIFICATION FUNCTIONS ================================//
//==================================================================================//

void expand(uint32 newSize)
{
	panic("Not Implemented");

}
void shrink(uint32 newSize)
{
	panic("Not Implemented");

}
void freeHeap(void* virtual_address)
{
	panic("Not Implemented");

}
