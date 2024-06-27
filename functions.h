#ifndef __FUNCTIONS__
#define __FUNCTIONS__

#include "header.h"
#include "constants.h"


void readFile(char* filename) {
    char line[200];
    char label[50];

    FILE* file;

    file = fopen(filename, "r");

    if (file == NULL) {
        perror("The file not exist\n");
        exit(-2);
    }

    char separator[] = "=";

    while (fgets(line, sizeof(line), file) != NULL) {

        char* str = strtok(line, separator);
        strncpy(label, str, sizeof(label));
        str = strtok(NULL, separator);

        if (strcmp(label, "NUMBER_OF_DIFFERENT_MEDICINES_PER_PRODUCTION_LINE") == 0) {
            NUMBER_OF_DIFFERENT_MEDICINES_PER_PRODUCTION_LINE = atoi(str);
        }
        else if (strcmp(label, "NUMBER_OF_PRODUCTION_LINES") == 0) {
            NUMBER_OF_PRODUCTION_LINES = atoi(str);
        }
        else if (strcmp(label, "MAX_ALLAWOD_NUMBER_OF_BOTTLED_OUTOF_SPEC") == 0) {
            MAX_ALLAWOD_NUMBER_OF_BOTTLED_OUTOF_SPEC = atoi(str);
        }
        else if (strcmp(label, "NUMBER_OF_INSPECTION_AND_PACKAGING_EMPLOYEES") == 0) {
            NUMBER_OF_INSPECTION_AND_PACKAGING_EMPLOYEES = atoi(str);
        }
        else if (strcmp(label, "MAXIMUM_NUMBER_OF_PLASTIC_CONTAINERS") == 0) {
            MAXIMUM_NUMBER_OF_PLASTIC_CONTAINERS = atoi(str);
        }        
        else if (strcmp(label, "MAX_ALLAWOD_NUMBER_OF_BILL_OUTOF_SPEC") == 0) {
            MAX_ALLAWOD_NUMBER_OF_BILL_OUTOF_SPEC = atoi(str);
        }
        else if (strcmp(label, "MAXIMUM_NUMBER_ALLOWED_FOR_TYPE") == 0) {
            MAXIMUM_NUMBER_ALLOWED_FOR_TYPE = atoi(str);
        }
        else if (strcmp(label, "LIQUID_LEVEL_ACCEPTED_RANGE_MIN") == 0) {
            LIQUID_LEVEL_ACCEPTED_RANGE_MIN = atoi(str);
        }
        else if (strcmp(label, "LIQUID_LEVEL_ACCEPTED_RANGE_MAX") == 0) {
            LIQUID_LEVEL_ACCEPTED_RANGE_MAX = atoi(str);
        }
        else if (strcmp(label, "LIQUID_LEVEL_GENERATED_MIN") == 0) {
            LIQUID_LEVEL_GENERATED_MIN = atoi(str);
        }
        else if (strcmp(label, "LIQUID_LEVEL_GENERATED_MAX") == 0) {
            LIQUID_LEVEL_GENERATED_MAX = atoi(str);
        }
        else if (strcmp(label, "TIME_NEEDED_FOR_EMPLOYEE_MIN") == 0) {
            TIME_NEEDED_FOR_EMPLOYEE_MIN = atoi(str);
        }
        else if (strcmp(label, "TIME_NEEDED_FOR_EMPLOYEE_MAX") == 0) {
            TIME_NEEDED_FOR_EMPLOYEE_MAX = atoi(str);
        }
        else if (strcmp(label, "NUM_OF_INSPECTION_EMPLOYEES") == 0) {
            NUM_OF_INSPECTION_EMPLOYEES = atoi(str);
        }
        else if (strcmp(label, "NUM_OF_PACKAGING_EMPLOYEES") == 0) {
            NUM_OF_PACKAGING_EMPLOYEES = atoi(str);
        }
        else if(strcmp(label, "SIMULATION_MAX_TIME") == 0){
            SIMULATION_MAX_TIME = atoi(str);
        }

    }
    fclose(file);
}

// Returns a random sleep dependant to energy_level, the bigger the energy level the less time the ball takes time to arrive to the next player.
// int getRandomSleepForEnergyLevel(int energy_level, pid_t process_id){
//     srand(process_id);
//     return 1 + rand() % (300 / energy_level);
// }

int random_range(int left, int right, pid_t process_id) {
    srand(process_id);
    return left + rand() % (right - left + 1);
}

// int getMax(int a, int b){
//     if(a > b){
//         return a;
//     }
//     return b;
// }

void initialize_shared_memory_variable(char* name) {
    const int SIZE = sizeof(int);

    int shm_fd = shm_open(name, O_CREAT | O_RDWR, 0666);
    ftruncate(shm_fd, SIZE);

    int* ptr = (int*)mmap(0, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);

    *ptr = 0;
}

