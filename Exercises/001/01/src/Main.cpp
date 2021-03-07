/*
 *  Author: Carlos Henrique Silva Correia de Araujo
 *  Computer engineering - UFPB (Undergraduate)
 *  github.com/Ch94Ca
 *  
 */

#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <unistd.h>
#include <sys/wait.h>

using namespace std;

void type_prompt();
void read_command();

// variável global para armazenar a entrada do usuário
// a ser enviada no exec()
char **arg;

int main(void)
{       
        while(true)
        {       
                int status;

                type_prompt();
                read_command();

                // cria o novo processo
                if(fork() != 0)
                {       
                        // faz com que o processo pai aguarde o fim
                        // da execução do processo filho
                        waitpid(-1, &status, 0);

                } // end if
                else
                {       
                        // executa o comando passado pelo usuário
                        // e verifica se a execução foi realizada
                        // corretamente
                        if(execvp(arg[0], &arg[0]) == -1)
                        {
                                cout << "Exec failed." << endl << endl;
                                exit(1);

                        } //

                        exit(0);

                } // end else

        } // end while

        return 0;

} // End Main

void type_prompt()
{
        cout << endl << "[shell]$ ";

} // end type_prompt

void read_command()
{       
        // recebe a entrada do usuário
        // em uma única string
        string user_entry;
        getline(cin, user_entry);

        // vector para armazenar separadamente
        // a entrada em string
        static vector <string> entry_parameters_string; 
        entry_parameters_string.clear();

        // vector para armazenar separadamente
        // em um array de caracteres
        static vector <char*> entry_parameters_char; 
        entry_parameters_char.clear();

        stringstream check(user_entry); 
        string intermediate; 

        // separa a string de entrada do usuário
        // utilizando um espaço em branco como token
        while(getline(check, intermediate, ' ')) 
        {                       
                entry_parameters_string.push_back(intermediate);

        } // end while

        // realiza a 'conversão' das strings separadas para
        // um array de caracteres, formato utilizado no exec()
        // e insere no vector
        for(unsigned long int i = 0; i < entry_parameters_string.size(); i ++)
        {
                entry_parameters_char.push_back(const_cast<char*>(entry_parameters_string[i].c_str())); 

        } // end for

        // verifica se o comando passado pelo usuário foi
        // exit, e caso seja, encerra a execução
        if(entry_parameters_string[0] == "exit")
        {
                exit(0);

        } // end fi
        
        // insere NULL no último elemento do vector.
        // o comando exec() espera que o último elemento
        // do array de argumentos seja nulo
        entry_parameters_char.push_back(NULL); 

        arg = entry_parameters_char.data();

} // end read_command
