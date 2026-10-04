//David Ramos

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

typedef struct {
	int id;
	int result;
} WorkerData;

void *worker_function(void *argument) {
    WorkerData *data = argument;

    printf("[thread %d] started.\n", data->id);

    data->result = data->id * data->id;

    return NULL;
}

int main(int argc, char *argv[]){

	char *unconverted_input;
	long converted_pthread_count;
	if(argc != 2){
		fprintf(stderr, "%s <thread_count>\n", argv[0]);
		return EXIT_FAILURE;		
	}

    /*
     * Convert argv[1], which is a string, into a number.
     * The 10 means interpret the string as a base-10 number.

	 * "25" becomes 25
	//long int strtol(const char *str, char **endptr, int base)
    //str − It is a string to be converted into a long integer value.

	//endptr − It is a pointer to a charter pointer and used to store the pointer to the first character after the numeric value.

	//base − It represents the base of the number system(e.g., 10 for decimal and 16 for hexadecimal).*/
    converted_pthread_count = strtol(argv[1], &unconverted_input, 10);

	/*
     * endptr points to the first character that strtol()
     * could not convert.  
     * "5"     -> endptr points to '\0'       valid
     * "5abc"  -> endptr points to 'a'        invalid
     * "abc"   -> no number was converted     invalid
     */
	if(unconverted_input == argv[1] ||
		*unconverted_input != '\0' ||
		converted_pthread_count < 1 ||
		converted_pthread_count > 100){
			fprintf(stderr, "thread count must be a positive integer between 1-100.\n");
			return EXIT_FAILURE;
		}

	int thread_count = (int)converted_pthread_count;

	printf("Creating %d threads.\n", thread_count);

	pthread_t *threads = malloc(thread_count * sizeof(*threads));

	WorkerData *worker_data = malloc(thread_count * sizeof(*worker_data));

	if (threads == NULL || worker_data == NULL) {
		fprintf(stderr, "Error: memory allocation failed.\n");
		free(threads);
		free(worker_data);
		return EXIT_FAILURE;
	}

	int created_count = 0;

	for (int i = 0; i < thread_count; i++) {
		worker_data[i].id = i + 1;

		int create_status = pthread_create(&threads[i],NULL,worker_function,&worker_data[i]);

		if (create_status != 0) {
			fprintf(stderr, "Error: could not create thread %d.\n", worker_data[i].id);
			
			//joing successfully created threads before freeing shared memory
			for (int j = 0; j < created_count; j++) {
            	pthread_join(threads[j], NULL);
        	}
			free(threads);
			free(worker_data);
			return EXIT_FAILURE;
		}
		created_count++;
	}

	int join_failed = 0;

	for (int i = 0; i < thread_count; i++) {
		int join_status = pthread_join(threads[i], NULL);

		if (join_status != 0) {
			fprintf(stderr, "Error: could not join thread %d.\n", worker_data[i].id);

			join_failed = 1;
		}
	}

	if (join_failed) {
		free(threads);
		free(worker_data);
		return EXIT_FAILURE;
	}

	for (int i = 0; i < thread_count; i++) {
		printf("Thread %d result: %d\n", worker_data[i].id, worker_data[i].result);
	}
	
	printf("All threads completed successfully.\n");
	
	free(threads);
	free(worker_data);

	return EXIT_SUCCESS;

}
