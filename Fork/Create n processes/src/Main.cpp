/*
 *  fork() and rnd numbers
 * 
 *  Author: Carlos Henrique Silva Correia de Araujo
 *  Computer engineering - UFPB (Undergraduate)
 *  github.com/Ch94Ca
 *  
 *  Last modification: Oct 11, 2020 
 * 
 *  Description:
 *     
 *     this code creates N processes and generate M rnd numbers
 *      in each process.
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

        pid_t newProcessPID = getpid();
        pid_t *childpid = nullptr;

        try
        {
                childpid = new pid_t[processQty - 1];

        } // end try
        catch (bad_alloc &)
        {
                std::cout << "Memory allocation failure." << std::endl;
                return 1;

        } // end catch

        for (unsigned int i = 0; i < processQty; i++)
        {
                if (newProcessPID != 0)
                {
                        newProcessPID = fork();

                        if (newProcessPID >= 0)
                        {
                                childpid[i] = newProcessPID;

                        } // end if
                        else
                        {
                                cout << "Fork failed. " << endl;
                                return 1;

                        } // end if

                } // end if

        } // end for

        if (newProcessPID != 0)
        {
                cout << "Iam the process, my PID: " << getpid() << endl
                     << "My children PID's: " << endl
                     << endl;

                for (unsigned int i = 0; i < processQty; i++)
                {
                        cout << "Child " << i + 1 << " PID: " << childpid[i] << endl;

                } // end for
        }

        // proposital to see the children process in TOP
        while (true)
        {
        }

        return 0;

} // end Main
