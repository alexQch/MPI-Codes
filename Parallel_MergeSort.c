#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <iostream>
#include <algorithm>
#include <ctime>

using namespace std;

void Get_Input(int rank, int comm_sz, int* value, int count)
{
	if(rank == 0)
	{
		printf("Enter Value: ");
		scanf("%d", value);
	}
	
	MPI_Bcast(value, count, MPI_INT, 0, MPI_COMM_WORLD);
}

void Build_MPI_Type(int* first, int* second, MPI_Datatype* new_datatype)
{
	/*
		1- Get relative addresses.
		2- Get Types array.
	*/
	int count = 2;
	int array_of_blocks_length[] = {0,0};
	MPI_Aint array_of_displacement[2];
	MPI_Datatype array_of_datatype[] = {MPI_INT, MPI_INT};

	MPI_Aint first_addr, second_addr;
	MPI_Get_address(first, &first_addr);
	MPI_Get_address(second, &second_addr);
	//Set array of displacements
	array_of_displacement[0] = 0;
	array_of_displacement[1] = second_addr - first_addr;

	//MPI_Create_struct
	MPI_Type_create_struct(count, array_of_blocks_length, array_of_displacement, array_of_datatype, new_datatype);
	MPI_Type_commit(new_datatype);

	//After Using dataType must call MPI_Type_free(new_datatype) to deallocate memory used
}

void my_merge(int* first, int* second, int* result, int szFirst, int szSecond)
{
	int i=0,j=0,k=0;

	while(i < szFirst && j < szSecond)
		result[k++] = (first[i] <= second[j]? first[i++]:second[j++]);

	while(i < szFirst)
		result[k++] = first[i++];

	while(j < szSecond)
		result[k++] = second[j++];
}

int logRank(int r)
{
	int logg = 0;
	while(r > 1 && r & 1)
	{
		logg++; r/=2;
	}

	return logg;
}

int main(int argc, char** argv) {

	int rank, size;
	MPI_Init(NULL, NULL);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	int sizePerProcess, root = 0, currentSize = 0;
	int* sortedArray = NULL, *tempArray = NULL, *resultArray = NULL;
	srand((unsigned)time(0) + rank); 
	/*
	1- Send Array size foreach proces.
	2- Random array and Sort it.
	3- Create Tree Process Comm Array.
	3- Start Tree Process Communication.
	4- Merge Arrays.
	*/

	
	// Create Tree Structure
	int* treeStructure = new int[size];
	int step = 1;
	while(step < size) step *= 2;
	//Create Tree Structure Array
	for(; step > 0; step/=2)
		for(int process = 0; process + step < size; process+= 2*step)
			treeStructure[process + step] = process;
	
	if(rank == root)
	{
		for(int i = 0; i < size; i++)
			printf("%i -> %i \n", i, treeStructure[i]);
	}

	
	if(rank == root)
		sizePerProcess = 10;
	
	MPI_Bcast(&sizePerProcess, 1, MPI_INT, root, MPI_COMM_WORLD);

	currentSize = sizePerProcess;
	sortedArray = new int[currentSize];
	tempArray = new int[currentSize];
	for(int i= 0; i < currentSize; i++)
		sortedArray[i] = rand();

	// Sort Current Array Elements
	sort(sortedArray, sortedArray + currentSize);

	// Syncronization Process
	int sendTo;
	if(rank % 2 == 0)
	{
		int i = rank + 1;
		for(; i < size; i++)
		{
			if(treeStructure[i] != rank)
				continue;

			int tempSize = 0;
			MPI_Recv(&tempSize, 1, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			delete []tempArray;
			tempArray = new int[tempSize];
			MPI_Recv(tempArray, tempSize, MPI_INT, i, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			
			// Merge Two Sorted Arrays
			delete [] resultArray;
			resultArray = new int[currentSize + tempSize];
			my_merge(tempArray, sortedArray, resultArray, tempSize, currentSize);
			delete [] sortedArray;
			sortedArray = new int[currentSize + tempSize];
			currentSize += tempSize;
			for(int j = 0; j < currentSize; j++)
				sortedArray[j] = resultArray[j];
		}

		// Root process Dosen't Send
		if(rank != root)
		{
			sendTo = treeStructure[rank];
			MPI_Send(&currentSize, 1, MPI_INT, sendTo, 0, MPI_COMM_WORLD);
			MPI_Send(sortedArray, currentSize, MPI_INT, sendTo, 1, MPI_COMM_WORLD);
		}
	}
	else
	{
		sendTo = rank - 1;
		MPI_Send(&currentSize, 1, MPI_INT, sendTo, 0, MPI_COMM_WORLD);
		MPI_Send(sortedArray, currentSize, MPI_INT, sendTo, 1, MPI_COMM_WORLD);
	}

	// Print Sorted Array
	if(rank == root)
		for(int i = 0; i < sizePerProcess * size; i++)
			printf("%i ", sortedArray[i]);
	

	delete [] sortedArray;
	delete [] tempArray;
	delete [] resultArray;

	MPI_Finalize();
	return 0;
}
