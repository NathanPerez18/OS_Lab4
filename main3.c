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
#define MAX_PROCESSES 100 // Constraint, as stated in lab manual, Changed to 100 for testing


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
void getFilenameFromUser(char* filename, int max_length);
void readProcesses(const char* filename, Process processes[], int *size);
void insertInOrder(Process array[], Process newProcess, int *size);
void printProcesses(const Process processes[], int size);
void simulateProcessArrival(const Process processes[], int size);



























// Start of the main

int main() {
    MemoryBlock* root = initializeMemory(TOTAL_MEMORY); // Initialize with 1KB for testing
    
    // All the File stuff
    char filename[256]; // Buffer to hold the filename
    Process processes[MAX_PROCESSES];
    int size = 0;
    getFilenameFromUser(filename, sizeof(filename)); // Call the function to get the filename from the user
    readProcesses(filename, processes, &size);// Call the function to process the file
    // Simple test function call to check if program was able to pull values
    printProcesses(processes, size); // comment this out before hand in
    // After this runs, there will be an array holding all of the process that were collected from the text file

    simulateProcessArrival(processes, size);








    
    return 0;
}


// End of the main

























// Memory Functions
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

// End of memory Functions 















// Start of Linked list fucntions

Node* createNode(Process process) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    if (newNode == NULL) {
        printf("Error creating a new node.\n");
        exit(0);
    }
    newNode->process = process;
    newNode->next = NULL;
    return newNode;
}

void insertAtBeginning(Node** head, Process process) {
    Node* newNode = createNode(process);
    newNode->next = *head;
    *head = newNode;
}

void insertAtEnd(Node** head, Process process) {
    Node* newNode = createNode(process);
    if (*head == NULL) {
        *head = newNode;
        return;
    }
    Node* temp = *head;
    while (temp->next != NULL) {
        temp = temp->next;
    }
    temp->next = newNode;
}

void deleteNode(Node** head, Process process) {
    Node* temp = *head, *prev = NULL;
    if (temp != NULL && temp->process.arrivalTime == process.arrivalTime) {
        *head = temp->next;
        free(temp);
        return;
    }
    while (temp != NULL && temp->process.arrivalTime != process.arrivalTime) {
        prev = temp;
        temp = temp->next;
    }
    if (temp == NULL) return; // Process not found in the list
    prev->next = temp->next;
    free(temp);
}


void displayList(Node* head) {
    Node* temp = head;
    while (temp != NULL) {
        printf("%d -> ", temp->process);
        temp = temp->next;
    }
    printf("NULL\n");
}



// End of Linked list fucntions












// File interpretation functions

// Function to prompt the user for the filename and store it
void getFilenameFromUser(char* filename, int max_length) {
    printf("Please enter name of Dispatcher list: ");// Ask the user for the filename
    fgets(filename, max_length, stdin);
    size_t len = strlen(filename); 
    if (len > 0 && filename[len - 1] == '\n') {
        filename[len - 1] = '\0';
    }
}

// Function to read text  file
void readProcesses(const char* filename, Process processes[], int *size) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    Process tempProcess;
    *size = 0; // Start with an empty array
    while (fscanf(file, "%d, %d, %d, %d, %d, %d, %d, %d\n",
                  &tempProcess.arrivalTime,
                  &tempProcess.priority,
                  &tempProcess.processorTime,
                  &tempProcess.memoryRequirement,
                  &tempProcess.printers,
                  &tempProcess.scanners,
                  &tempProcess.modems,
                  &tempProcess.cds) == 8) {
        insertInOrder(processes, tempProcess, size);
    }

    fclose(file);
}

// Function to insert
void insertInOrder(Process array[], Process newProcess, int *size) {
    int i = *size - 1;
    // Find the correct position for the new process
    while (i >= 0 && array[i].arrivalTime > newProcess.arrivalTime) {
        array[i + 1] = array[i]; // Shift elements to the right
        i--;
    }
    array[i + 1] = newProcess; // Insert the new process
    (*size)++; // Increase the size of the array
}

// Function to print the contents of the processes array, this is a test function, not needed in end program
void printProcesses(const Process processes[], int size) {
    printf("Listing all processes:\n");
    for (int i = 0; i < size; ++i) {
        printf("Process %d:\n", i + 1);
        printf("\tArrival Time: %d\n", processes[i].arrivalTime);
        printf("\tPriority: %d\n", processes[i].priority);
        printf("\tProcessor Time: %d\n", processes[i].processorTime);
        printf("\tMemory Requirement: %d\n", processes[i].memoryRequirement);
        printf("\tPrinters: %d\n", processes[i].printers);
        printf("\tScanners: %d\n", processes[i].scanners);
        printf("\tModems: %d\n", processes[i].modems);
        printf("\tCDs: %d\n\n", processes[i].cds);
    }
}

// End of File interpretation functions








// Start of handling scheduling
void handleProcess(Process process) {
    // Placeholder for now. Implement the logic that should be executed for each matching process.
    printf("Handling process with arrival time %d\n", process.arrivalTime);
}

void simulateProcessArrival(const Process processes[], int size) {
    for (int currentTime = 0; currentTime < MAX_PROCESSES; currentTime++) {
        printf("New Tick %d\n",currentTime);
        for (int i = 0; i < size; i++) {
            if (processes[i].arrivalTime == currentTime) {
                handleProcess(processes[i]);
            }
        }
    }
}







// End of handling scheduling
