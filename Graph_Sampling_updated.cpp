//#define __STDC_FORMAT_MACROS
#define N 65000

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <math.h>

#include <map> 


uint64_t total_edges;

struct Graph {
	struct Node* head[N];
	struct Edge* edges;
	uint64_t numberOfEdges;
};


struct Node {
	uint64_t dest;
	struct Node* next;
};

struct Edge {
	uint64_t src, dest;
};

int getRandomIndex(int upperbound);
struct Graph* createGraph(struct Edge edges[], uint64_t n);
void printGraph(struct Graph* graph);
uint64_t getTotalLines(char* filename);
struct Edge* loadEdges(char* filename);



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
	uint64_t i;
	for (i = 0; i < N; i++)
	{
		if(graph->head[i] != NULL)
		{	//printf("NULL\n");
			// print current vertex and all ts neighbors
			struct Node* ptr = graph->head[i];
			while (ptr != NULL)
			{
				printf("(%lu -> %lu)\t", i, ptr->dest);
				ptr = ptr->next;
			}

			printf("\n");
		}
	}
}

uint64_t getTotalLines(char* filename){
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
	printf("$TOTAL j: %lu and Max: %ld\n", j, max);

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


	printf("##total_edges %lu", total_edges);
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
		{ 	if(k==0){
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

	//int outrange = 0;
	//int exits=0, nonexists=0;

	// Create the sampled Edge list
	struct Edge* sampledEdges = (struct Edge*) malloc(preferredEdgeSize * sizeof(struct Edge));
	
	if(sampledEdges == NULL){

		printf("ERROR in allocating memory for sampled edges. Exiting the execution.\n");
		exit(0);
	}

	// Map to track which indexes already been sampled
	std::map<uint64_t, int> sampledIndexMap;

	while(sampledEdgeIndex < preferredEdgeSize) {

		uint64_t randomIndex = getRandomIndex(total_edges);

		//Check if this randomIndex is already been sampled, if not then take this edge
		if(sampledIndexMap.find(randomIndex)->second == 0)
		{
			sampledEdges[sampledEdgeIndex].src = originalGraph->edges[randomIndex].src;
			sampledEdges[sampledEdgeIndex].dest = originalGraph->edges[randomIndex].dest;

			sampledEdgeIndex++;

			// Add the random index into the map
			sampledIndexMap.insert(std::pair<uint64_t,int>(randomIndex, 1));
			//nonexists++;
		}
		else {
			//printf("Exists %lu\n", randomIndex);
			//exits++;
			continue;
		}

		//printf("%lu %d\n", randomIndex, sampledEdgeSize );


		
	}
	//printf("DONE %d %d\n", exits, nonexists );

	// Print the sampled Edges
	/*int x;
	for(x = 0; x < preferredEdgeSize; x++){

		int src = sampledEdges[x].src;
		int dest = sampledEdges[x].dest;
		printf("s: %d %d\n",src, dest );
	}*/

	printf("\nCALLaed %lu %lu fraction %f\n", total_edges, preferredEdgeSize, fraction);
	
	// construct graph from given edges
	if(sampledEdges != NULL)
	{
		sampleGraph = createGraph(sampledEdges, preferredEdgeSize);
	}
	
	return sampleGraph;
}

// Directed Graph Implementation in C
int main(void)
{

	char filename[] = "fb.txt";

	// Use current time as seed for random generator 
    srand(time(0));

	struct Edge* edges = loadEdges(filename);

	// calculate number of edges
	uint64_t n = total_edges;//sizeof(edges)/sizeof(edges[0]);
	//printf("N: %d\n", n);

	// construct graph from given edges
	struct Graph *graph = createGraph(edges, n);
	

	struct Graph *sampledGraph = func_sampling(graph, 0.3);

	// Uncomment to print the edge list of the sampled graph
	/*int x = 0;

	for(x = 0; x < sampledGraph->numberOfEdges; x++){

		int src = sampledGraph->edges[x].src;
		int dest = sampledGraph->edges[x].dest;
		printf("#: %d %d\n",src, dest );
	}

	printf("Total Printed sample edge: %lu\n", sampledGraph -> numberOfEdges );
	*/

	printGraph(sampledGraph);

	// print adjacency list representation of graph
	//printGraph(graph);

	free(edges);
	free(graph);

	return 0;
}