int* get_shared_memory_variable_ptr(char* name) {
    const int SIZE = sizeof(int);

    int shm_fd = shm_open(name, O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("shm_open");
        exit(-1);
    }

    int* ptr = (int*)mmap(0, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (ptr == MAP_FAILED) {
        perror("mmap");
        exit(-1);
    }
    return ptr;
}

void initialize_shared_memory_array_variable(char* name, size_t array_size) {
    const size_t SIZE = array_size * sizeof(int);

    int shm_fd = shm_open(name, O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("shm_open");
        exit(-1);
    }

    if (ftruncate(shm_fd, SIZE) == -1) {
        perror("ftruncate");
        exit(-1);
    }

    int* ptr = (int*)mmap(0, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (ptr == MAP_FAILED) {
        perror("mmap");
        exit(-1);
    }

    // Initialize array with zeroes
    for (size_t i = 0; i < array_size; i++) {
        ptr[i] = 0;
    }

    munmap(ptr, SIZE);
    close(shm_fd);
}

int* get_shared_memory_array_variable_ptr(char* name, size_t array_size) {
    const size_t SIZE = array_size * sizeof(int);

    int shm_fd = shm_open(name, O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("shm_open");
        exit(-1);
    }

    int* ptr = (int*)mmap(0, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (ptr == MAP_FAILED) {
        perror("mmap");
        exit(-1);
    }
    return ptr;
}

void send_a_message(char* msg) {
    // Create a file if it doesn't exist
    int fd = open(MESSAGE_QUEUE_PATH, O_CREAT | O_RDWR, 0666);
    if (fd == -1) {
        perror("open");
        exit(1);
    }
    close(fd);
    key_t key = ftok(MESSAGE_QUEUE_PATH, 'A');

    if (key == -1) {
        perror("ftok");
        exit(1);
    }

    // Create or obtain the message queue ID
    int msgid = msgget(key, 0666 | IPC_CREAT);
    if (msgid == -1) {
        perror("msgget");
        exit(1);
    }

    // Prepare the message
    struct message message;
    message.message_type = 1; // Unique message type
    strcpy(message.message, msg);

    // Send the message
    if (msgsnd(msgid, &message, sizeof(message), 0) == -1) {
        perror("msgsnd");
        exit(1);
    }
    printf("Message Sent = %s\n", message.message);
}

char* receive_a_message() {
    static struct message received_message;
    key_t key = ftok(MESSAGE_QUEUE_PATH, 'A');
    if (key == -1) {
        perror("ftok");
        exit(1);
    }

    int msgid = msgget(key, 0666 | IPC_CREAT);
    if (msgid == -1) {
        perror("msgget");
        exit(1);
    }

    // Receive the message
    if (msgrcv(msgid, &received_message, sizeof(received_message), 1, IPC_NOWAIT) == -1) {
        if (errno == ENOMSG) {
            // No message available yet
            return NULL;
        }
        else {
            perror("msgrcv");
            exit(1);
        }
    }

    return received_message.message;
}
/**
int get_random_sleep_according_to_energy_level(int total_energy, pid_t process_id){
    return random_range(TIME_NEEDED_TO_COLLECT_CONTAINERS_MIN, getMax(TIME_NEEDED_TO_COLLECT_CONTAINERS_MIN, 15000/total_energy), process_id);
}**/

// int plane_gets_targeted(pid_t process_id){
//     if(random_range(0, 100, process_id) <= 5){
//         return 1;
//     }
//     return 0;
// }

// int worker_get_targeted(int energy_level, pid_t process_id){
//     if(random_range(0, 100, process_id) <= 200/energy_level){
//         return 1;
//     }
//     return 0;
// }

void kill_process(pid_t pid) {
    if (kill(pid, SIGKILL) == -1) {
        perror("kill");
    }
}
// random 0 or 1  , 80% chance for 1 depend on the time and the process id
int random_0_or_1(double probability_of_one, pid_t process_id) {
    // Generate a random number between 0 and 1
    srand(process_id);
    double random_value = (double)rand() / RAND_MAX;
    return random_value < probability_of_one ? 1 : 0;
}

//get random liquid medicine type
char* get_random_liquid_medicine_type() {
    //not based on pid 
    return liquid_medicine_types[rand() % NUM_TYPES];
}

//get random pill medicine type
char* get_random_pill_medicine_type() {
    return pill_medicine_types[rand() % NUM_TYPES];
}

char * get_liquid_medicine_from_type(int type_index){
    return liquid_medicine_types[type_index];
}

char* get_pill_medicine_from_type(int type_index) {
    return pill_medicine_types[type_index];
}
// get random number based in rand for MAXIMUM_NUMBER_OF_PLASTIC_CONTAINERS
int get_random_number_of_plastic_containers() {
    return random_range(0, MAXIMUM_NUMBER_OF_PLASTIC_CONTAINERS, getpid());
}
// get random sleep value between 1 and 4
int get_random_sleep_value() {
    return random_range(TIME_NEEDED_FOR_EMPLOYEE_MIN, TIME_NEEDED_FOR_EMPLOYEE_MAX, getpid());
}
#endif