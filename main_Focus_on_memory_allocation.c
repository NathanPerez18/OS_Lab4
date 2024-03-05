/*
Nathan Perez 100754066
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>
#include <stdbool.h>

// Define constraints for resources
#define TOTAL_MEMORY 960 // in Mbytes
#define MEMORY_FOR_RT 64  // in Mbytes
#define TOTAL_PRINTERS 2    // Total available printers
#define TOTAL_SCANNERS 1    // Total available scanners
#define TOTAL_MODEMS 1      // Total available modems
#define TOTAL_CDS 2         // Total available CD drives
#define MAX_PROCESSES 1000 // Constraint, as stated in lab manual


// Define all structures
// Define a structure for processes
typedef struct {
    int arrivalTime;
    int priority;
    int processorTime;
    int memoryRequirement;
    int printers;
    int scanners;
    int modems;
    int cds;
} Process;

// Creating nessesary structure for the linked list
typedef struct Node {
    Process process;
    struct Node* next;
} Node;

// Creates structure for holding resource information
typedef struct {
    int usedMemory;    // Currently used memory in Mbytes
    int usedPrinters;  // Currently used printers
    int usedScanners;  // Currently used scanners
    int usedModems;    // Currently used modems
    int usedCDs;       // Currently used CDs
} ResourceState;

// Create structure for storing memory
typedef struct MemoryBlock {
    int size;                 // Size of the memory block (power of 2)
    int startAddress;         // Starting address (for simulation purposes)
    bool isAllocated;         // Allocation status
    struct MemoryBlock *buddy; // Pointer to its buddy
    struct MemoryBlock *parent; // Pointer to its parent
    struct MemoryBlock *left;  // Left child in the binary tree
    struct MemoryBlock *right; // Right child in the binary tree
} MemoryBlock;

MemoryBlock* initializeMemory(int totalMemorySize);
MemoryBlock* initializeMemory(int totalMemorySize);
MemoryBlock* allocateBlock(MemoryBlock* root, int size);
void deallocateBlock(MemoryBlock* block);
void mergeIfPossible(MemoryBlock* block);

int main() {
    MemoryBlock* root = initializeMemory(TOTAL_MEMORY); // Initialize with 1KB for testing
    
    return 0;
}

MemoryBlock* initializeMemory(int totalMemorySize) {
    MemoryBlock* root = (MemoryBlock*)malloc(sizeof(MemoryBlock));
    if (root == NULL) {
        perror("Failed to allocate memory for root block");
        exit(EXIT_FAILURE);
    }
    root->size = totalMemorySize;
    root->startAddress = 0; // Assuming starting address is 0
    root->isAllocated = false;
    root->buddy = NULL;
    root->parent = NULL;
    root->left = NULL;
    root->right = NULL;
    return root;
}

void deallocateBlock(MemoryBlock* block) {
    // Mark the block as not allocated
    block->isAllocated = false;
    printf("Block starting at %d of size %d has been freed.\n", block->startAddress, block->size);
    // Attempt to merge with buddy
    mergeIfPossible(block);
}


void mergeIfPossible(MemoryBlock* block) {
    if (block == NULL || block->parent == NULL || block->buddy == NULL) {
        return; // Base case or invalid scenario
    }

    if (!block->isAllocated && !block->buddy->isAllocated) {
        printf("Merging block starting at %d with buddy of size %d.\n", block->startAddress, block->size);

        MemoryBlock* parent = block->parent;

        // Before freeing, remove references from the parent
        parent->left = NULL;
        parent->right = NULL;

        free(block);
        free(block->buddy);

        // Parent block is now free and undivided
        parent->isAllocated = false;

        // Attempt further merging up the tree
        mergeIfPossible(parent);
    }
}

void splitBlock(MemoryBlock* block) {
    int halfSize = block->size / 2;
    block->left = (MemoryBlock*)malloc(sizeof(MemoryBlock));
    block->right = (MemoryBlock*)malloc(sizeof(MemoryBlock));

    if (block->left == NULL || block->right == NULL) {
        perror("Failed to allocate memory for block splitting");
        exit(EXIT_FAILURE);
    }

    // Initialize left child
    block->left->size = halfSize;
    block->left->startAddress = block->startAddress;
    block->left->isAllocated = false;
    block->left->parent = block;
    block->left->left = NULL;
    block->left->right = NULL;
    block->left->buddy = block->right;

    // Initialize right child
    block->right->size = halfSize;
    block->right->startAddress = block->startAddress + halfSize;
    block->right->isAllocated = false;
    block->right->parent = block;
    block->right->left = NULL;
    block->right->right = NULL;
    block->right->buddy = block->left;
}

MemoryBlock* allocateBlock(MemoryBlock* block, int size) {
    // Base case: If the block is already allocated or too small, return NULL
    if (block->isAllocated || block->size < size) {
        return NULL;
    }
    
    // Perfect fit or cannot split further
    if (block->size == size || block->size / 2 < size) {
        block->isAllocated = true;
        return block;
    }
    
    // Try to split the block if it's larger than needed
    if (block->left == NULL && block->right == NULL) {
        splitBlock(block); // Split the block into two buddies
    }

    // Recursively attempt to allocate in the left or right child
    MemoryBlock* allocatedBlock = allocateBlock(block->left, size);
    if (allocatedBlock != NULL) {
        return allocatedBlock;
    }

    return allocateBlock(block->right, size);
}
