i'm implementing programs and algorithms in Graph Theory, Numerical Methods, Linear System Solvers using MPI. 


First: MPI primitives:

using Custom DataTypes:
	1- Build_MPI_Type: creates custom MPI_DataType to enhance communication overhead of sending messages with different types.

using synchronous Send/Recieve 
	1- PingPong.
	2- Round Ring.

using MPI_Probe, MPI_Get_count to recive Dynamic Size data
	1- DynamicSendRecieve.

using MPI_Scatter, MPI_Gather to scatter random array digits and calculate average
	1- AverageNumbers.

Second: Simple Programs:
	1- OddEven Sort.
	2- PI Estimation using Randomness.
	3- Parallel Merge Sort(combining sub-problems using Tree Communication Structure).
