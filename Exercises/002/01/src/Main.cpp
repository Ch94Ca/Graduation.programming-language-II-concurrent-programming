/*
 *   AUTOR: Carlos Henrique Silva Correia de Araujo
 *   ENGENHARIA DE COMPUTAÇÃO - UFPB
 *   https://github.com/ch94ca
 *     
 */ 

#include <iostream>
#include <thread>
#include <vector>
#include <unistd.h>

using namespace std;

void rollerCoasterCarFunction();
void personFunction(int personId);

// variável que indica se o parque
//está fechado
bool isClosed = false;

// variáveis que indicam o funcionamento 
// da montanha russa
bool started = false;
bool finished = true;

// variável para armazenar o número
// de assentos ocupados 
int occupiedSeats = 0;

// variáveis do algoritmo de ticket
int ticket = 1;
int nextPerson = 1;
vector<int> turn;

// vector auxiliar para mostrar, ao fim da execução,
// quantas vezes cada thread entrou na montanha russa
vector<int> timesIn;

//////////////////////////////////////////////////////////
// Parâmetros
//////////////////////////////////////////////////////////

const int maxRollerCoasterLaps = 10;    // número max de voltas
const int waitTime = 5;                 // tempo de espera antes de iniciar uma volta
const int maxLapTime = 5;               // tempo max de cada volta
const int maxCapacity = 5;              // capacidade máxima para cada volta
const int maxWalkTime = 5;              // tempo max de espera após cada volta

int main(void)
{               
        srand(time(NULL));

        // número de pessoas no parque
        const int personNo = 20;

        cout << "Parque Aberto." << endl << endl;
        cout << "- Número de pessoas no parque: " << personNo << endl;

        // inicia a thread da montanha russa
        thread carThread (rollerCoasterCarFunction);

        // inicializa o vector dos tickets para cada thread
        // e do vector auxiliar
        for(int i = 0; i < personNo; i++)
        {
                turn.push_back(0);
                timesIn.push_back(0);

        } // end for

        vector<thread> person;

        // faz a main dormir 1s, apenas para dar tempo
        // da função da montanha russa imprimir no terminal
        // e não ter problemas de impressão com as threads
        // que serão criadas

        sleep(1);

        // cria as threads que irão representar as pessoas
        for(int i = 0; i < personNo; i++)
        {
                person.push_back(thread(personFunction, i));

        } // end for
        
        // join na thread da montanha russa
        carThread.join();

        // join nas threads das pessoas
        for(auto& thread : person)
        {
                thread.join();

        } // end for

        cout << "-----------------------------------------------" << endl << endl;

        cout << "- Número de vezes que cada pessoa andou na montanha russa: " << endl << endl;

        for(int i = 0; i < personNo; i++)
        {
                cout << "- Pessoa #" << i+1 << " : " << timesIn[i] << endl;

        } // end for
        
        return 0;

} // End Main

void rollerCoasterCarFunction()
{       
        // número atual de voltas na montanha russa
        int rollerCoasterLaps = 0;

        int lapTime;

        cout << "Montanha russa ativa." << endl << endl;
        cout << "- Capacidade máxima da montanha russa: " << maxCapacity << endl;
        cout << "- Número máximo de voltas: " << maxRollerCoasterLaps << endl << endl;

        // enquanto o parque não está fechado
        while(!isClosed)
        {       
                finished = false;

                cout << "-----------------------------------------------" << endl << endl;
                cout << "- Volta #" << rollerCoasterLaps + 1 << endl;
                cout << "- Aguardando " << waitTime << " segundos para iniciar a volta." << endl;
                cout << "-----------------------------------------------" << endl << endl;

                // aguarda tempo fixo para iniciar a volta
                sleep(waitTime);

                cout << "-----------------------------------------------" << endl << endl;
                cout << "- Volta iniciada." << endl;
                cout << "- Passageiros a bordo: " << occupiedSeats << endl;

                // sinaliza que o carro está dando a volta
                started = true;

                // define o tempo aleatório da volta, +1 para evitar tempo = 0
                lapTime = rand() % maxLapTime + 1;

                cout << "- A volta vai durar " << lapTime << " segundos." << endl;

                // realiza a volta no tempo aleatório
                sleep(rand() % maxLapTime);

                cout << "- A volta #" << rollerCoasterLaps + 1 << " foi concluida." << endl << endl;

                // incrementa o número de voltas dado pela montanha russa
                rollerCoasterLaps++;

                // esvazia os assentos após a volta
                occupiedSeats = 0;

                // informa a volta terminou
                finished = true;

                // fecha o parque se o número de voltas for igual ao número
                // máximo de voltas
                if(rollerCoasterLaps == maxRollerCoasterLaps)
                {       
                        isClosed = true;
                        cout << "Número máximo de voltas atingido. Fechando o parque." << endl << endl;

                } // end if

                started = false;

        } // end while

} // end rollerCoasterCar

void personFunction(int personId)
{       
        // variável que armazena o tempo que cada thread
        // irá esperar após cada volta
        int walkTime;

        // variável booleana que indica se a thread
        // entrou na montanha russa
        bool personEntered;

        while(!isClosed)
        {
                personEntered = false;

                // protocolo de entrada
                turn[personId] = __sync_fetch_and_add(&ticket, 1);

                while(turn[personId] != nextPerson);

                // seção crítica

                // enquanto o parque não estiver fechado
                // a thread que representa a pessoa
                // ficará na 'fila' para entrar na montanha russa
                while(!isClosed)
                {
                        // verifica se tem assentos vazios E a volta ainda não iniciou
                        if((occupiedSeats < maxCapacity) && !started)
                        {       
                                // ocupa um assento na montanha russa
                                occupiedSeats++;

                                // indica que a thread entrou na montanha russa
                                personEntered = true;

                                cout << "- Pessoa #" << personId + 1 << " entrou na montanha russa. " << endl;
                                cout << "- Assentos disponíveis: " << maxCapacity - occupiedSeats << "/" << maxCapacity << endl << endl;

                                break;

                        } // end if

                } // end while

                // protocolo de saida
                nextPerson++;

                // se a pessoa conseguiu entrar na montanha russa
                if(personEntered)
                {       
                        // aguarda a volta terminar
                        while(!finished);
                        
                        // incrementa o valor no vector auxiliar
                        // indicando que aquela thread deu uma volta 
                        // na montanha russa
                        timesIn[personId]++;

                        // se o parque não estiver fechado
                        // a thread irá dormir (dar uma volta) 
                        // por um tempo aleatório 
                        if(!isClosed)
                        {       
                                // define o tempo aleatório, +1 para evitar tempo = 0
                                walkTime = (rand() % maxWalkTime) + 1;
                                sleep(walkTime);

                        } // end if

                } // end if

        } // end while

} // end void