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
 *      in each process, then, the parent process wait for the
 *      children to return the sum value with exit()
 * 
 */

#include <unistd.h>
#include <sys/wait.h>
#include <iostream>
#include <fstream>
#include <new>

using namespace std;

int main(int argc, char **argv)
{
        unsigned int processQty = atoi(argv[1]);
        unsigned int rndNumbersQty = atoi(argv[2]);

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

        int *childSum;

        try
        {
                childSum = new pid_t[processQty - 1];

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
                        wait(&childSum[i]);

                        if (newProcessPID >= 0)
                        {
                                childpid[i] = newProcessPID;

                        } // end if
                        else
                        {
                                cout << "Fork failed. " << endl;
                                delete childpid;
                                return 1;

                        } // end if

                } // end if

        } // end for

        srand(time(NULL) + getpid());

        int *rndNumbers;
        int sum = 0;

        try
        {
                rndNumbers = new int[rndNumbersQty];

        } // end try
        catch (bad_alloc &)
        {
                std::cout << "Memory allocation failure." << std::endl;
                return 1;

        } // end catch

        for (unsigned int i = 0; i < rndNumbersQty; i++)
        {
                rndNumbers[i] = rand() % 100;
                sum += rndNumbers[i];

        } // end for

        string fileName = "Rnd numbers created by procces with PID:  ";
        fileName += to_string(getpid());
        fileName += ".txt";

        ofstream file;
        file.open(fileName);

        for (unsigned int i = 0; i < rndNumbersQty; i++)
        {
                file << to_string(rndNumbers[i]) << endl;

        } // end for

        file << "Sum: " << sum << endl;

        delete rndNumbers;

        file.close();

        cout << "Created file: " << fileName << endl;
        cout << "Sum: " << sum << endl;

        delete childpid;

        if(newProcessPID == 0)
        {       
                exit(sum);

        } // end if
        else
        {       
                cout << endl << "Sum of the rnd numbers genereted by children processes: " << endl << endl;

                for(unsigned int i = 0; i < processQty; i++)
                {
                        cout << "Child " << (i + 1) << " sum: " << WEXITSTATUS(childSum[i]) << endl;

                } // end for

                cout << endl << "Sum of the rnd numbers genereted by parent process: " << sum << endl << endl;

        } // end else

        return 0;

} // End Main
