#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cachelab.h"

/******************************************************************************
* Ashley Krattiger 101797698                                                  *
*                                                                             *
* csim.c - Cache Simulator						      *
******************************************************************************/

/******************************************************************************
* struct Parameters							      *
*                                                                             *
* Holds passed-in arguments and other parameters necessary for determining the*
* size of the cache. Also holds int verbose, a parameter which holds 1 if the *
* program is being run verbosely and 0 if it is not and *trace, a String which*
* holds the name of the trace file passed in from command line arguments.     *
******************************************************************************/
struct Parameters{
	int   s;
	int   S;
	int   E;
	int   b;
	int   B;
	char  *trace;
	int   verbose;
};

/******************************************************************************
* struct Set								      *
* 									      *
* Holds the tags stored and an array of numbers representing the time each    *
* tag was last accessed.						      *
******************************************************************************/
struct Set{
	int *tags;
	int *mostRecentlyUsed;
};

/******************************************************************************
* struct Cache								      *
*									      *
* Holds information necessary for the cache, including the hits, misses, and  *
* evictions that have occurred during the reading of the tracefile	      *
******************************************************************************/
struct Cache{
	struct Set *sets;
	int hits;
	int misses;
	int evictions;
};

/******************************************************************************
* Global Variables 							      *
* 									      *
* params - global parameters						      *
* cache - global cache							      *
* timeStamp - counter that increments with every address read   	      *
******************************************************************************/
struct Parameters params;
struct Cache cache;
int timeStamp;

/******************************************************************************
* parseArgs								      *
* 									      *
* Takes the arguments from the command line and stores the information into   *
* the global parameters.						      *
* 									      *
* Arguments: argc - number of arguments, argv[] - the arguments from cmd line *
* Returns nothing							      *
* 									      *
* Variables: token - holds String while it is being read, i - index for loop  *
******************************************************************************/
void parseArgs(int argc, char *argv[])
{
	char *token;
	int i;
	params.trace = (char *)malloc(20);
	for(i = 1; i < argc; i++)
	{
		token = argv[i];
		if(strcmp(token, "-h") == 0 || strcmp(token, "-hv") == 0)
		{
			printf("Usage: ./csim");
			printf(" [-hv] -s <s> -E <E> -b <b> -t <tracefile>\n");
			if(strcmp(token, "-hv") != 0){ i++; }
		}
		if(strcmp(token, "-v") == 0 || strcmp(token, "-hv") == 0)
		{
			params.verbose = 1;
			i++;
		}
		token = argv[i];
		if(strcmp(token, "-s") == 0)
		{
			token = argv[++i];
			params.s = atoi(token);
		}
		else if(strcmp(token, "-E") == 0)
		{
			token = argv[++i];
			params.E = atoi(token);
		}
		else if(strcmp(token, "-b") == 0)
		{
			token = argv[++i];
			params.b = atoi(token);
		}
		else
		{
			token = argv[++i];
			strcpy(params.trace, token);
		}
	}
}

/******************************************************************************
* initializeCache							      *
* 									      *
* Initializes fields and struct Sets for the cache			      *
*									      *
* Takes no arguments, returns nothing					      *
* 									      *
* Variables: i - index for first loop, j - index for second loop	      *
******************************************************************************/
void initializeCache()
{
	int i;
	int j;
	params.S = (1 << params.s);
	params.B = (1 << params.b);
	cache.sets = malloc(sizeof(struct Set)*params.S);
	for(i = 0; i < params.S; i++)
	{
		cache.sets[i].tags = malloc(sizeof(int)*params.E);
		cache.sets[i].mostRecentlyUsed = malloc(sizeof(int)*params.E);
		for(j = 0; j < params.E; j++)
		{
			cache.sets[i].tags[j] = -1;
			cache.sets[i].mostRecentlyUsed[j] = j;
		}
	}
	cache.hits = 0;
	cache.misses = 0;
	cache.evictions = 0;
	timeStamp = 0;
}

