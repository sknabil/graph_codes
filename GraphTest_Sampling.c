//#define __STDC_FORMAT_MACROS
#define N 65000

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>


uint64_t total_edges;

struct Graph {
	struct Node* head[N];
};


struct Node {
	uint64_t dest;
	struct Node* next;
};

struct Edge {
	uint64_t src, dest;
};



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

		printf("ERROR in memory allocating for edges1\n");
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

// Directed Graph Implementation in C
int main(void)
{

	char* filename = "fb.txt";

	struct Edge* edges = loadEdges(filename);

	// calculate number of edges
	uint64_t n = total_edges;//sizeof(edges)/sizeof(edges[0]);
	//printf("N: %d\n", n);

	// construct graph from given edges
	struct Graph *graph = createGraph(edges, n);

	// print adjacency list representation of graph
	printGraph(graph);

	free(edges);
	free(graph);

	return 0;
}