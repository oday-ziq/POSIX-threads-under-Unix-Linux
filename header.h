
#ifndef LIBRARIES
#define LIBRARIES

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <limits.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <time.h>
#include <sys/sem.h>
#include <semaphore.h>
#include <pthread.h>
#include <sys/shm.h>
#include <errno.h>
#include <sys/mman.h>
#include <unistd.h>
#include <sys/stat.h>
#include <GL/glut.h>
#include <math.h>
//#include <GLFW/glfw3.h>
#include <sys/msg.h>
#include <GL/gl.h>
#include <time.h>

#define MAX_LINE_LENGTH 256
#define MESSAGE_QUEUE_PATH "/tmp/message_queue"
#define B_SIZ (PIPE_BUF / 2)
const int NUM_TYPES = 5;
const char *liquid_medicine_types[] = {"Cough Syrup", "Antibiotics", "Antiseptics", "Antacids", "Laxatives"};

// Define pill medicine types
const char *pill_medicine_types[] = {"Antibiotics", "Antiseptics", "Antacids", "Laxatives", "Painkillers"};

const char* SHARED_MEMORY_LIQUID_MEDICINE_ARRAY_COUNT = "/liquid_medicine_array_count_shared_memory";
const char* SHARED_MEMORY_PILL_MEDICINE_ARRAY_COUNT = "/pill_medicine_array_count_shared_memory";
const char* SHARED_MEMORY_DESCARDED_BOTTLE_MEDICINE_COUNT = "/descarded_bottle_medicine_count_shared_memory";
const char* SHARED_MEMORY_DESCARDED_PILL_MEDICINE_COUNT = "/descarded_pill_medicine_count_shared_memory";
const char* SHARED_MEMORY_AVAILABLE_INSPECTION_EMPLOYEES = "/available_inspection_employees_shared_memory";
const char* SHARED_MEMORY_AVAILABLE_PACKAGING_EMPLOYESS = "/available_packaging_employees_shared_memory";

struct message {
    long message_type;
    char message[100];
};


#endif