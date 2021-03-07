/*
 *  Author: Carlos Henrique Silva Correia de Araujo
 *  Computer engineering - UFPB (Undergraduate)
 *  github.com/Ch94Ca
 * 
 */

#include <stdio.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

// porta que será utilizada pelo
// servidor 
#define SERVER_PORT 9988

// tamanho máximo para a string
// recebida do cliente
#define BUFFER_MAX 10

// tamanho máximo para a string
// recebida do cliente
#define MAX_CONNECTED_CLIENTS 6

// struct de dados que serão enviados
// para as threads
struct threadData 
{
	int clientfd;
	struct sockaddr_in clientAddr;	

};

// verifica se uma string é um palindromo
int isPalindrome(const char *string);

// função da thread que irá tratar os
// clientes
void* handleClient(void* data);

// variável de semáforo
// que será utilizado
// para realizar exclusão mútua
sem_t mutex;

// variável de semáforo
// que será utilizado
// para realizar um pequeno
// embarreiramento
sem_t barrier;

// variável global que indica
// quantos clientes estão conectados
int connectedClients = 0;

int main(void) {

	// inicialização do semáforo que será
	// usado para realizar exclusão mútua
	sem_init(&mutex, 0, 1);

	// inicialização do semáforo que será
	// usado para realizar o embarreiramento
	sem_init(&barrier, 0, 0);

	// filedescriptor do servidor e cliente
	int serverfd, clientfd;

	// variáveis para armazenar os atributos
	// do servidor e do cliente
	struct sockaddr_in serveraddr, clientAddr;
	
	socklen_t sizeclientAddr = sizeof(struct sockaddr);

	serverfd = socket(AF_INET, SOCK_STREAM, 0);

	// verifica se o socket foi criado corretamente
	if(serverfd == -1)
	{
		printf("Server socket creation failed!\n");
		exit(1);

	} // end if

	bzero(&serveraddr, sizeof(serveraddr));
	
	// Atributos do servidor
	serveraddr.sin_family = AF_INET; 				// familia TCP/IP
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY); // indica que o socket é um servidor
	serveraddr.sin_port = htons(SERVER_PORT);		// atribui a porta de conexão do socket
	
	// bind, associa o socket aos atributos do servidor
	int bindStatus = bind(serverfd, (struct sockaddr *) &serveraddr, sizeof(serveraddr));

	// verifica se o bind foi executado corretamente
	if(bindStatus != 0)
	{
		printf("Server socket bind failed!\n");
		exit(1);

	} // end if

	// listen, o servidor irá 'escutar' na porta selecionada
	int listenStatus = listen(serverfd, 4);

	// verifica se o listen foi executado corretamente
	if(listenStatus != 0)
	{
		printf("Server socket listen failed!\n");
		exit(1);

	} // end if

	printf("Server listening on port: %d \n\n", SERVER_PORT);

	pthread_t clientThread;
	struct threadData *data = (struct threadData*) malloc(sizeof(struct threadData));

	// loop infinito
	while(1)
	{	
		// aceita novos clientes apenas se o número de clientes conectados
		// for menor que 6
		if(connectedClients < MAX_CONNECTED_CLIENTS)
		{	
			// aceita um novo cliente
			clientfd = accept(serverfd, (struct sockaddr *) &clientAddr, &sizeclientAddr);

			// insere os dados do cliente na struct que será enviada para a thread
			data->clientfd = clientfd;
			data->clientAddr = clientAddr;

			// cria a thread que irá tratar o cliente
			pthread_create(&clientThread, NULL, handleClient, (void*) data);

			// pequena barreira para que a main aguarde a thread do cliente
			// recuperar os dados enviados como argumento para sua própria 
			// variável local, e impedir que a main aceite outro cliente
			// e modifique o ponteiro 'data' antes do cliente realizar a 
			// cópia para a variável local
			sem_wait(&barrier);

		} // end if

	} // end while

	return 0;

} // end main

// verifica se uma string é um palindromo
int isPalindrome(const char *string)
{       
        // posição inicial da string
        int left = 0;

        // posição final da string
        int right = strlen(string) - 2;

        // enquanto a posição final for > 1
        while(right > 1)
        {       
                // verifica se o primeiro caracter é diferente
                // do ultimo, e decrementa as variáveis, na proxima
                // execução, irá verificar se o caracter inicio+1
                // é diferente de fim-1, e assim sucessivamente até
                // percorrer toda a string, se entar no if. retorna 
                // false, indicando que a string não é um palindromo
                if(string[left++] != string[right--])
                {
                        return 0;

                } // end if

        } // end while

        // se passar por todas as iterações do loop
        // não encontrou caracteres diferentes, e 
        // retorna true
        return 1;

} // end isPalindrome

