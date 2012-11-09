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

int main(int argc, char** argv) {

	int rank, size;
	MPI_Init(NULL, NULL);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	int tossPerProcess, root = 0;
	long long totalHits = 0;
	srand((unsigned)time(0) + rank); 
	/*
	number in circle = 0;
	for (toss = 0; toss < number of tosses; toss++) 
		f x = random double between -1 and 1;
		y = random double between 􀀀1 and 1;
		distance squared = xx + yy;
		if (distance squared <= 1) number in circle++;
	pi estimate = 4 * number in circle/((double) number of tosses); */

	if(rank == root)
		tossPerProcess = 10000000;
	
	MPI_Bcast(&tossPerProcess, 1, MPI_INT, root, MPI_COMM_WORLD);

	//printf("process %i recieved %i tosses\n", rank, tossPerProcess);
	long long numberInCircle = 0;
	double x,y, distance;
	for(int i= 0; i < tossPerProcess; i++)
	{
		x = (rand()-RAND_MAX/2.0)/(RAND_MAX/2.0);
		y = (rand()-RAND_MAX/2.0)/(RAND_MAX/2.0);
		distance = x*x + y*y;
		if(distance <= 1)
			numberInCircle++;
	}
	//printf("process %i made %i hits\n", rank, numberInCircle);
	
	MPI_Reduce(&numberInCircle, &totalHits, 1, MPI_LONG_LONG, MPI_SUM, root, MPI_COMM_WORLD);

	if(rank == root)
	{
		printf("total hits: %i\n", totalHits);
		printf("Pi Equals: %f", (4.0 * totalHits)/(tossPerProcess * size));
	}

	MPI_Finalize();
	return 0;
}
