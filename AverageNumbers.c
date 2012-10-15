#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <iostream>

using namespace std;

void create_rand_array(int** ptr_array, int count)
{
	
	printf("generating random digits. \n");

	(*ptr_array) = new int[count];

	for(int i = 0; i < count; i++)
	{
		(*ptr_array)[i] = rand();
		printf("%d, ", (*ptr_array)[i]);
	}
	printf("\n");
	
}

int main(int argc, char** argv) {

	//INIT MPI World
	MPI_Init(NULL, NULL);

	int rank, world_size;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);
	
	int *sendData, *recieveData, max_data_size = 10;
	int recieve_size = max_data_size/world_size;
	//Check if i'm the master
	if(rank == 0)
	{
		create_rand_array(&sendData, max_data_size);
		/*
		sendData = new int[max_data_size];
		for(int i = 0; i < max_data_size; i++)
			sendData[i] = rand() * 0.5;
		*/
	}

	recieveData = new int[recieve_size];
	MPI_Scatter(sendData, recieve_size, MPI_INT, recieveData, recieve_size, MPI_INT, 0, MPI_COMM_WORLD);

	float average = 0;
	for(int i = 0; i < recieve_size; i++)
		average += recieveData[i];

	//average = average / recieve_size;
	//printf("average is %f at %d \n", average, rank);

	float *recieve_array;
	if(rank == 0)
		recieve_array = new float[world_size];

	MPI_Gather(&average, 1, MPI_FLOAT, recieve_array, 1, MPI_FLOAT, 0, MPI_COMM_WORLD);

	if(rank == 0)
	{
		float result = 0;
		for(int i = 0; i < world_size; i++)
			result += recieve_array[i];
		result = result / world_size;

		printf("\n result of calculating average is = %f", result);
		//cleanup memory allocations at root node
		delete[] sendData;
		delete[] recieve_array;
	}

	delete[] recieveData;

	//Finalize MPI World
	MPI::Finalize();
	return 0;
}
