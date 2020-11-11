/*
Generates sample graph using Random Edge Algorithm
No multi-threading
*/

//#define __STDC_FORMAT_MACROS
#define N 65000//25000//65000
#define FILE_NAME "fb_10.txt"

#include <err.h>
#include <fcntl.h>
#include <inttypes.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <malloc.h>

#include <stdbool.h>


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <math.h>
#include <pthread.h>
#include <sys/time.h>
//#include <stdbool.h>

#include <map> 

#include <gem5/m5ops.h>

#define PAGE    4096
#define BLOCK   64

//pthread_mutex_t lock1;
//pthread_mutex_t lock2;

int timeval_subtract(struct timeval *result, struct timeval *x, struct timeval *y);
int getRandomIndex(int upperbound);
struct Graph* createGraph(struct Edge edges[], uint64_t n);
void printGraph(struct Graph* graph);
uint64_t getTotalLines(char* filename);
struct Edge* loadEdges(char* filename);
struct Graph* func_sampling(struct Graph* originalGraph, float fraction);



uint64_t total_edges;

// REJECTION set to 0 // CHANGE THIS****/
int enable_rejection = 1;
int num_threads = 0;

struct Graph {
	struct Node* head[N];
	struct Edge* edges;
	uint64_t numberOfEdges;
};

struct result_s
{
    int skiped;
    int rejected;
    int processed;
};

struct Node {
	uint64_t dest;
	struct Node* next;
};

struct Edge {
	uint64_t src, dest;
};

struct Graph *originalGraph;

void reset_stats(uint64_t t1, uint64_t t2)
{
    __asm__ __volatile__ (".byte 0x0F, 0x04; .word 0x40;");
}

void dump_stats(uint64_t t1, uint64_t t2)
{
    __asm__ __volatile__ (".byte 0x0F, 0x04; .word 0x41;");
}

void dump_reset_stats()
{
    __asm__ __volatile__ (".byte 0x0F, 0x04; .word 0x42;");
}

void gem5(uint64_t * pointer)
{
    __asm__ __volatile__ (".byte 0x0F, 0x04; .word 0x55;");
}

void gem5_clean(Edge * pointer)
{
    __asm__ __volatile__ (".byte 0x0F, 0x04; .word 0x56;");
}

//void *thread(void *args) 
void testThread()
{
    /*struct result_s *result = (struct result_s *)args;
    result->skiped = 0;
    result->rejected = 0;
    result->processed = 0;*/

    float fraction = 0.1;

    /*for(int i = 0; i < size; i++) {
        if ( i % 0x100 == 0 )
            result->skiped++;
        else if ( *(data + i) == 0 )
            result->rejected++;
        else
            result->processed++;
    }*/


    /** STARTING GRAPH SAMPLING **/
    uint64_t preferredEdgeSize = total_edges * fraction;
	int sampledEdgeIndex=0;

	//pthread_mutex_lock(&lock1);
	struct Graph *sampleGraph = NULL;
	printf("Func sampling started\n");

	//int outrange = 0;
	int exits=0, nonexists=0;

	
	// Create the sampled Edge list
	struct Edge* sampledEdges = (struct Edge*) malloc(preferredEdgeSize * sizeof(struct Edge));
	
	if(sampledEdges == NULL){

		printf("ERROR in allocating memory for sampled edges. Exiting the execution.\n");
		exit(0);
	}

	printf("Edge loaded\n");
	//pthread_mutex_unlock(&lock);
	//pthread_mutex_unlock(&lock1); 

	//printf("Intermediary unlock\n");
	//pthread_mutex_lock(&lock2);
	//printf("lock again\n");


	// Map to track which indexes already been sampled
	std::map<int, int> sampledIndexMap;
	printf("Sample index map created\n");

	while(sampledEdgeIndex < preferredEdgeSize) {
		printf("Inside loop: %d\n", sampledEdgeIndex);

		int randomIndex = getRandomIndex(total_edges);

		printf("random index: %d\n", randomIndex);

		//Check if this randomIndex is already been sampled, if not then take this edge
		if(sampledIndexMap.find(randomIndex)->second == 0)
		{
			// CHECK if the index is set to zero
			if(originalGraph->edges[randomIndex].src == 0){
				//result->rejected++;
				printf("ZERO: %d\n", randomIndex);
				continue;
			}

			//printf("Found\n");
			printf("src: %lu\n", originalGraph->edges[randomIndex].src);

			sampledEdges[sampledEdgeIndex].src = originalGraph->edges[randomIndex].src;
			sampledEdges[sampledEdgeIndex].dest = originalGraph->edges[randomIndex].dest;

			sampledEdgeIndex++;

			// Add the random index into the map
			sampledIndexMap.insert(std::pair<int,int>(randomIndex, 1));
			nonexists++;
			//result->processed++;
		}
		else {
			printf("Not found\n");
			//printf("Exists %lu\n", randomIndex);
			exits++;
			continue;
		}

		//if(sampledEdgeIndex % 100 == 0)
			//printf("Sample edge: %d\n", sampledEdgeIndex );
		//printf("%lu %d\n", randomIndex, sampledEdgeSize );


		
	}
	//printf("DONE exists: %d nonexists: %d\n", exits, nonexists );

	// Print the sampled Edges
	/*int x;
	for(x = 0; x < preferredEdgeSize; x++){

		int src = sampledEdges[x].src;
		int dest = sampledEdges[x].dest;
		printf("s: %d %d\n",src, dest );
	} */

	//printf("\nCALLaed %lu %lu fraction %f\n", total_edges, preferredEdgeSize, fraction);
	
	// construct graph from given edges
	if(sampledEdges != NULL)
	{
		sampleGraph = createGraph(sampledEdges, preferredEdgeSize);
	}

	//pthread_mutex_unlock(&lock1);
	//printf("Unlocked\n");

	//printf("Func sampling ended\n");

    /** ENDING GRAPH SAMPLING **/

    /*int x = 0;

	for(x = 0; x < originalGraph->numberOfEdges; x++){

		uint64_t src = originalGraph->edges[x].src;
		//uint64_t dest = graph->edges[x].dest;
		//printf("@#: %lu %lu\n",src, dest);

		if ( x % 0x100 == 0 )
            result->skiped++;
        else if ( src == 0 )
            result->rejected++;
        else
            result->processed++;
	}*/

	printf("Thread printed\n");
}

