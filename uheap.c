#include <inc/lib.h>
#define HeapSize (USER_HEAP_MAX - USER_HEAP_START)/4096
struct TheHeap {
	int empty ;
	int NoOfBlocks;
};
struct TheHeap Heap[HeapSize];
int first = 1 ;
int First_Time = 0;
struct save{
	int index ;
	int NoOfBlocks;
};
struct save arr[HeapSize];
void* malloc(uint32 size)
{
	//[1]Check if the size entered is greater than the heap size;
	if(size>HeapSize*PAGE_SIZE)
		return NULL;
	//[2]initialzation of heap at first time by empty = 0 , NoOfBlocks = -1;
	if (first==1)
	{
			for (int i = 0; i <HeapSize; i++)
			{
				Heap[i].empty = 0;
				Heap[i].NoOfBlocks = -1;
			}
			first++;
	}
	//[3]Methods-->first and best fit.
	if(sys_isUHeapPlacementStrategyFIRSTFIT())
	{
		//[4]initialization
		int NeededBlocks = ROUNDUP(size, PAGE_SIZE) / PAGE_SIZE;
		int FCounter;
		int index;
		int isTrue = 0;
		int Available = HeapSize;
		for (int i = 0; i < HeapSize; i++)
		{
			if (Heap[i].empty == 0)
			{
				index = i;
				FCounter = 1;
				for (int j = 1; j < NeededBlocks; j++)
				{
					if (Heap[i + j].empty != 0)
					{
						break;
					}
					FCounter++;
				}
				if (FCounter == NeededBlocks)
				{
					isTrue = 1;
					uint32 VA = (PAGE_SIZE* index) + USER_HEAP_START;
					//cprintf("Address%x\n",VA);
					//cprintf("NumberOFPages%d\n",found);
					if(VA >= USER_HEAP_MAX)
						return NULL;
					int x = i;
					Available-=NeededBlocks;
					if(NeededBlocks>Available)
						return NULL;
					for (int k = 0; k < NeededBlocks; k++)
					{
						Heap[x].empty = 1;
						Heap[x].NoOfBlocks = FCounter - 1;
						FCounter--;
						x++;
					}
					sys_allocateMem(VA, size);
					return (void*) VA;
				}
			}
	   }
	}
	else if(sys_isUHeapPlacementStrategyBESTFIT())
		{
			int NeededBlocks = ROUNDUP(size, PAGE_SIZE) / PAGE_SIZE;
			int counter= 0 ;
			int  FCounter ;
			int y=0;
			int t=0;
            if (NeededBlocks > HeapSize)
            	return NULL;
			while(y<HeapSize)
			{
				if(Heap[y].empty==0)
				{
					//cprintf("number:%d\n",y);
					FCounter = 1;
					arr[counter].index = y;

					//cprintf("ValueInsideTheArray:%d\n",arr[counter].index);

					int ponter = 0000;

					for(int j = 1 ;j<HeapSize;j++)
					{
						int ponter = 1111;
						int oh = y+j;
						if(oh<HeapSize )
						{
								if (Heap[oh].empty!=0)
								{
									break;
								}
						}
						FCounter++;
					}
					//cprintf("TheArray:%d\n",ponter);
					//cprintf("found%d\n",found);
				    arr[counter].NoOfBlocks = FCounter ;

				    counter++;
				    y+=(FCounter);
				    //cprintf("YValue%d\n",y);
				    //cprintf("H_heap%d\n",h_size);
			     }
				 else
				 {
					//cprintf("number:%d\n",t);
					y++;
				 }
				//cprintf("Max%d\n",y);
			}

		//cprintf("counter%d\n",counter);
		int  min =99999999;
		int IndexOFBlock;
		int NumOfBlocks;
		int arrindex;
        int AvailableSegment = HeapSize;
		for(int i = 0 ; i < counter ; i++)
		{
			//cprintf("NumOfBlocksatArray%d\n",arr[i].NoOfBlocks);
			//cprintf("ValueOfIndexatArray%d\n*********\n",arr[i].index);
			if(arr[i].NoOfBlocks<min&&arr[i].NoOfBlocks>=NeededBlocks)
			{
				min = arr[i].NoOfBlocks;

				arrindex = i;
			}
		}
		//cprintf("Min%d  arrindex=%d\n",min,arrindex);
		//cprintf("h_blocks We need to store %d\n",h_blocks);
		IndexOFBlock = arr[arrindex].index;
		//cprintf("IndexOFBlock%d\n",IndexOFBlock);
		NumOfBlocks = arr[arrindex].NoOfBlocks;
		//cprintf("NumOfBlocks%d\n",NumOfBlocks);
		uint32 VA = (PAGE_SIZE* IndexOFBlock) + USER_HEAP_START;
		if(VA >= USER_HEAP_MAX)
			return NULL;
		int x = IndexOFBlock;
		AvailableSegment -=NeededBlocks;
		for (int k = 0 ; k < NeededBlocks; k++)
		{
			int number = NeededBlocks ;
			if(AvailableSegment<number)
			{
				return NULL;
			}
			Heap[x].empty = 1;
			Heap[x].NoOfBlocks = number - 1;
			number--;
            x++;
		}
		//cprintf("-------------------------------------------------------------");
		sys_allocateMem(VA, size);
		return (void*) VA;
   }
   return NULL;
}
// free():
//	This function frees the allocation of the given virtual_address
//	To do this, we need to switch to the kernel, free the pages AND "EMPTY" PAGE TABLES
//	from page file and main memory then switch back to the user again.
//
//	We can use sys_freeMem(uint32 virtual_address, uint32 size); which
//		switches to the kernel mode, calls freeMem(struct Env* e, uint32 virtual_address, uint32 size) in
//		"memory_manager.c", then switch back to the user mode here
//	the freeMem function is empty, make sure to implement it.

