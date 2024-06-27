#include "functions.h"
#include "constants.h"
#include "header.h"
time_t start_time, current_time;
pid_t production_lines[100];
void signal_handler(int signum);
void create_production_lines();
int check_if_game_ends();
void kill_all_production_lines();
void init_shared_memory_variables();

int gui_pid;

int main() {
    readFile("args.txt");
    signal(SIGUSR1, signal_handler);
    time(&start_time);
    init_shared_memory_variables();
    create_production_lines();

    /* Run Opengl */
    gui_pid = fork();
    if (gui_pid == -1)
    {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    else if (gui_pid == 0)
    {
        char ogl_exe[6];
        sprintf(ogl_exe, "./opengl");
        execl(ogl_exe, NULL);
        perror("Execution failed "); // execl will only return if it fails
        exit(EXIT_FAILURE);
    }
    // wait for all production lines to finish
    for(int i = 0; i < NUMBER_OF_PRODUCTION_LINES; i++){
        waitpid(production_lines[i], NULL, 0);
    }
    //kill all production lines
    while (check_if_game_ends() == 0) {};
    
    kill_all_production_lines();
    kill_process(gui_pid);

    return 0;


}
void create_production_lines() {

    for (int i = 0; i < NUMBER_OF_PRODUCTION_LINES; i++) {
        production_lines[i] = fork();
        if (production_lines[i] == -1) {
            perror('production_lines Creation Failed');
            exit(-1);
        }
        else if (production_lines[i] == 0) {
            char index_str[10];
            snprintf(index_str, sizeof(index_str), "%d", i);
            // exec as liquid for half the NUMBER_OF_PRODUCTION_LINES other half bills
            if (i % 2 == 0) {
                // give the execlp i 

                execlp("./production_lines", "production_lines", "pills", index_str, NULL);

            }
            else {
                execlp("./production_lines", "production_lines", "liquid", index_str, NULL);
            }
            perror("Exec Error\n");
            exit(-5);
        }
    }
}
int check_if_game_ends() {
    sleep(2);
    time(&current_time);
    double seconds = difftime(current_time, start_time);
    if (seconds >= SIMULATION_MAX_TIME) {
        return 1;
    }
    return 0;
}
//kill all production lines
void kill_all_production_lines() {
    for (int i = 0; i < NUMBER_OF_PRODUCTION_LINES; i++) {
        kill_process(production_lines[i]);
    }
    printf("Done\n");
    kill_process(gui_pid);
    kill_process(getpid());
}
void signal_handler(int signum) {
    printf("One of the conditions has failed !!!!!!!!!!!!!!!!!!!\n");
    kill_all_production_lines();
    exit(0);
}
void init_shared_memory_variables() {
    initialize_shared_memory_array_variable(SHARED_MEMORY_LIQUID_MEDICINE_ARRAY_COUNT, NUM_TYPES);
    initialize_shared_memory_array_variable(SHARED_MEMORY_PILL_MEDICINE_ARRAY_COUNT, NUM_TYPES);
    initialize_shared_memory_variable(SHARED_MEMORY_DESCARDED_BOTTLE_MEDICINE_COUNT);
    initialize_shared_memory_variable(SHARED_MEMORY_DESCARDED_PILL_MEDICINE_COUNT);
    initialize_shared_memory_variable(SHARED_MEMORY_AVAILABLE_INSPECTION_EMPLOYEES);
    int* ptr1 = get_shared_memory_variable_ptr(SHARED_MEMORY_AVAILABLE_INSPECTION_EMPLOYEES);
    (*ptr1) = NUM_OF_INSPECTION_EMPLOYEES;
    initialize_shared_memory_variable(SHARED_MEMORY_AVAILABLE_PACKAGING_EMPLOYESS);
    int* ptr2 = get_shared_memory_variable_ptr(SHARED_MEMORY_AVAILABLE_PACKAGING_EMPLOYESS);
    (*ptr2) = NUM_OF_PACKAGING_EMPLOYEES;
}
