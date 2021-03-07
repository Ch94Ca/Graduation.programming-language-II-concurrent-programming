/*
 *  Author: Carlos Henrique Silva Correia de Araujo
 *  Computer engineering - UFPB (Undergraduate)
 *  github.com/Ch94Ca
 * 
 */

#include <iostream>
#include <vector>
#include <thread>
#include <semaphore.h>
#include <unistd.h>

using namespace std;

// número de produtores e consumidores
#define P_AND_C_THREADNO 3

// número de execuções das threads
#define EXECNO 2

// tamanho do buffer
#define BUFFERMAX 3

// funções dos produtores 
void producer(int threadId, int execNo);

// função dos consumidores
void consumer(int threadId, int execNo);

// vector do buffer
// apesar de ser uma estrutura dinâmica
// a lógica do código, limita o tamanho
// máximo à constente BUFFERMAX
vector<int> intBuffer;

// semáforo de exclusão mútua
sem_t mutex;

// semáforo de controle da 
// existencia de itens no buffer
sem_t itens;

// semáforo de controle
// do espaço ocupado no buffer
sem_t bufferSpaces;

int main(void)
{       
        // seed para a função rand()
        srand(time(NULL));

        // inicializa o semáforo para exclusão mútua
        sem_init(&mutex, 0, 1);

        // inicializa o semáforo de controle da existencia
        // de itens no buffer
        sem_init(&itens, 0, 0);

        // inicializa o semáforo de controle do espaço
        // ocupado no buffer
        sem_init(&bufferSpaces, 0, BUFFERMAX);

        // inicializa as threads dos produtores
        // de acordo com à constante P_AND_C_THREADNO
        vector<thread> producerThreads;
        for(long int i = 0; i < P_AND_C_THREADNO; i++)
        {
                producerThreads.push_back(thread(producer, i, EXECNO));

        } // end for

        // inicializa as threads dos consumidores
        // de acordo com à constante P_AND_C_THREADNO
        vector<thread>consumerThreads;
        for(long int i = 0; i < P_AND_C_THREADNO; i++)
        {
                consumerThreads.push_back(thread(consumer, i, EXECNO));

        } // end for

        // join nas threads dos produtores
        for(auto& thread : producerThreads)
        {
                thread.join();

        } // end for

        /// join nas threads dos consumidores
        for(auto& thread : consumerThreads)
        {
                thread.join();

        } // end for

        // destroi os semáforos criados
        sem_destroy(&mutex);
        sem_destroy(&itens);
        sem_destroy(&bufferSpaces);

        return 0;

} // End Main

// funções dos produtores 
// esta função gera um número aleatório e insere no fim
// do array de buffer
void producer(int threadId, int execNo)
{       
        // variável que armazena o número aleatório
        int rndValue;

        // loop de número de execuções que cada produtor irá realizar
        for(int i = 0; i < execNo; i++)
        {       
                // gera um número aleatório entre 0 e 10
                rndValue = rand() % 10;

                // semáforo de controle de espaços disponíveis
                // no buffer. se o valor do semáforo for 0,
                // indica que o buffer não tem espaços disponíveis
                // e o produtor será bloqueado até que alguma thread
                // consumidora realize o signal neste semáforo, indicando
                // um espaço livre no buffer
                // para valores maiores que 0, o produtor prossegue
                sem_wait(&bufferSpaces);

                // entra na seção crítica para acessar o buffer
                sem_wait(&mutex);

                        // insere no fim do vetor de buffer o valor gerado
                        intBuffer.push_back(rndValue);

                        cout << "- Produtor #" << threadId 
                                << " produziu e adicionou ao buffer, o valor: " 
                                << rndValue << endl;
                        cout << "- Ocupação do buffer: " << intBuffer.size() << "/" << BUFFERMAX << endl << endl;

                // sai da seção crítica
                sem_post(&mutex);

                // semáforo de controle de existencia de itens no buffer
                // sinaliza para as threads consumidoras que existe um 
                // item pronto no buffer
                sem_post(&itens);       

        } // end for

} // end producer

// funções dos consumidores 
// esta função apenas pega um valor no buffer
void consumer(int threadId, int execNo)
{       
        // define um tempo que o consumidor irá dormir
        // apenas para que o consumo não seja rápido
        // e seja possível vizualizar o funcionamento 
        // das threads produtoras aguardando o buffer  
        // ter espaço disponível para prosseguir
        int sleepTime = (rand() % 4) + threadId;

        // variável que armazena o valor capturado do buffer
        int value;

        // loop de número de execuções que cada produtor irá fazer
        for(int i = 0; i < execNo; i++)
        {       
                // dorme o tempo definido aleatóriamente
                sleep(sleepTime);

                // semáforo de controle de existencia de itens no buffer
                // se o valor do semáforo for 0, significa que ainda não
                // existem itens disponíveis no buffer e o consumidor fica 
                // bloqueado. se for um valor maior que 0, o consumidor prossegue,
                // e decrementa o valor do semáforo
                sem_wait(&itens);

                // entra na seção crítica para acessar o buffer
                sem_wait(&mutex);

                        // captura o valor armazenado na posição 0 do array de buffer
                        value = intBuffer[0];

                        // apaga o valor armazenado na posição 0 do array de buffer
                        intBuffer.erase(intBuffer.begin());

                        // define o tempo aleatório que o consumidor irá dormir
                        // na próxima iteração
                        sleepTime = (rand() % 4) + threadId;

                        cout << "- Consumidor #" << threadId 
                                << " pegou no buffer, o valor: " 
                                << value << ". E agora irá dormir por " 
                                << sleepTime << "s."<< endl;
                                cout << "- Ocupação do buffer: " << intBuffer.size() << "/" << BUFFERMAX << endl << endl;

                // sai da seção crítica
                sem_post(&mutex);

                // semáforo de controle do espaço ocupado no buffer
                // incrementa o valor do semáforo, incrementando
                // o número de posições livres no buffer em 1
                sem_post(&bufferSpaces);

        } // end for

} // end consumer