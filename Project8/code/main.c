#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>

#define BUFFER_SIZE 256
#define PHYSICAL_MEM_SIZE 256
#define PAGE_TABLE_SIZE 256
#define TLB_SIZE 16


void getPage(int logical_address, char *pageTable, char *physMem, int *openFrame, char *TLBPageNum, char *TLBFrameNum, int *tlbindex, int *tlbhits, int *pagefaults){
  unsigned char mask = 0xFF;
  unsigned char offset;
  char pageNum;
  int value = 0, newFrame = 0, frame = 0;
  bool tlbhit = false;
  printf("\nLA        %d\t", logical_address);
  pageNum = (logical_address >> 8) & mask;
  offset = logical_address & mask;

  int i;
  for (i = 0; i < TLB_SIZE; ++i){
    if (TLBPageNum[i] == pageNum && TLBPageNum[i] != -1){
      tlbhit = true;
      (*tlbhits)++;
      frame = TLBFrameNum[i];
      printf("tlbhit\t\t");
    }
  }
  if (tlbhit == false){
    if (pageTable[pageNum] != -1){
      printf("page hit\t");
    }
    else{
      (*pagefaults)++;
      printf("page fault\t");
      newFrame = readFromDisk(pageNum, physMem, openFrame);
      pageTable[pageNum] = newFrame;
    }
    TLBPageNum[(*tlbindex)] = pageNum;
    TLBFrameNum[(*tlbindex)] = pageTable[pageNum];
    (*tlbindex)++;
    (*tlbindex) = (*tlbindex) % TLB_SIZE;
    frame = pageTable[pageNum];
  }
  int ind = ((unsigned char)frame * PHYSICAL_MEM_SIZE) + offset;
  value = *(physMem + ind);
  printf("Physical Address: %d\t Value=%d", ind, value);
}

int readFromDisk(int pageNum, char *physMem, int *openFrame){
  FILE *disk = fopen("BACKING_STORE.bin", "r");
  if (disk == NULL){
    printf("Error opening disk file\n");
    exit(1);
  }
  char address[BUFFER_SIZE];
  memset(address, 0, sizeof(address));

  if (fseek(disk, pageNum * PHYSICAL_MEM_SIZE, SEEK_SET) != 0)
    printf("error with fseek!\n");
  if (fread(address, sizeof(char), PHYSICAL_MEM_SIZE, disk) == 0)
    printf("error with fread\n");
  int i;
  for (i = 0; i < PHYSICAL_MEM_SIZE; ++i){
    *((physMem + i + (*openFrame) * PHYSICAL_MEM_SIZE)) = address[i];
  }
  (*openFrame)++;
  fclose(disk);

  return (*openFrame)-1;
}



int main(int argc, char const *argv[]){
  if (argc > 2){
    fprintf(stderr, "%s\n", "Need only one parameter.");
  }

  FILE *address_file = fopen(argv[1], "r");
  if (address_file == NULL){
    fprintf(stderr, "%s\n", "Error opening the address file.");
    return 0;
  }

  unsigned char pageTable[PAGE_TABLE_SIZE];
  memset(pageTable, -1, sizeof(pageTable));

  char physMem[PHYSICAL_MEM_SIZE][PHYSICAL_MEM_SIZE];
  unsigned char TLBPageNum[TLB_SIZE];
  unsigned char TLBFrameNum[TLB_SIZE];
  int tlbindex = 0;
  memset(TLBPageNum, -1, sizeof(TLBPageNum));
  memset(TLBFrameNum, -1, sizeof(TLBFrameNum));

  char buffer[10];
  int openFrame = 0;
  int logical_address, totAddr = 0, tlbhits = 0, pagefaults = 0;
  double tlbHitRate = 0, pageFaultRate = 0;

  while (fgets(buffer, 10, address_file) != NULL){
    logical_address = atoi(buffer);
    getPage(logical_address, pageTable, (char*)physMem, &openFrame, TLBPageNum, TLBFrameNum, &tlbindex, &tlbhits, &pagefaults);
    totAddr++;
  }

  fclose(address_file);
  printf("\ntlbhits: %d, pagefaults: %d\n", tlbhits, pagefaults);
  pageFaultRate = (double)pagefaults / (double)totAddr;
  tlbHitRate = (double)tlbhits / (double)totAddr;
  printf("PageFaultRate: %3lf, tlbhitRate: %.3lf\n", pageFaultRate, tlbHitRate);
  
  return 0;
}