int timeval_subtract(struct timeval *result, struct timeval *x, struct timeval *y)
{
    /* Perform the carry for the later subtraction by updating Y. */
    if (x->tv_usec < y->tv_usec)
    {
        int nsec = (y->tv_usec - x->tv_usec) / 1000000 + 1;
        y->tv_usec -= 1000000 * nsec;
        y->tv_sec += nsec;
    }
    if (x->tv_usec - y->tv_usec > 1000000)
    {
        int nsec = (x->tv_usec - y->tv_usec) / 1000000;
        y->tv_usec += 1000000 * nsec;
        y->tv_sec -= nsec;
    }

    /* Compute the time remaining to wait.
       `tv_usec' is certainly positive. */
    result->tv_sec = x->tv_sec - y->tv_sec;
    result->tv_usec = x->tv_usec - y->tv_usec;

    /* Return 1 if result is negative. */
    return x->tv_sec < y->tv_sec;
}



// Returns random number upperbound - 1
int getRandomIndex(int upperbound){
	int num = rand() % upperbound;//(upperbound + 1);

	return num;	
}

struct Graph* createGraph(struct Edge edges[], uint64_t n){
	uint64_t i;
	struct Graph* graph = (struct Graph*) malloc(sizeof(struct Graph));

	for(i=0; i < N; i++){
		graph->head[i] = NULL;
	}

	for(i=0; i < n; i++){
		struct Node* newNode = (struct Node*) malloc(sizeof(struct Node));
		
		newNode->dest = edges[i].dest;

		newNode->next = graph -> head[edges[i].src];
		graph -> head[edges[i].src] = newNode;
	}

	graph -> edges = edges;
	graph->numberOfEdges = n;
	
	return graph;
}
 
// Function to print adjacency list representation of graph
void printGraph(struct Graph* graph)
{
	printf("Printing started\n");
	uint64_t i;
	for (i = 0; i < N; i++)
	{
		if(graph->head[i] != NULL)
		{	//printf("NULL\n");
			// print current vertex and all ts neighbors
			struct Node* ptr = graph->head[i];
			while (ptr != NULL)
			{
				//printf("(%lu -> %lu)\t", i, ptr->dest);
				ptr = ptr->next;


			}

			if(i% 1000 == 0)
					printf("i = %lu\n", i);

			//printf("\n");
		}
	}
	printf("Printing ends\n");
}

uint64_t getTotalLines(char* filename){
	printf("Reading Total lines\n");
	FILE* file = fopen(filename, "r"); // should check the result 
	char line[256];

	long max = -100;
	
	if(file == NULL)
		printf("NULL\n");
	
	uint64_t j = 0;

	while (fgets(line, sizeof(line), file)) {

		char *token = strtok(line, "\t"); 
	
		
		while (token != NULL) 
		{ 
			
			long tk = atoi(token);
			if (tk > max)
				max = tk;
			
			token = strtok(NULL, " "); 
			//k++;
		} 

		j++;
	}
	printf("Read Completed!! TOTAL j: %lu and Max: %ld\n", j, max);

	fclose(file);
	return j;

}

