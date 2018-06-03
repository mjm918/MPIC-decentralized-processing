//Lab Task 3

#include <mpi.h>
#include <iostream>
#include <unistd.h>
#include <cstdlib>
#include <cstring>
#include <fstream>
using namespace std;

const int sizeCoordinator = 10; //10 coordinator
int votesNeeded = 7; //need at least 7 votes to access resource
int coordinator[sizeCoordinator] = {1,2,3,4,5,6,7,8,9,10};
int listOfCoordinatorApprove[sizeCoordinator];

int main (int argc, char* argv[]) {
    //modified code
    MPI_Status stat{};

    for (int i = 0; i < sizeCoordinator; i++) {
        listOfCoordinatorApprove[i] = 0;
    }
    //end modified
    // Initialize the MPI environment
    MPI_Init (NULL, NULL);

    int world_size, count = 0;
    MPI_Comm_size (MPI_COMM_WORLD, &world_size);

    // Get the rank of the process
    // each process will have a "rank" (just an ID) in the communicator
    int rank;
    int data;
    int color;
    fstream file;
    int participants = world_size-sizeCoordinator;

    MPI_Comm_rank (MPI_COMM_WORLD, &rank);

    //Process 0 execute this code
    //Clear the file if it exists and start with a value of 0
    if (rank == 0) {
        file.open ("data.txt", ios::out | ios::trunc);
        file << 0 << endl;
        file.close();
    }

    //wait for every process to reach this point
    MPI_Barrier(MPI_COMM_WORLD);
    color = 30 + rank % 10;

    if(rank == 0 || rank > 10){

        int counter = 0;
        bool access = false;

        for(int i = 0; i < sizeCoordinator; i++){

            int FLAG = 1,res = -1;

            MPI_Send(&FLAG,1,MPI_INT, coordinator[i],0,MPI_COMM_WORLD);
            MPI_Recv(&res,1,MPI_INT,coordinator[i],0,MPI_COMM_WORLD,&stat);

            if(res == 1){
                counter++;
            }

            listOfCoordinatorApprove[i] = stat.MPI_SOURCE;

            if(counter >= votesNeeded){
                access = true;
                for (int l = 0; l < sizeCoordinator; l++){
                    if(coordinator[l] != listOfCoordinatorApprove[l]){
                        FLAG = 4;
                        MPI_Send(&FLAG,1,MPI_INT, coordinator[l],0,MPI_COMM_WORLD);
                    }
                }
                break;
            }
        }
        if(access == true){
            cout << "\033[" << color << "m" << "Process " << rank << " access the file" << endl;
            file.open ("data.txt", ios::in | ios::out);
            file >> data;
            cout << "\033[" << color << "m" << "Process " << rank << " reads " << data << " from the file" << endl;
            data = data + 1;
            file.seekg (0, file.beg);
            file << data << endl;
            cout << "\033[" << color << "m" << "Process " << rank << " writes " << data << " to the file" << endl;
            file.close();
        }

    }else{
        MPI_Status source_sender{};
        int res;
        int approvement;
        for(int i = 0; i < participants; i++){
            MPI_Recv(&res,1,MPI_INT, MPI_ANY_SOURCE,0,MPI_COMM_WORLD,&source_sender);
            if(res != 4){ // 4 means already accessed
                approvement = rand() % 2;
                MPI_Send(&approvement, 1, MPI_INT,source_sender.MPI_SOURCE ,0, MPI_COMM_WORLD);
            }
        }
    }

    MPI_Barrier(MPI_COMM_WORLD);

    //Process 0 execute this code
    //Print out the final value of the file
    if (rank == 0) {
        file.open ("data.txt", ios::in);
        file >> data;
        cout << "The final value of the file is " << data << endl;
        file.close();
    }

    cout << "\033[0m";
    // Finalize the MPI environment.
    MPI_Finalize();
}