void* handleClient(void* data)
{	
	// protocolo de entrada da seção crítica
	// para incrementar o número de clientes
	// conectados
	sem_wait(&mutex);

		connectedClients++;

	// protocolo de saída da seção crítica
	sem_post(&mutex);

	// recupera os dados passados como argumento para a função
	// e os armazena em uma variável local
	struct threadData clientData = *((struct threadData*) data);

	// sinaliza para a função main que terminou de copiar os
	// dados do ponteiro para sua variável local, permitindo
	// que a função main possa aceitar outro cliente e modificar
	// o conteúdo apontado pelo ponteiro data
	sem_post(&barrier);

	// recupera o IP do cliente
	char ipAddress[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &clientData.clientAddr.sin_addr, ipAddress, INET_ADDRSTRLEN);

	// variáveis com strings que serão enviadas aos clientes
	char *msg = "\nOlá cliente, seja bem-vindo, digite o seu nome (10 caracteres max): ";
	char *palindromeTrue = "\n        PALINDROME=TRUE\n";
	char *palindromeFalse = "\n       PALINDROME=FALSE\n";
	
	// variáveis que armazenam strings enviadas pelos clientes
	char buffer[BUFFER_MAX + 1];
	char clientName[BUFFER_MAX + 1];
	bzero(clientName, BUFFER_MAX);

	// Envia a menssagem inicial ao cliente
	// pedindo para que ele informe o seu nome
	// apenas para identificar cada cliente no
	// terminal do servidor
	write(clientData.clientfd, msg, strlen(msg));

	// Recebe o retorno do cliente
	read(clientData.clientfd, clientName, BUFFER_MAX);

	// remove o \n da string
	clientName[strlen(clientName)-2] = '\0';

	// protocolo de entrada da seção crítica
	// para imprimir no terminal do servidor
	// e evitar 'bagunça' nos printfs
	// caso dois ou mais clientes façam o
	// servidor imprimir no terminal ao mesmo tempo
	sem_wait(&mutex);

		// imprime o nome e ip do cliente conectado
		printf("- Cliente '%s' conectado com o ip: %s\n", clientName, ipAddress);
		printf("- Capacidade do servidor: %d/%d\n\n", connectedClients, MAX_CONNECTED_CLIENTS);

	// protocolo de saída da seção crítica
	sem_post(&mutex);

	// define a menssagem que será enviada ao cliente durante o loop
	msg = "\n - Digite uma palavra (10 caracteres max):  ";

	// variável que irá armazenar o retorno da função isPalindrome
	int palindrome;

	// loop infinito
	while(1)
	{	
		bzero(buffer, BUFFER_MAX);

		// Envia menssagem ao cliente, pedindo que digite
		// uma palavra
		write(clientData.clientfd, msg, strlen(msg));

		// Recebe o retorno do cliente
		read(clientData.clientfd, buffer, BUFFER_MAX);

		// remove o \n da string
		buffer[strlen(buffer)-1] = '\0';

		// informa que o cliente desconectou e interrompe
		// o loop infinito caso o cliente digite "BYEBYE"
		if(strstr(buffer, "BYEBYE") != 0)
		{	
			// protocolo de entrada da seção crítica
			// para decrementar o número de clientes
			// conectados, e imprimir no terminal do
			// servidor
			sem_wait(&mutex);	

				connectedClients--;

				printf("- Cliente %s(%s) se desconectou.\n", clientName, ipAddress);
				printf("- Capacidade do servidor: %d/%d\n\n", connectedClients, MAX_CONNECTED_CLIENTS);

			// protocolo de saída da seção crítica
			sem_post(&mutex);
			break;

		} // end if

		// destroi o semáforo criado, libera o espaço de memória
		// alocado para a variável data, informa que o cliente
		// encerrou o servidor, e encerra a execução do programa
		if(strstr(buffer, "SHUTDOWN") != 0)
		{	
			// protocolo de entrada da seção crítica
			// para destruir o semáforo, liberar
			// espaço de memória alocado dinamicamente
			// e imprimir no terminal do servidor
			sem_wait(&mutex);

				printf("- Servidor encerrado pelo cliente %s(%s)\n\n", clientName, ipAddress);

				free((struct threadData*) data);
				sem_destroy(&barrier);
				sem_destroy(&mutex);

				exit(0);

			// sem protocolo de saida da seção crítica, pois o semáforo será destruido
			// e a execução encerrada
			//sem_post(&mutex);

		} // end if

		// verifica se existe uma string no buffer
		if(strlen(buffer) > 0)
		{	
			// verifica se a string passada pelo cliente é um palindromo
			palindrome = isPalindrome(buffer);

			// protocolo de entrada da seção crítica
			// para imprimir no terminal do servidor
			// e evitar 'bagunça' nos printfs
			// caso dois ou mais clientes façam o
			// servidor imprimir no terminal ao mesmo tempo
			sem_wait(&mutex);

				// imprime no terminal do servidor, nome, IP do cliente, e string enviada por ele
				printf("Recebi de: %s(%s), a string: %s\n", clientName, ipAddress, buffer);

			// protocolo de saída da seção crítica
			sem_post(&mutex);

			// retorna TRUE ao cliente caso a string seja um palindrome e imprime
			// o retorno no terminal do servidor
			if(palindrome)
			{
				write(clientData.clientfd, palindromeTrue, strlen(palindromeTrue));

				// exclusão mútua para
				// evitar bagunça nos printfs
				sem_wait(&mutex);

					printf("E retornei: PALINDROME=TRUE\n\n");

				sem_post(&mutex);

			}// end if

			// retorna FALSE ao cliente caso a string não seja um palindrome e imprime
			// o retorno no terminal do servidor
			else
			{
				write(clientData.clientfd, palindromeFalse, strlen(palindromeFalse));

				// exclusão mútua para
				// evitar bagunça nos printfs
				sem_wait(&mutex);

					printf("E retornei: PALINDROME=FALSE\n\n");

				sem_post(&mutex);

			} // end else

		} // end if

	} // end while

	// encerra a conexão com o cliente
	close(clientData.clientfd);

	// encerra a thread
	pthread_exit(0);

} // end handleClient