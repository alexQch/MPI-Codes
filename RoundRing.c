#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv) {

	//INIT MPI World
	MPI_Init(NULL, NULL);

	int rank, world_size;
	//get process rank
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	//get world size
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);

	int number, MPI_TAG = 0, sendTo = (rank + 1)% world_size, recieveFrom = (rank - 1 + world_size) % world_size;
	//Master node will send main message
	if(rank == 0)
	{
		number = -1;
		MPI_Send(&number, 1, MPI_INT, sendTo, MPI_TAG, MPI_COMM_WORLD);
	}

	MPI_Recv(&number, 1, MPI_INT, recieveFrom, MPI_TAG, MPI_COMM_WORLD, MPI_STATUSES_IGNORE);
	printf("process: %d recieved number: %d from : %d \n", rank, number,recieveFrom); 

	if(rank != 0)
		MPI_Send(&number, 1, MPI_INT, sendTo, MPI_TAG, MPI_COMM_WORLD);
	
	//Finalize MPI World
	MPI::Finalize();
}
