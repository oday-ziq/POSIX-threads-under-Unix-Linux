#include "header.h"
#include "constants.h"
#include "functions.h"
void* inspection(char* arg);
void* packaging(char* arg);
void produce_medicine(char* type);
int check_exit_condition();
void check_termination_condition(char* type);
// Production Line is specified by the type of medicine it produces
sem_t mutex;
int number_of_medicine_produced = 0;
int number_of_medicine_inspected = 0;
int number_of_medicine_packaged = 0;
int position = 0;
int production_line_id = 0;
// #######  Note  : Data Segement is shared between threads #############
struct liquid_medicine {
    // Binary valuse to indicate if this prop cleanerty is acceptable or not
    int id;
    int level_inside_bottle;
    int color;
    int label;
    int expiry_date;
    int type_index;
    char type[50];
};
struct pills_medicine {
    int id;
    int missing_any_pill;
    int color;
    int size;
    int expiry_date;
    int type_index;
    char type[50];
};
// array of liquid medicine
struct liquid_medicine liquid_medicines_array_produced[100];
// array of pills medicine
struct pills_medicine pills_medicines_array_produced[100];

//array to store the acceptable medicine 
// TODO : Used to check termination_condition based on the type
struct liquid_medicine liquid_medicines_array_accpeted[100];
// array to store the acceptable pills medicine
struct pills_medicine pills_medicines_array_accpeted[100];

int main(int argc, char* argv[]) {
    if (argc != 3) {
        perror("Invalid number of arguments\n");
        exit(-1);
    }
    sem_init(&mutex, 0, 1);
    // sem_init(&mutex_p, 0, 1);
    readFile("args.txt");

    char* type = argv[1];
    char* index = argv[2];
    printf("Index: %s\n", index);
    production_line_id = atoi(index) + 1;
    // TODO : get the index of the production line
   //id for the production line
    printf("Type: %s\n", type);
    printf("Production Line %d created\n", getpid());
    //prodecing & inspection & packaging employees are threads
    pthread_t produce_medicine_thread, inspection_thread, packaging_thread;
    // create threads
    pthread_create(&produce_medicine_thread, NULL, produce_medicine, (char*)type);
    pthread_create(&inspection_thread, NULL, inspection, (char*)type);
    pthread_create(&packaging_thread, NULL, packaging, (char*)type);
    // wait for threads to finish
    //pthread_join(produce_medicine_thread, NULL);
    pthread_join(inspection_thread, NULL);
    pthread_join(packaging_thread, NULL);
    // destroy the semaphores
    sem_destroy(&mutex);
    // cancel the threads
    printf("Canceling threads\n");
    pthread_cancel(inspection_thread);
    pthread_cancel(packaging_thread);
    pthread_cancel(produce_medicine_thread);

    return 0;
}
void* inspection(char* type) {
    // Will check if the produced medicine is acceptable or not
    
    while (1)
    {
        sleep(1);
        int *ptr = get_shared_memory_variable_ptr(SHARED_MEMORY_AVAILABLE_INSPECTION_EMPLOYEES);
        if((*ptr) == 0){
            continue;
        }
        (*ptr)--;
        int exit_condition = check_exit_condition();
        check_termination_condition(type);
        if (exit_condition == 1)
        {
            printf("Inspection Thread is done\n");
            // cancel the thread
            pthread_exit(NULL);
        }

        if (number_of_medicine_produced != 0) {
            printf(" ### number_of_%s_medicine_produced is : %d by production line %d \n", type, number_of_medicine_produced, production_line_id);
            sleep(get_random_sleep_value());
            printf("Inspection Employee is working\n");
            sleep(get_random_sleep_value());
            if (strcmp(type, "liquid") == 0) {
                //check the medicine properties if all ones 
                printf("******************************************\n");
                struct  liquid_medicine parameters = liquid_medicines_array_produced[number_of_medicine_produced - 1];
                // call print_liquid_medicine function to print the liquid medicine properties
                printf("Liquid Medicine produced by  by production line %d \n", production_line_id);
                print_liquid_medicine(parameters);
                printf("******************************************\n");
                fflush(stdout);
                if (parameters.id == 1 && parameters.level_inside_bottle >= LIQUID_LEVEL_ACCEPTED_RANGE_MIN && parameters.level_inside_bottle <= LIQUID_LEVEL_ACCEPTED_RANGE_MAX && parameters.color == 1 && parameters.label == 1 && parameters.expiry_date == 1)
                {
                    printf("Medicine is acceptable in production Line %d \n", production_line_id);
                    sem_wait(&mutex);
                    int* ptr = get_shared_memory_array_variable_ptr(SHARED_MEMORY_LIQUID_MEDICINE_ARRAY_COUNT, NUM_TYPES);
                    ptr[parameters.type_index]++;
                    check_if_produce_type_more_than_threshould(parameters.type);
                    number_of_medicine_inspected++;
                    sem_post(&mutex);
                    liquid_medicines_array_accpeted[position] = parameters;
                    position++;
                }
                else
                {
                    printf("Medicine is not acceptable it will be discarded in production Line %d \n", production_line_id);
                    int* ptr = get_shared_memory_variable_ptr(SHARED_MEMORY_DESCARDED_BOTTLE_MEDICINE_COUNT);
                    (*ptr) += 1;
                }
            }
            else if (strcmp(type, "pills") == 0) {
                //check the medicine properties if all ones
                printf("******************************************\n");
                struct  pills_medicine parameters = pills_medicines_array_produced[number_of_medicine_produced - 1];
                // call print_pills_medicine function to print the pills medicine properties
                print_pills_medicine(parameters);
                printf("******************************************\n");
                fflush(stdout);
                if (parameters.id == 1 && parameters.missing_any_pill == 1 && parameters.color == 1 && parameters.size == 1 && parameters.expiry_date == 1)
                {
                    printf("Medicine is acceptable in production Line\n", production_line_id);
                    sem_wait(&mutex);
                    int* ptr = get_shared_memory_array_variable_ptr(SHARED_MEMORY_PILL_MEDICINE_ARRAY_COUNT, NUM_TYPES);
                    ptr[parameters.type_index]++;
                    check_if_produce_type_more_than_threshould(type);
                    number_of_medicine_inspected++;
                    sem_post(&mutex);
                    pills_medicines_array_accpeted[position] = parameters;
                    position++;
                }
                else
                {
                    printf("Medicine is not acceptable it will be discarded in production Line %d \n", production_line_id);
                    int* ptr = get_shared_memory_variable_ptr(SHARED_MEMORY_DESCARDED_PILL_MEDICINE_COUNT);
                    (*ptr) += 1;
                }

            }
            printf("Inspection Employee is done\n");
        }
        (*ptr)++;

    }
}
void* packaging(char* type) {

    while (1)
    {
        sleep(1);
        int *ptr = get_shared_memory_variable_ptr(SHARED_MEMORY_AVAILABLE_PACKAGING_EMPLOYESS);
        if((*ptr) == 0){
            continue;
        }
        (*ptr)--;
        if (number_of_medicine_inspected != 0) {
            printf("Packaging Employee is working\n");
            sleep(get_random_sleep_value());
            sem_wait(&mutex);
            number_of_medicine_inspected--;;
            sem_post(&mutex);
            number_of_medicine_packaged++;
            //if type is liquid
            printf("Packaging Employee  is done\n");
            printf(" number_of_%s_medicine_packaged is : %d by production line %d \n", type, number_of_medicine_packaged, production_line_id);
            int exit_condition = check_exit_condition();
            if (exit_condition == 1)
            {
                printf("Production Line is done\n");
                pthread_exit(NULL);

            }
        }
        (*ptr)++;
    }
}