/******************************************************************************
* storeData								      *
* 									      *
* Stores a new tag in the least recently used line in the given set    	      *
*									      *
* Arguments: set - index of the set being accessed, tag - new tag to store    *
* Returns nothing							      *
* 									      *
* Variables: leastRecentlyUsed - holds lowest timestamp found in the set      *
* 	     leastIndex - holds index corresponding to lowest timestamp       *
*	     i - index for loop						      *
******************************************************************************/
void storeData(int set, int tag)
{
	int leastRecentlyUsed = -1;
	int leastIndex;
	int i;
	for(i = 0; i < params.E; i++)
	{
		if(leastRecentlyUsed == -1)
		{
			leastRecentlyUsed = cache.sets[set].mostRecentlyUsed[i];
			leastIndex = i;
		}
		else if(cache.sets[set].mostRecentlyUsed[i]< leastRecentlyUsed)
		{
			leastRecentlyUsed = cache.sets[set].mostRecentlyUsed[i];
			leastIndex = i;
		}
	}
	cache.sets[set].tags[leastIndex] = tag;
	cache.sets[set].mostRecentlyUsed[leastIndex] = timeStamp;
}

/******************************************************************************
* readTrace								      *
*									      *
* Reads the trace file specified in command line arguments and carries out    *
* commands given by the input from that file				      *
*									      *
* Takes no arguments, returns nothing					      *
*									      *
* Variables: traceFile - File specified by command line argument	      *
*	     action - char for the command specified by input from the file   *
*	     address - unsigned that holds the address from input from file   *
*	     dataSize - int that holds the request size from input from file  *
*	     set - unsigned that holds the set that will be accessed          *
*	     setClear - bit mask to isolate the set from the address	      *
*	     tag - unsigned that holds the tag that will be compared          *
*	     i - index for loop						      *
*	     leastUsed - holds the smallest timestamp during iteration        *
*	     leastInd - index of the smallest timestamp			      *
******************************************************************************/
void readTrace()
{
	FILE *traceFile = fopen(params.trace, "r");
	char action = 'I';
	unsigned address = 0;
	int dataSize = -1;
	unsigned set;
	unsigned setClear = ~(0xffffffff << params.s);
	unsigned tag;
	int i;
	int leastUsed = -1;
	int leastInd;
	fscanf(traceFile, "%c %x,%d\n", &action, &address, &dataSize);
	while(fscanf(traceFile, "%c %x,%d\n", &action, &address, &dataSize) != EOF)
	{
		if(action != 'I')
		{
			if(params.verbose == 1)
			{ 
				printf("%c %x,%d", action, address, dataSize);
			}
			set = (address >> params.b) & setClear;
			tag = address >> (params.s + params.b);
			for(i = 0; i < params.E; i++)
			{
				if(leastUsed == -1)
				{
					leastUsed = cache.sets[set].mostRecentlyUsed[i];
					leastInd = i;
				}
				else if(cache.sets[set].mostRecentlyUsed[i] < leastUsed)
				{
					leastUsed = cache.sets[set].mostRecentlyUsed[i];
					leastInd = i;
				}
			}
			if(cache.sets[set].tags[leastInd] == -1)
			{
				if(params.verbose == 1){ printf(" miss"); }
				storeData(set, tag);
				cache.misses++;
			}
			else if(cache.sets[set].tags[leastInd] == (int)tag)
			{
				if(params.verbose == 1){ printf(" hit"); }
				cache.hits++;
			}
			else
			{
				if(params.verbose == 1){ printf(" miss eviction"); }
				storeData(set, tag);
				cache.misses++;
				cache.evictions++;
			}
			if(action == 'M')
			{
				if(params.verbose == 1){ printf(" hit"); }
				cache.hits++;
			}
			if(params.verbose == 1){ printf("\n"); }
			timeStamp++;
		}
	}
}

/******************************************************************************
* main									      *
*									      *
* Main process for the program. Calls printSummary() at the end. Takes        *
* standard command line arguments					      *
******************************************************************************/
int main(int argc, char *argv[])
{
	params.s = 0;
	params.S = 0;
	params.E = 0;
	params.b = 0;
	params.B = 0;
	params.trace = "empty";
	params.verbose = 0;
	parseArgs(argc, argv);
	initializeCache();
	readTrace();
	printSummary(cache.hits, cache.misses, cache.evictions);
	return 0;
}