void free(void* virtual_address)
{
	int index = ((uint32)virtual_address - USER_HEAP_START)/PAGE_SIZE;
	int size = Heap[index].NoOfBlocks;
	for (int i = index ; i <= index +size ; i++)
	{
		Heap[ i ].empty = 0;
		Heap[ i].NoOfBlocks = -1 ;
	}
	sys_freeMem((uint32)virtual_address ,(size+1));
}
//==================================================================================//
//============================== BONUS FUNCTIONS ===================================//
//==================================================================================//
//===============
// [1] realloc():
//===============

//	Attempts to resize the allocated space at "virtual_address" to "new_size" bytes,
//	possibly moving it in the heap.
//	If successful, returns the new virtual_address, in which case the old virtual_address must no longer be accessed.
//	On failure, returns a null pointer, and the old virtual_address remains valid.

//	A call with virtual_address = null is equivalent to malloc().
//	A call with new_size = zero is equivalent to free().

//  Hint: you may need to use the sys_moveMem(uint32 src_virtual_address, uint32 dst_virtual_address, uint32 size)
//		which switches to the kernel mode, calls moveMem(struct Env* e, uint32 src_virtual_address, uint32 dst_virtual_address, uint32 size)
//		in "memory_manager.c", then switch back to the user mode here
//	the moveMem function is empty, make sure to implement it.

void *realloc(void *virtual_address, uint32 new_size)
{
	panic("this function is not required...!!");
	return 0;
}


//==================================================================================//
//================================ OTHER FUNCTIONS =================================//
//==================================================================================//

void* smalloc(char *sharedVarName, uint32 size, uint8 isWritable)
{
	panic("this function is not required...!!");
	return 0;
}

void* sget(int32 ownerEnvID, char *sharedVarName)
{
	panic("this function is not required...!!");
	return 0;
}

void sfree(void* virtual_address)
{
	panic("this function is not required...!!");
}

void expand(uint32 newSize)
{
	panic("this function is not required...!!");
}
void shrink(uint32 newSize)
{
	panic("this function is not required...!!");
}

void freeHeap(void* virtual_address)
{
	panic("this function is not required...!!");
}
