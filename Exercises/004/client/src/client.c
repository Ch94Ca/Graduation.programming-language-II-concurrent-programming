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

// porta que será utilizada pelo
// servidor 
#define SERVER_PORT 9988

// tamanho máximo para a string
// recebida do cliente
#define RECEIVEBUFFER_MAX 100

// tamanho máximo para a string
// recebida do cliente
#define SENDBUFFER_MAX 10

// número de strings a serem geradas
// e enviadas ao servidor
#define MAXSGENTRINGS 10

// gera caracteres aleatórios
char rndLetter();

// gera strings aleatórias
void generateRndString(int stringSize, char* string);

int main(void)
{	
	// filedescriptor do cliente
    int clientfd;

	// variáveis para armazenar os atributos
	// do cliente
    struct sockaddr_in serveraddr;

    clientfd = socket(AF_INET, SOCK_STREAM, 0);
	
	// verifica se o socket foi criado corretamente
	if(clientfd == -1)	
	{
		printf("server socket creation error \n");
		exit(1);

	} // end if

	bzero(&serveraddr, sizeof(serveraddr));
	
	// Atributos do cliente
	serveraddr.sin_family = AF_INET;						// familia TCP/IP
	serveraddr.sin_addr.s_addr = inet_addr("127.0.0.1");	// indica o ip que o socket tentara se conectar
	serveraddr.sin_port = htons(SERVER_PORT);				// atribui a porta de conexão do socket

	// bind, associa o socket aos atributos do servidor
    //bind(clientfd, (struct sockaddr_in *) &serveraddr, sizeof(serveraddr));

	// tenta realizar a conexão do cliente com o servidor

	// sockets TCP prezam pela confiabilidade, com controle de fluxo e recebimento,
	// assim sockets TCP garantem a ordem, e o recebimento das menssagens, entretanto
	// isso demanda um maior fluxo de dados

	// sockets UDP, diferente dos sockets TCP, não garantem ordem nem recebimendo
	// não é necessário criar uma conexão, apenas enviar os dados.

	// o commando connect é essencial para o envio utilizando o protocolo TCP, ja que 
	// em TCP é necessário estabelecar uma conexão. Para o envio em UDP, basta apenas
	// enviar os dados
    int connectStatus = connect(clientfd, (struct sockaddr *) &serveraddr, sizeof(serveraddr));

	// verifica se a conexão foi criada corretamente
    if(connectStatus != 0)
    {
        printf("Connection Error\n");
        exit(1);

    } // end if

	// variáveis que irão armazenar as strings
	char buffer[RECEIVEBUFFER_MAX + 1];
	bzero(buffer, RECEIVEBUFFER_MAX);
        
    char rndString[SENDBUFFER_MAX + 1];
	bzero(rndString, SENDBUFFER_MAX);

	// por algum motivo, o telnet adiciona dois \n a string enviada,
	// no servidor que eu implementei, eu tratei isso, entao preciso criar
	// a string aqui com os dois \n
	char userName[SENDBUFFER_MAX] = "Carlos\n\n";

	// RECEBE MENSSAGEM DO SERVIDOR
	read(clientfd, buffer, RECEIVEBUFFER_MAX);
	printf("- Servidor enviou:\n");
	printf("%s", buffer);

	// ENVIA o nome de usuário ao servidor
	write(clientfd, userName, strlen(userName));

	// tratamento que eu realizo no server para remover os dois \n
	userName[strlen(userName)-2] = '\0';

    printf("\n\n  - Enviei o meu nome de usuário: %s, ao servidor\n", userName);

	for(int i = 0; i < MAXSGENTRINGS; i++)
	{	
		// zera as strings buffer e rndString
		bzero(buffer, RECEIVEBUFFER_MAX);
		bzero(rndString, SENDBUFFER_MAX);

        // RECEBE MENSSAGEM DO SERVIDOR
		read(clientfd, buffer, RECEIVEBUFFER_MAX);
		printf("- Servidor enviou:\n");
		printf("%s", buffer);

		// gera uma string aleatória com 6 caracteres
		generateRndString(6, rndString);

		// ENVIA a string gerada ao servidor
		write(clientfd, rndString, strlen(rndString));

		printf("\n\n- Enviei ao servidor a string: %s\n\n", rndString);

		bzero(buffer, RECEIVEBUFFER_MAX);
		// RECEBE MENSSAGEM DO SERVIDOR
		read(clientfd, buffer, RECEIVEBUFFER_MAX);
		printf("- Servidor enviou:\n");
		printf("%s", buffer);

	} // end for

	// envia menssagem para se desconectar do servidor
	char* endMsg = "BYEBYE\n";
	write(clientfd, endMsg, strlen(endMsg));

	printf("\n\n- Enviei ao servidor a string: %s\n\n", endMsg);

	close(clientfd);
	
} // end main

// gera caracteres aleatórios
char rndLetter()
{       
	// valor aleatório
    int rnd = rand() % 26;
                
    // caracter aleatório
    char rndC = 'a' + rnd;

    return rndC;

} // end for

// gera strings aleatórias
void generateRndString(int stringSize, char* string)
{	
	for(int i = 0; i < stringSize; i++)
	{	
		string[i] = rndLetter();

	} // end for

	string[stringSize] = '\n';

} // end generateRndString