struct Edge* loadEdges(char* filename){



	/*struct Edge edges1[] =
	{
		/*{ 0, 1 }, { 1, 2 }, { 2, 0 }, { 2, 1 }, {2,11}, {2,55},
		{ 3, 2 }, { 4, 5 }, { 5, 4 }, {3,11}
		{3,7}, {5,3}, {5,7}, {7,5}, {9,5}, {11,13},{13,11}
	};*/

	total_edges = getTotalLines(filename);//24;
	
	printf("TOTAL EDGE: %lu \n", total_edges);
	
	struct Edge* edges1 = (struct Edge*) malloc(total_edges * sizeof(struct Edge));
	
	if(edges1 == NULL){

		printf("ERROR in allocating memory for edges. Exiting the execution.\n");
		exit(0);
	}


	printf("##total_edges %lu\n", total_edges);
	FILE* file = fopen(filename, "r"); // should check the result 
	char line[512];
	
	if(file == NULL)
		printf("NULL\n");
	
	uint64_t j = 0;

	while (fgets(line, sizeof(line), file)) {
		//printf("#aa%s@", line); 

		//printf("dfs\n");
		char *token = strtok(line, "\t"); 
	
		int k=0;
		while (token != NULL) 
		{ 	
			if(k==0){
				//printf(" ");
				//printf("SRC = %s\n", token); 
				edges1[j].src = atoi(token);
			} else if(k==1){
				//printf("g");
				//printf("DEST = %s\n", token); 
				edges1[j].dest = atoi(token);
			}
			
			token = strtok(NULL, " "); 
			k++;
		} 

		if(j %10000 == 0)
			printf("Read line: %lu\n", j);

		j++;
		//printf("@@ %lu\n", j);


		/*if(token != NULL){
			printf("@@%d", atoi(token));
			//edges1[i].src = atoi(token);
		
			token = strtok(NULL, " ");
			printf("$%d", atoi(token));
			//edges1[i].dest = atoi(token);
			j++;
		}*/
	}
	//printf("$ggggj: ");
	/* may check feof here to make a difference between eof and io failure -- network
	timeout for instance */

	fclose(file);

	return edges1;
}

struct Graph* func_sampling(struct Graph* originalGraph, float fraction) 
{
	

	uint64_t preferredEdgeSize = total_edges * fraction;
	int sampledEdgeIndex=0;

	struct Graph *sampleGraph = NULL;
	printf("Func sampling started\n");

	//int outrange = 0;
	int exits=0, nonexists=0;

	// Create the sampled Edge list
	struct Edge* sampledEdges = (struct Edge*) malloc(preferredEdgeSize * sizeof(struct Edge));
	
	if(sampledEdges == NULL){

		printf("ERROR in allocating memory for sampled edges. Exiting the execution.\n");
		exit(0);
	}

	printf("Edge loaded\n");


	// Map to track which indexes already been sampled
	std::map<int, int> sampledIndexMap;
	printf("Sample index map created\n");

	while(sampledEdgeIndex < preferredEdgeSize) {
		//printf("Inside loop: %d\n", sampledEdgeIndex);

		int randomIndex = getRandomIndex(total_edges);

		//printf("random index: %d\n", randomIndex);

		//Check if this randomIndex is already been sampled, if not then take this edge
		if(sampledIndexMap.find(randomIndex)->second == 0)
		{
			//printf("Found\n");
			printf("src: %lu\n", originalGraph->edges[randomIndex].src);

			sampledEdges[sampledEdgeIndex].src = originalGraph->edges[randomIndex].src;
			sampledEdges[sampledEdgeIndex].dest = originalGraph->edges[randomIndex].dest;

			sampledEdgeIndex++;

			// Add the random index into the map
			sampledIndexMap.insert(std::pair<int,int>(randomIndex, 1));
			nonexists++;
		}
		else {
			//printf("Not found\n");
			//printf("Exists %lu\n", randomIndex);
			exits++;
			continue;
		}

		if(sampledEdgeIndex % 100 == 0)
			printf("Sample edge: %d\n", sampledEdgeIndex );
		//printf("%lu %d\n", randomIndex, sampledEdgeSize );


		
	}
	printf("DONE exists: %d nonexists: %d\n", exits, nonexists );

	// Print the sampled Edges
	/*int x;
	for(x = 0; x < preferredEdgeSize; x++){

		int src = sampledEdges[x].src;
		int dest = sampledEdges[x].dest;
		printf("s: %d %d\n",src, dest );
	}*/

	//printf("\nCALLaed %lu %lu fraction %f\n", total_edges, preferredEdgeSize, fraction);
	
