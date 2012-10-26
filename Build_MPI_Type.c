#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <iostream>

using namespace std;

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

	int rank, size, n=1024, local_n;
	int source = 0;

	MPI_Init(NULL, NULL);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	MPI_Finalize();
	return 0;
}
