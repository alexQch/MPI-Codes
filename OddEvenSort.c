#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <iostream>
#include <algorithm>

using namespace std;

int trap(double local_a, double local_b, int local_n, double h)
{
	return local_n * (local_a + local_b)/ h;
}

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

void swap(int& a, int& b)
{
	int temp = a;
	a = b;
	b = temp;
}

void oddEvenSort(int arr[], int sz)
{
	int max_sz = sz-1;
	for(int possible = 0; possible < sz; possible++)
		for(int i = (possible % 2); i < max_sz; i+=2)
			if(arr[i] > arr[i+1])
				swap(arr[i], arr[i+1]);
}

void merge_low(int n, int low[], int high[], int workspace[], bool lower)
{
	for(int i = 0; i < n; i++)
		workspace[i] = low[i];

	int i, k, j;

	if(lower)
	{
		for(i = k = j = 0; k < n; k++)
		{
			if(low[i] <= high[j])
				workspace[k] = low[i++];
			else
				workspace[k] = high[j++];
		}	
	}
	else
	{
		for(k = i = j = n-1; k > -1; k--)
		{
			if(low[i] >= high[j])
				workspace[k] = low[i--];
			else
				workspace[k] = high[j--];
		}
	}

	for(int i = 0; i < n; i++)
		low[i] = workspace[i];
}

int main(int argc, char** argv) {

	int rank, size, nlocal = 5, evenrank, oddrank;
	int *elements, *relements, *workspace;

	MPI_Init(NULL, NULL);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	/* Get Input */
	elements = new int[nlocal];
	relements = new int[nlocal];
	workspace = new int[nlocal];
	//create array
	srand(rank);
	for (int i=0; i<nlocal; i++)
		elements[i] = rand();
	//sort array
	sort(elements, elements + nlocal);

	/* Get sendTo */
	if(rank % 2 == 0)
	{
		oddrank = rank - 1;
		evenrank = rank + 1;
	}
	else
	{
		oddrank = rank + 1;
		evenrank = rank - 1;
	}

	if(oddrank == -1 || oddrank == size)
		oddrank = MPI_PROC_NULL;
	if(evenrank == -1 || evenrank == size)
		evenrank = MPI_PROC_NULL;

	MPI_Status mpi_status;
	for(int i = 0; i < size; i++)
	{
		if(i%2 == 0)
			MPI_Sendrecv(elements, nlocal, MPI_INT, evenrank, 1, relements, nlocal, MPI_INT, evenrank, 1, MPI_COMM_WORLD, &mpi_status);
		else
			MPI_Sendrecv(elements, nlocal, MPI_INT, evenrank, 1, relements, nlocal, MPI_INT, evenrank, 1, MPI_COMM_WORLD, &mpi_status);

		merge_low(nlocal, elements, relements, workspace, (rank < mpi_status.MPI_SOURCE));
	}

	for(int i = 0; i < nlocal; i++)
		printf("my rank:%i value: %i", rank, elements[i]);
	
	delete [] elements;
	delete [] relements;
	delete [] workspace;


	MPI_Finalize();
	return 0;
}