void produce_medicine(char* type) {
    printf("Production Employee %d is working in by production line %d \n ", getpid(), production_line_id);
    // printf("Type: %s\n", (char *)type);
    while (1)
    {
        int exit_condition = check_exit_condition();
        if (exit_condition == 1)
        {
            printf("Produce Medicine Thread is done\n");
            pthread_exit(NULL);
        }

        // check if the type is liquid
        if (strcmp(type, "liquid") == 0) {
            struct liquid_medicine parameters;
            srand(time(NULL) ^ getpid());
            double probability_of_one = 0.95;
            // 95% chance of getting 1
            parameters.id = random_0_or_1(probability_of_one, getpid());
            parameters.level_inside_bottle = random_range(LIQUID_LEVEL_GENERATED_MIN, LIQUID_LEVEL_GENERATED_MAX, getpid());
            parameters.color = random_0_or_1(probability_of_one, getpid());
            parameters.label = random_0_or_1(probability_of_one, getpid());
            parameters.expiry_date = random_0_or_1(probability_of_one, getpid());

            sleep(get_random_sleep_value());
                    // get random liquid medicine type
            int medicine_produced_type = rand() % NUM_TYPES;
            int* ptr = get_shared_memory_array_variable_ptr(SHARED_MEMORY_LIQUID_MEDICINE_ARRAY_COUNT, NUM_TYPES);
            ptr[medicine_produced_type]++;
            check_if_produce_type_more_than_threshould(type);
            strcpy(parameters.type, get_liquid_medicine_from_type(medicine_produced_type));
            parameters.type_index = medicine_produced_type;
            liquid_medicines_array_produced[number_of_medicine_produced] = parameters;
        }
        else if (strcmp(type, "pills") == 0) {
            struct pills_medicine parameters;
            srand(time(NULL) ^ getpid());
            double probability_of_one = 0.95;
            // 95% chance of getting 1
            parameters.id = random_0_or_1(probability_of_one, getpid());
            parameters.missing_any_pill = random_0_or_1(probability_of_one, getpid());
            parameters.color = random_0_or_1(probability_of_one, getpid());
            parameters.size = random_0_or_1(probability_of_one, getpid());
            parameters.expiry_date = random_0_or_1(probability_of_one, getpid());
            sleep(get_random_sleep_value());
            int* ptr = get_shared_memory_array_variable_ptr(SHARED_MEMORY_PILL_MEDICINE_ARRAY_COUNT, NUM_TYPES);
            int medicine_produced_type = rand() % NUM_TYPES;
            ptr[medicine_produced_type]++;
            check_if_produce_type_more_than_threshould(type);
            // get random pill medicine type
            strcpy(parameters.type, get_pill_medicine_from_type(medicine_produced_type));
            parameters.type_index = medicine_produced_type;
            // store the pills medicine in the array
            pills_medicines_array_produced[number_of_medicine_produced] = parameters;
        }
        sleep(get_random_sleep_value());
        number_of_medicine_produced++;
    }
}
int check_exit_condition() {
    // check if the number of medicine accepted is equal to the number of different medicines per production line
    if (number_of_medicine_packaged >= NUMBER_OF_DIFFERENT_MEDICINES_PER_PRODUCTION_LINE)
    {
        return 1;
    }
    return 0;
}
void check_termination_condition(char* type) {
    if (strcmp(type, "liquid") == 0) {
        int* ptr = get_shared_memory_variable_ptr(SHARED_MEMORY_DESCARDED_BOTTLE_MEDICINE_COUNT);
        if ((*ptr) >= MAX_ALLAWOD_NUMBER_OF_BOTTLED_OUTOF_SPEC)
        {
            // send signal to the parent  to end the simulation
            kill(getppid(), SIGUSR1);
        }
    }
    else if (strcmp(type, "pills") == 0) {
        int* ptr = get_shared_memory_variable_ptr(SHARED_MEMORY_DESCARDED_PILL_MEDICINE_COUNT);
        if ((*ptr) >= MAX_ALLAWOD_NUMBER_OF_BILL_OUTOF_SPEC)
        {
            // send signal to the parent  to end the simulation
            kill(getppid(), SIGUSR1);
        }
    }
}
void check_if_produce_type_more_than_threshould(char* type) {

    // TODO  :Check if the number of discarded medicine is more than the threshold 
    // stored in  liquid_medicines_array_accpeted
    if (strcmp(type, "liquid") == 0) {
        //const char *liquid_medicine_types[] = {"Cough Syrup", "Antibiotics", "Antiseptics", "Antacids", "Laxatives"};
        int* ptr = get_shared_memory_array_variable_ptr(SHARED_MEMORY_LIQUID_MEDICINE_ARRAY_COUNT, NUM_TYPES);
        for (int i = 0; i < NUM_TYPES; i++) {
            if (ptr[i] >= MAXIMUM_NUMBER_ALLOWED_FOR_TYPE) {
                kill(getppid(), SIGUSR1);
            }
        }

    }
    else if (strcmp(type, "pills") == 0) {
        int* ptr = get_shared_memory_array_variable_ptr(SHARED_MEMORY_PILL_MEDICINE_ARRAY_COUNT, NUM_TYPES);
        for (int i = 0; i < NUM_TYPES; i++) {
            if (ptr[i] >= MAXIMUM_NUMBER_ALLOWED_FOR_TYPE) {
                kill(getppid(), SIGUSR1);
            }
        }
    }


}
void print_liquid_medicine(struct liquid_medicine parameters) {
    printf("id: %d\n", parameters.id);
    printf("level_inside_bottle: %d\n", parameters.level_inside_bottle);
    printf("color: %d\n", parameters.color);
    printf("label: %d\n", parameters.label);
    printf("expiry_date: %d\n", parameters.expiry_date);
    printf("type: %s\n", parameters.type);
}
void print_pills_medicine(struct pills_medicine parameters) {
    printf("id: %d\n", parameters.id);
    printf("missing_any_pill: %d\n", parameters.missing_any_pill);
    printf("color: %d\n", parameters.color);
    printf("size: %d\n", parameters.size);
    printf("expiry_date: %d\n", parameters.expiry_date);
    printf("type: %s\n", parameters.type);
}
