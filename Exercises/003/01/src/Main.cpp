#include <iostream>
#include <thread>
#include <vector>
#include <unistd.h>

using namespace std;

////////////////////////
//      Parâmetros
////////////////////////
#define THREADNO 6
#define EXECNO 10

// gera caracteres aleatórios
void* rndLetter(void *voidThreadId);

// verifica se a string é um palindromo
bool isPalindrome(const string &palindrome);

// variável da barreira das threads
pthread_barrier_t threadBarrier;

// variável da barreira da main
pthread_barrier_t mainBarrier;

// array de caracteres onde as threads irão
// armazenar os caracteres gerados
char rndChar[THREADNO];

int main(void)
{       
        // seed para a função rand
        srand(time(NULL));
        
        pthread_t thread[THREADNO];
        int threadStatus[THREADNO];
        int threadId[THREADNO];

        // inicializa as duas barreiras
        // elas irão aguardar o número de threads + 1(main)
        // chegarem na barreira para liberar a execução
        pthread_barrier_init(&threadBarrier, NULL, THREADNO + 1);
        pthread_barrier_init(&mainBarrier, NULL, THREADNO + 1);

        // cria as threads
        for(long int i = 0; i < THREADNO; i++)
        {       
                threadId[i] = i;
                threadStatus[i] = pthread_create(&thread[i], NULL, rndLetter, &threadId[i]);

        } // end for

        // verifica se alguma thread não foi criada
        for(unsigned int i = 0; i < THREADNO; i++)
        {
                if(threadStatus[i] != 0)
	        {
		        cout << "Thread create error." << endl;
		        exit(EXIT_FAILURE);

                } // end if

	} // end if
        
        // variável que irá armazenar a string gerada
        string word;

        for(int i = 0; i < EXECNO; i++)
        {       
                // barreira da main, esta bareira informa
                // as threds que a função main está pronta
                // para gerar a string, as threads esperam
                // a main chegar nesta barreira antes de 
                // reiniciar o loop de execução
                pthread_barrier_wait(&mainBarrier);

                // barreira das threads, esta barreira é 
                // utilizada pelas threads para informar a 
                // função main que todas as threads terminaram
                // de gerar seus carecteres aleatórios
                pthread_barrier_wait(&threadBarrier);

                // gera a string com os carecteres aleatórios
                // gerados pelas threads
                for(int i = 0; i < THREADNO; i++)
                {       
                        word += rndChar[i];

                } // end for

                cout << "--------------------------------" << endl;
                cout << "String #" << i+1 << ": " << word << endl;
                cout << "- Is Palindrome: " << isPalindrome(word) << endl << endl;
                
                // reseta a string
                word = "";

        } // end for
        
        // realiza o join nas threads criadas
        for(unsigned int i = 0; i < THREADNO; i++)
        {       
                threadStatus[i] = pthread_join(thread[i], NULL);

        } // end for

        // destroi as barreiras criadas
        pthread_barrier_destroy(&threadBarrier);
        pthread_barrier_destroy(&mainBarrier);

        return 0;

} // End Main

// gera caracteres aleatórios
void* rndLetter(void *voidThreadId)
{       
        // realiza a conversão do id da thread
        // para inteiro
        int *threadId = (int*) voidThreadId;

        for(int i = 0; i < EXECNO; i++)
        {       
                // ao chegar nesta barreira, as threads ficam
                // bloqueadas até que a main chegue na barreira
                // isso acontece quando a main reinicia um loop
                // e ja gerou a string com os caracteres gerados
                // a partir dai as threads podem gerar novos 
                // carecteres aleatórios
                // esta barreira é necessária para que as threads
                // aguardem a thread main acessar os caracteres 
                // gerados e realizar a montagem da string antes
                // de gerar novos caracteres
                pthread_barrier_wait(&mainBarrier);

                // valor aleatório
                int rnd = rand() % 26;
                
                // carecter aleatório
                char rndC = 'a' + rnd;

                // armazena no array de carecteres o carecter gerado
                rndChar[*threadId] = rndC;
                
                // ao chegar nesta barreira, as threads ficam
                // bloqueadas até que todas + a função main
                // atinjam a barreira, significando que todas
                // as threads ja realizaram a geração de números
                // aleatórios e a thread main pode prosseguir com
                // o processamento da string
                pthread_barrier_wait(&threadBarrier);

        } // end for
        
        return NULL;

} // end for

// verifica se a string é um palindromo
bool isPalindrome(const string &palindrome)
{       
        // posição inicial da string
        int left = 0;

        // posição final da string
        int right = palindrome.size() - 1;

        // enquanto a posição final for > 1
        while(right > 1)
        {       
                // verifica se o primeiro caracter é diferente
                // do ultimo, e decrementa as variáveis, na proxima
                // execução, irá verificar se o caracter inicio+1
                // é diferente de fim-1, e assim sucessivamente até
                // percorrer toda a string, se entar no if. retorna 
                // false, indicando que a string não é um palindromo
                if(palindrome[left++] != palindrome[right--])
                {
                        return false;

                } // end if

        } // end while

        // se passar por todas as iterações do loop
        // não encontrou caracteres diferentes, e 
        // retorna true
        return true;

} // end isPalindrome