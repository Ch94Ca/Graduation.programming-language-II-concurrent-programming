/*
 *  Author: Carlos Henrique Silva Correia de Araujo
 *  Computer engineering - UFPB (Undergraduate)
 *  github.com/Ch94Ca
 *  
 */

#include <unistd.h>
#include <sys/wait.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>

using namespace std;

// realiza a leitura do arquivo armazenando cada linha em um elemento
// de um vector de strings
void readFile(vector <string> &fileString, unsigned int fileNumber);

// retorna o número de vezes que a string passada como argumento aparece
// no vector de strings passado como argumento
int searchStringIn(const string &userString, const vector <string> &fileString);

int main(int argc, char **argv)
{       
        // armazena o número de processos a serem criados
        const unsigned int processQty = 5;

        // armazena o pid do processo criado
        pid_t newProcessPID = getpid();

        // vector que armazena o PID dos processos filhos
        vector <pid_t> childPid;

        // vector que armazena a saída dos processos filhos
        vector <int> childSum(processQty);
        fill(childSum.begin(), childSum.end(), 0); 

        cout << "Digite a string a ser buscada nos arquivos: ";
        string userString;
        getline(cin, userString);

        for (unsigned int i = 0; i < processQty; i++)
        {       
                // verifica se está no processo pai
                if (newProcessPID != 0)
                {       
                        // cria um novo processo
                        newProcessPID = fork();

                        if (newProcessPID >= 0)
                        {       
                                // armazena o pid do processo criado
                                childPid.push_back(newProcessPID);

                        } // end if
                        else
                        {       
                                // encerra a execução em caso de erro
                                // na criação do processo
                                cout << "Fork failed. " << endl;
                                return 1;

                        } // end if

                } // end if

        } // end for

        if(newProcessPID != 0)
        {               
                int sumOfAl = 0;

                // aguarda os processos filhos encerrarem suas execuções
                // recupera o valor retornado por eles e realiza a soma
                // na variável sumOfAll
                for(unsigned int i = 0; i < processQty; i++)
                {
                        waitpid(childPid[i], &childSum[i], 0);

                        sumOfAl += WEXITSTATUS(childSum[i]);

                } // end for

                cout << "Número de ocorrências em que a string '" << userString << "' aparece em todos os arquivos: " << sumOfAl << endl;

        } // end if

        // execução dos processos filhos
        if(newProcessPID == 0)
        {       
                // numero do processo para identificar 
                // em qual processo está ocorrendo a execução
                // de acordo com o tamanho do vector que armazena
                // o pid dos processos filhos
                long unsigned int childNo = childPid.size(); 

                // vector que armazena as linhas dos arquivos lidos
                vector <string> fileString;

                // execução do professo filho 1
                if(childNo == 1)
                {       
                        int sum;

                        readFile(fileString, 0);
                        sum = searchStringIn(userString, fileString);

                        cout << "Número de ocorrências em que a string '" << userString << "' aparece no arquivo 0.txt: " << sum << endl;

                        exit(sum);

                } // end if

                // execução do processo filho 2
                else if(childNo== 2)
                { 
                        int sum;

                        readFile(fileString, 1);
                        sum = searchStringIn(userString, fileString);

                        cout << "Número de ocorrências em que a string '" << userString << "' aparece no arquivo 1.txt: " << sum << endl;

                        exit(sum);

                } // end if

                // execução do professo filho 3
                else if(childNo == 3)
                { 
                        int sum;

                        readFile(fileString, 2);
                        sum = searchStringIn(userString, fileString);

                        cout << "Número de ocorrências em que a string '" << userString << "' aparece no arquivo 2.txt: " << sum << endl;

                        exit(sum);

                } // end if

                // execução do professo filho 4
                else if(childNo== 4)
                { 
                        int sum;

                        readFile(fileString, 3);
                        sum = searchStringIn(userString, fileString);

                        cout << "Número de ocorrências em que a string '" << userString << "' aparece no arquivo 3.txt: " << sum << endl;

                        exit(sum);

                } // end if

                // execução do professo filho 5
                else if(childNo == 5)
                { 
                        int sum;

                        readFile(fileString, 4);
                        sum = searchStringIn(userString, fileString);

                        cout << "Número de ocorrências em que a string '" << userString << "' aparece no arquivo 4.txt: " << sum << endl;

                        exit(sum);

                } // end if

        } // end if
      
        return 0;

} // End Main

// realiza a leitura do arquivo armazenando cada linha em um elemento
// de um vector de strings
void readFile(vector <string> &fileString, unsigned int fileNumber)
{       
        string filePath = "files/";
        filePath += to_string(fileNumber);
        filePath += ".txt";

        string temp;

        ifstream file;
        file.open(filePath);

        if (file.is_open())
        {
                while (std::getline(file, temp))
                {       
                        // remove o /n no fim da string
                        temp.erase(temp.length() - 1);

                        // insere a linha lida no vector
                        fileString.push_back(temp);

                } // end while
        
        } // end if

        file.close();

} // end readFile

// retorna o número de vezes que a string passada como argumento aparece
// no vector de strings passado como argumento
int searchStringIn(const string &userString, const vector <string> &fileString)
{
        long unsigned int vectorSize = fileString.size();
        int sum = 0;

        for(long unsigned int i = 0; i < vectorSize; i++)
        {
                if(userString.compare(fileString[i]) == 0)
                {
                        sum++;

                } // end if

        } // end for

        return sum;

} // end searchStringIn