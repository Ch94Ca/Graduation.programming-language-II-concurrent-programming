/*
 *   AUTOR: Carlos Henrique Silva Correia de Araujo
 *   ENGENHARIA DE COMPUTAÇÃO - UFPB
 *   https://github.com/ch94ca
 *     
 */ 

#include <unistd.h>
#include <sys/wait.h>
#include <iostream>
#include <new>

using namespace std;

int main(int argc, char **argv)
{
        unsigned int processQty = atoi(argv[1]);

        pid_t newProcessPID;
        pid_t *childpid = new pid_t[processQty - 1];

        newProcessPID = fork();
        childpid[0] = newProcessPID;

        for(unsigned int i = 1; i < processQty; i++)
        {
                if(newProcessPID != 0)
                {
                        newProcessPID = fork();
                        childpid[i] = newProcessPID;

                } // end if

        } // end for

        if(newProcessPID != 0)
        {
                cout << "Iam the process, my PID: " << getpid() << endl
                     << "My children PID's: " << endl << endl;

                for(unsigned int i = 0; i < processQty; i++)
                {
                        cout << "Child " << i + 1 << " PID: " << childpid[i] << endl;

                } // end for

        }
        
        // proposital to see the children process in TOP
        while(true)
        {


        }

        return 0;

} // End Main