	// construct graph from given edges
	if(sampledEdges != NULL)
	{
		sampleGraph = createGraph(sampledEdges, preferredEdgeSize);
	}

	printf("Func sampling ended\n");
	
	return sampleGraph;
}

// Directed Graph Implementation in C
int main(int argc, char *argv[])
{
	struct timeval timeval1, timeval2, timeval_result;

	/*if(argc <= 3) {
        printf("%s <input file name> <number of threads> <enable rejection 0,1>\n", argv[0]);
        return -1;
    }*/

    num_threads = 4;//atoi(argv[2]);

    enable_rejection = 0;//atoi(argv[3]);

    pthread_t threads[num_threads];
    struct result_s results[num_threads];
    

	char filename[] = FILE_NAME;
	//char *filename = FILE_NAME;//"fb_10.txt";//argv[1];//

	printf("Enable rejection: %d Num of threads: %d and input file is: %s\n", enable_rejection, num_threads, filename );

	// Use current time as seed for random generator 
    srand(time(0));

	struct Edge* edges = loadEdges(filename);

	// calculate number of edges
	uint64_t n = total_edges;//sizeof(edges)/sizeof(edges[0]);
	//printf("N: %d\n", n);

	printf("EDGE LOADED:\n");
	// construct graph from given edges
	//struct Graph *originalGraph = createGraph(edges, n);
	originalGraph = createGraph(edges, n);

	// Enable rejection to the original graph
	printf("Rejection started\n");
	
	printf("Edge structure size: %lu\n", sizeof(originalGraph->edges[0]));

	int block_new = BLOCK / sizeof(originalGraph->edges[0]);
	printf("New block size: %d\n", block_new);

	if ( enable_rejection )
    {
        for(int i = 0; i < originalGraph->numberOfEdges; i += block_new) {
            gem5(&originalGraph->edges[i].src);
            //printf("##%lu\n", graph->edges[i].src);
        }
    }

    printf("Rejection ends\n");

    // Create and the threads
    gettimeofday(&timeval1, NULL);
    reset_stats(0,0);

    //m5_reset_stats(0, 0);
    //__asm__ __volatile__ (".byte 0x0F, 0x04; .word 0x40;");

    for(int i = 0; i < num_threads; i++) {
    	printf("Thread creating %d\n", i);
        //pthread_create(threads + i, NULL, thread, (void *)(results + i));

        // call sampling function
        testThread();
        printf("Thread created %d\n", i);
    }

    /*for(int i = 0; i < num_threads; i++) {
    	printf("test\n");
        pthread_join(threads[i], NULL);
        printf("test1\n");
    }*///// CLOSE HERE

    dump_stats(0,0);
    //m5_dump_stats(0, 0);
    //__asm__ __volatile__ (".byte 0x0F, 0x04; .word 0x41;");
    gettimeofday(&timeval2, NULL);

    for(int i = 0; i < num_threads; i++) {
        printf("thread %d - rejected %d processed %d\n", i, results[i].rejected, results[i].processed);
    }

    timeval_subtract(&timeval_result, &timeval2, &timeval1);

    printf("runtime: %lu.%lu seconds.\n", timeval_result.tv_sec, timeval_result.tv_usec);

    // Edge list of the original graph
    /*int x = 0;

	for(x = 0; x < originalGraph->numberOfEdges; x++){

		uint64_t src = originalGraph->edges[x].src;
		uint64_t dest = graph->edges[x].dest;
		printf("@#: %lu %lu\n",src, dest);
	}*/

	//struct Graph *sampledGraph = func_sampling(originalGraph, 0.1);

	// Uncomment to print the edge list of the sampled graph
	/*int x = 0;

	for(x = 0; x < sampledGraph->numberOfEdges; x++){

		int src = sampledGraph->edges[x].src;
		int dest = sampledGraph->edges[x].dest;
		printf("#: %d %d\n",src, dest );
	}

	printf("Total Printed sample edge: %lu\n", sampledGraph -> numberOfEdges );
	//// CLOSE HERE

	//M5 Reset Stats
	//__asm__ __volatile__ (".byte 0x0F, 0x04; .word 0x40;");

	
	//printGraph(sampledGraph);


	// M5 Dump Stats
	//__asm__ __volatile__ (".byte 0x0F, 0x04; .word 0x41;");

	// print adjacency list representation of graph
	//printGraph(graph);

	if ( enable_rejection )
        gem5_clean(originalGraph->edges);

	printf("Freeing objects\n");
	
	free(originalGraph);
	printf("originalGraph freed\n");
	free(edges);


	*/
	printf("edges freed\n");
	//free(sampledGraph);

	

    

	return 0;
}