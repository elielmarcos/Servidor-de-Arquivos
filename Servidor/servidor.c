#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <windows.h>
#include <locale.h>
#include <dirent.h>
#include <pthread.h>
#include <string.h>

//#include <semaphore.h>
//#include <sys/stat.h>

#define BYTE 16384
#define PORTA 5000
#define BACKLOG 10
#define TITULO "\n =======\033[43m SERVIDOR DE ARQUIVOS \033[40m=======\n\n"

void Ajuda(int connfd);
void Criar_DIR(int connfd, char *dir_Caminho);
void Remover_DIR(int connfd, char *dir_Caminho);
void Entrar_DIR(int connfd, char *dir_Caminho);
void Sair_DIR(int connfd, char *dir_Caminho);
void Mostrar_DIR(int connfd, char *dir_Caminho);
void Criar_FILE(int connfd);
void Remover_FILE(int connfd);
void Escrever_FILE(int connfd);
void Mostrar_FILE(int connfd);
void CMD(int connfd);
void Invalido(int connfd);
void retENT(char *recvBuff);
void* Thread_Conexao(void *Con_socket);

char dir_Raiz[BYTE];



int main(int argc, char *argv[]){
	
	setlocale(LC_ALL, "Portuguese");
	
	/*Listen File Descriptor (listenfd) and Conection File Descriptor (connfd)*/

	int listenfd = 0, connfd = 0, sktbind = 0, sktlisten = 0;
	struct sockaddr_in serv_addr; // por alocação automática
	
	printf(TITULO);

	/* Zera a struct*/
	listenfd = socket(AF_INET, SOCK_STREAM,6); // AF_INET  Arpa Internet Protocols, SOCK_STREAM socket por stream, 0  Internet Protocol
	
	if (listenfd == -1) // verifica se ocorreu erro na criação do socket descritor
	{
		printf("\033[41mErro: Criar socket descritor.\033[40m\n");
	}else
		printf("Criado socket descritor!\n");
	
	
	memset(&serv_addr, 0, sizeof(serv_addr)); // ou poderia usar o bzero


	/*Instancia os campos do Struct*/
	serv_addr.sin_family = AF_INET; // familia
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY); // endereço
	serv_addr.sin_port = htons(PORTA); // porta

	/* Associa um endereço ao descritor do socket */
	sktbind = bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)); // associa esse soquete a um porta na sua máquina local
	
	char servidor_IP[80];
	inet_ntop(AF_INET, &serv_addr.sin_addr, servidor_IP, 80);
	printf("Socket IP:PORTA = %s, %d\n", servidor_IP, ntohs(serv_addr.sin_port));
	
	
	if (sktbind == -1) // verifica se ocorreu erro na associção do socket a um endereço
	{
		printf("\033[41mErro: Bind socket.\033[40m\n");
	}else
		printf("Bind socket!\n");	
	
	sktlisten = listen(listenfd, BACKLOG);	// fila para escutar os clientes da conexão socket
	
	if (sktlisten == -1) // verifica se ocorreu erro na fila
	{
		printf("\033[41mErro: Listen socket.\033[40m\n");
	}else
		printf("Listen socket!\n");	

	
	getcwd(dir_Raiz, BYTE); // getcwd - obtém o nome do caminho do diretório de trabalho Raiz 
	
	while(1){
		
		printf("Aguardando conexão.\n\n");
		
		while(connfd = accept(listenfd, (struct sockaddr*)NULL,NULL))
		{
			pthread_t Thread_Cliente;
			
			pthread_create(&Thread_Cliente, NULL, Thread_Conexao, &connfd);

			sleep(1);
		}

	}
}




void* Thread_Conexao(void *Con_socket)
{
	int connfd = *(int*)Con_socket;
	char sendBuff[BYTE];
	char recvBuff[BYTE];
	int tamBuff=0;
	char dir_Caminho[BYTE];
	int *id_socket;
	
	
	id_socket = malloc(sizeof(byte)); // aloca memoria para o conteudo do endereço ID do socket.
	*id_socket = connfd; // atribuir o endereço ID do socket, pois connfd carrega em seu conteúdo o endereço de memória do socket e não o ID.
	//*id_socket = 0xffffcbd4; // atribuir o endereço ID do socket, pois connfd carrega em seu conteúdo o endereço de memória do socket e não o ID.
			
	printf("Cliente %i conectado! \n",id_socket);
			
	stpcpy(dir_Caminho,dir_Raiz);
	//dir_Atual = opendir(dir_Caminho);	// opendir - abre um diretório

	printf(">Diretório atual: %s\n\n",dir_Caminho);
	
	memset(sendBuff, 0, sizeof(sendBuff)); // preenche área de memoria com 0
	memset(recvBuff, 0, sizeof(recvBuff)); // preenche área de memoria com 0

	snprintf(sendBuff, sizeof(sendBuff), "Conectado!\n> Diretório atual: %s\n",dir_Caminho);
	send(connfd,sendBuff,strlen(sendBuff), 0);

		do
		{
			
			tamBuff = recv(connfd,recvBuff,BYTE, 0);
			recvBuff[tamBuff] = 0x00;
			retENT(recvBuff);

			
			if (tamBuff < 0) // erro na recepção de mensagem
			{
				printf("\033[41mErro: Buffer de entrada.\033[40m\n");
				snprintf(recvBuff, sizeof(recvBuff), "sair");
				tamBuff = strlen(recvBuff);

			}else
			
			if (strcmp(recvBuff,"cdir") == 0)
			{
				printf("> (%i) %s\n",id_socket,recvBuff);
				Criar_DIR(connfd,dir_Caminho);
			}else
			
			if (strcmp(recvBuff,"rdir") == 0)
			{
				printf("> (%i) %s\n",id_socket,recvBuff);
				Remover_DIR(connfd,dir_Caminho);
			}else
			
			if (strcmp(recvBuff,"edir") == 0)
			{
				printf("> (%i) %s\n",id_socket,recvBuff);
				Entrar_DIR(connfd,dir_Caminho);
			}else
				
			if (strcmp(recvBuff,"sdir") == 0)
			{
				printf("> (%i) %s\n",id_socket,recvBuff);
				Sair_DIR(connfd,dir_Caminho);
			}else

			if (strcmp(recvBuff,"mdir") == 0)
			{
				printf("> (%i) %s\n",id_socket,recvBuff);
				Mostrar_DIR(connfd,dir_Caminho);
			}else
			
			if (strcmp(recvBuff,"cfile") == 0)
			{
				printf("> (%i) %s\n",id_socket,recvBuff);
				Criar_FILE(connfd);
			}else
			
			if (strcmp(recvBuff,"rfile") == 0)
			{
				printf("> (%i) %s\n",id_socket,recvBuff);
				Remover_FILE(connfd);
			}else
			
			if (strcmp(recvBuff,"efile") == 0)
			{
				printf("> (%i) %s\n",id_socket,recvBuff);
				Escrever_FILE(connfd);
			}else

			if (strcmp(recvBuff,"mfile") == 0)
			{
				printf("> (%i) %s\n",id_socket,recvBuff);
				Mostrar_FILE(connfd);
			}else
			
			if (strcmp(recvBuff,"cmd") == 0)
			{
				printf("> (%i) %s\n",id_socket,recvBuff);
				CMD(connfd);		
			}else
			
			if (strcmp(recvBuff,"-h") == 0)
			{
				printf("> (%i) %s\n",id_socket,recvBuff);
				Ajuda(connfd);
			}else
				
			if (strcmp(recvBuff,"sair") == 0)
			{
				printf("> (%i) %s\n",id_socket,recvBuff);	
			}else
				{
					printf("> (%i) Cliente digitou comando inválido.\n",id_socket);
					Invalido(connfd);				
				}
		}while(strcmp(recvBuff,"sair") != 0);

	printf("> Cliente %i desconectado.\n", id_socket);
	
	close(connfd);
		
	pthread_exit(0);

}




void Ajuda(int connfd)
{
	char sendBuff[BYTE];
	char recvBuff[BYTE];
	int tamBuff=0;
	
	snprintf(sendBuff, sizeof(sendBuff), "\n\t╔══\033[43m AJUDA \033[40m════════════════X═╗\n\t║ cdir    -  cria diretório ║\n\t║ rdir    -remove diretório ║\n\t║ edir    - entra diretório ║\n\t║ sdir    -  sair diretório ║\n\t║ mdir    -mostra diretório ║\n\t║ cfile   -    cria arquivo ║\n\t║ rfile   -  remove arquivo ║\n\t║ efile   - escreve arquivo ║\n\t║ mfile   -  mostra arquivo ║\n\t║ cmd     -  comando prompt ║\n\t║ sair    -        encerrar ║\n\t╚═══════════════════════════╝\n");
	send(connfd,sendBuff,strlen(sendBuff), 0);


/*
185 ╣	186 ║	187 ╗	188 ╝	200 ╚	201 ╔	202 ╩	203 ╦	204 ╠	205 ═	206 ╬
*/	

}




void Criar_DIR(int connfd, char *dir_Caminho)
{
	char sendBuff[BYTE];
	char recvBuff[BYTE];
	int tamBuff=0;

	if (chdir(dir_Caminho) == 0)	// chdir - altera o diretório de trabalho
	{
		snprintf(sendBuff, sizeof(sendBuff), "\033[44mCriar diretório, digite o nome:\033[40m\n");
		send(connfd,sendBuff,strlen(sendBuff), 0);
		
		tamBuff = recv(connfd,recvBuff,BYTE, 0);
		recvBuff[tamBuff] = 0x00;
		retENT(recvBuff);
		
		char comando[BYTE]  = "mkdir ";
		strcat(comando,recvBuff);
			
		if (system(comando) == 0)
		{
			snprintf(sendBuff, sizeof(sendBuff), "\033[42mDiretório criado com sucesso.\033[40m\n");
			send(connfd,sendBuff,strlen(sendBuff), 0);
		}else
			{			
			snprintf(sendBuff, sizeof(sendBuff), "\033[41mErro ao criar diretório.\033[40m\n");
			send(connfd,sendBuff,strlen(sendBuff), 0);
			}
	}else
		{			
		stpcpy(dir_Caminho,dir_Raiz);
		snprintf(sendBuff, sizeof(sendBuff), "\033[41mErro ao acessar diretório atual. Redirecionado a Raiz.\033[40m\n");
		send(connfd,sendBuff,strlen(sendBuff), 0);
		}
}



void Remover_DIR(int connfd, char *dir_Caminho)
{
	char sendBuff[BYTE];
	char recvBuff[BYTE];
	int tamBuff=0;

	if (chdir(dir_Caminho) == 0)	// chdir - altera o diretório de trabalho
	{	
		snprintf(sendBuff, sizeof(sendBuff), "\033[44mRemover diretório, digite o nome:\033[40m\n");
		send(connfd,sendBuff,strlen(sendBuff), 0);
		
		tamBuff = recv(connfd,recvBuff,BYTE, 0);
		recvBuff[tamBuff] = 0x00;
		retENT(recvBuff);
		
		char comando[BYTE]  = "rmdir ";
		strcat(comando,recvBuff);
			
		if (system(comando) == 0)
		{
			snprintf(sendBuff, sizeof(sendBuff), "\033[42mDiretório removido com sucesso.\033[40m\n");
			send(connfd,sendBuff,strlen(sendBuff), 0);
		}else
			{			
			snprintf(sendBuff, sizeof(sendBuff), "\033[41mErro ao remover diretório.\033[40m\n");
			send(connfd,sendBuff,strlen(sendBuff), 0);
			}
	}else
		{			
		stpcpy(dir_Caminho,dir_Raiz);
		snprintf(sendBuff, sizeof(sendBuff), "\033[41mErro ao acessar diretório atual. Redirecionado a Raiz.\033[40m\n");
		send(connfd,sendBuff,strlen(sendBuff), 0);
		}			
}  



void Entrar_DIR(int connfd, char *dir_Caminho)
{
	char sendBuff[BYTE];
	char recvBuff[BYTE];
	char pasta[BYTE];
	int tamBuff=0;

	if (chdir(dir_Caminho) == 0)	// chdir - altera o diretório de trabalho
	{	
		snprintf(sendBuff, sizeof(sendBuff), "\033[44mEntrar em diretório, digite o nome:\033[40m\n");
		send(connfd,sendBuff,strlen(sendBuff), 0);
		
		tamBuff = recv(connfd,recvBuff,BYTE, 0);
		recvBuff[tamBuff] = 0x00;
		retENT(recvBuff);
		
		memset(pasta, 0, sizeof(pasta)); // preenche área de memoria com 0
		
		strcpy(pasta,dir_Caminho);
		strcat(pasta,"/");
		strcat(pasta,recvBuff);
		
		if (chdir(pasta) == 0)	// chdir - altera o diretório de trabalho
		{
			getcwd(dir_Caminho, BYTE); // getcwd - obtém o nome do caminho do diretório de trabalho atual
			printf("Entrou no diretório> %s \n",dir_Caminho);
			
			snprintf(sendBuff, sizeof(sendBuff), "\033[42mDiretório atual:\033[40m %s\n",dir_Caminho);
			send(connfd,sendBuff,strlen(sendBuff), 0);
		}else
			{			
			snprintf(sendBuff, sizeof(sendBuff), "\033[41mErro ao entrar em diretório.\033[40m\n");
			send(connfd,sendBuff,strlen(sendBuff), 0);
			}
	}else
		{			
		stpcpy(dir_Caminho,dir_Raiz);
		snprintf(sendBuff, sizeof(sendBuff), "\033[41mErro ao acessar diretório atual. Redirecionado a Raiz.\033[40m\n");
		send(connfd,sendBuff,strlen(sendBuff), 0);
		}
} 



void Sair_DIR(int connfd, char *dir_Caminho)
{
	char sendBuff[BYTE];
	char recvBuff[BYTE];
	char pasta[BYTE];
	int tamBuff=0;

	if (chdir(dir_Caminho) == 0)	// chdir - altera o diretório de trabalho
	{	
		memset(pasta, 0, sizeof(pasta)); // preenche área de memoria com 0
		
		strcpy(pasta,dir_Caminho);
		strcat(pasta,"/..");
		
		if (chdir(pasta) == 0)	// chdir - altera o diretório de trabalho
		{
			getcwd(dir_Caminho, BYTE); // getcwd - obtém o nome do caminho do diretório de trabalho atual
			printf("Retornou no diretório> %s \n",dir_Caminho);
			
			snprintf(sendBuff, sizeof(sendBuff), "\033[42mDiretório atual >\033[40m %s\n",dir_Caminho);
			send(connfd,sendBuff,strlen(sendBuff), 0);
		}else
			{			
			snprintf(sendBuff, sizeof(sendBuff), "\033[41mErro ao retornar em diretório.\033[40m\n");
			send(connfd,sendBuff,strlen(sendBuff), 0);
			}
	}else
		{			
		stpcpy(dir_Caminho,dir_Raiz);
		snprintf(sendBuff, sizeof(sendBuff), "\033[41mErro ao acessar diretório atual. Redirecionado a Raiz.\033[40m\n");
		send(connfd,sendBuff,strlen(sendBuff), 0);
		}
}



void Mostrar_DIR(int connfd, char *dir_Caminho)
{
	char sendBuff[BYTE];
	char recvBuff[BYTE];
	DIR *dir_Atual = NULL;
	struct dirent *dir = NULL;

	
	if (chdir(dir_Caminho) == 0)	// chdir - altera o diretório de trabalho
	{	
		dir_Atual = opendir(dir_Caminho); 
		memset(sendBuff, 0, sizeof(sendBuff)); // preenche área de memoria com 0
		
		strcat(sendBuff,"\033[42mDiretótio >\033[40m ");
		strcat(sendBuff,dir_Caminho);
		strcat(sendBuff,"\n\n\t");
		while(dir = readdir(dir_Atual)){
			strcat(sendBuff, dir->d_name);
			strcat(sendBuff, "\t\n\t");
		}
		strcat(sendBuff, "\n");
		rewinddir(dir_Atual);	// rewinddir - redefine a posição de um fluxo de diretório para o início de um diretório
		send(connfd,sendBuff,strlen(sendBuff),0);
	}else
		{			
		stpcpy(dir_Caminho,dir_Raiz);
		snprintf(sendBuff, sizeof(sendBuff), "\033[41mErro ao acessar diretório atual. Redirecionado a Raiz.\033[40m\n");
		send(connfd,sendBuff,strlen(sendBuff), 0);
		}
} 



void Criar_FILE(int connfd)
{
	char sendBuff[BYTE];
	char recvBuff[BYTE];
	int tamBuff=0;

	
	snprintf(sendBuff, sizeof(sendBuff), "\033[44mCriar arquivo, digite o nome:\033[40m\n");
	send(connfd,sendBuff,strlen(sendBuff), 0);
	
	tamBuff = recv(connfd,recvBuff,BYTE, 0);
	recvBuff[tamBuff] = 0x00;
	retENT(recvBuff);
	
	char comando[BYTE]  = "touch ";
	strcat(comando,recvBuff);
		
	if (system(comando) == 0)
	{
		snprintf(sendBuff, sizeof(sendBuff), "\033[42mArquivo criado com sucesso.\033[40m\n");
		send(connfd,sendBuff,strlen(sendBuff), 0);
	}else
		{			
		snprintf(sendBuff, sizeof(sendBuff), "\033[41mErro ao criar arquivo.\033[40m\n");
		send(connfd,sendBuff,strlen(sendBuff), 0);
		}
}



void Remover_FILE(int connfd)
{
	char sendBuff[BYTE];
	char recvBuff[BYTE];
	int tamBuff=0;

	
	snprintf(sendBuff, sizeof(sendBuff), "\033[44mRemover arquivo, digite o nome:\033[40m\n");
	send(connfd,sendBuff,strlen(sendBuff), 0);
	
	tamBuff = recv(connfd,recvBuff,BYTE, 0);
	recvBuff[tamBuff] = 0x00;
	retENT(recvBuff);
	
	char comando[BYTE]  = "rm ";
	strcat(comando,recvBuff);
	
	if (system(comando) == 0)
	{
		snprintf(sendBuff, sizeof(sendBuff), "\033[42mArquivo removido com sucesso.\033[40m\n");
		send(connfd,sendBuff,strlen(sendBuff), 0);
	}else
		{			
		snprintf(sendBuff, sizeof(sendBuff), "\033[41mErro ao remover arquivo.\033[40m\n");
		send(connfd,sendBuff,strlen(sendBuff), 0);
		}
}  



void Escrever_FILE(int connfd)
{
	char sendBuff[BYTE];
	char recvBuff[BYTE];
	int tamBuff=0;
	
	
	snprintf(sendBuff, sizeof(sendBuff), "\033[44mEscrever em arquivo, digite o nome:\033[40m\n");
	send(connfd,sendBuff,strlen(sendBuff), 0);
	
	tamBuff = recv(connfd,recvBuff,BYTE, 0);
	recvBuff[tamBuff] = 0x00;
	retENT(recvBuff);
 
	FILE *arquivo; 
	
	if(arquivo = fopen(recvBuff,"a+")) // abertura como escrita no final e não cria novo arquivo se não existir
	{ 
		snprintf(sendBuff, sizeof(sendBuff), "\033[44mDigite o texto:\033[40m\n");
		send(connfd,sendBuff,strlen(sendBuff), 0);	
		
		tamBuff = recv(connfd,recvBuff,BYTE, 0);
		recvBuff[tamBuff] = 0x00;
		retENT(recvBuff);
		
		strcat(recvBuff,"\n");
		
		if(fprintf(arquivo,recvBuff) < 0)
		{
			snprintf(sendBuff, sizeof(sendBuff), "\033[41mNão foi possivel escrever no arquivo.\033[40m\n");
			send(connfd,sendBuff,strlen(sendBuff), 0);
		}else
			{
				snprintf(sendBuff, sizeof(sendBuff), "\033[42mArquivo salvo.\033[40m \n");
				send(connfd,sendBuff,strlen(sendBuff), 0);			
			}
		
	}else
		{			
		snprintf(sendBuff, sizeof(sendBuff), "\033[41mNão foi possivel abrir o arquivo.\033[40m\n");
		send(connfd,sendBuff,strlen(sendBuff), 0);
		}
	fclose(arquivo);
}



void Mostrar_FILE(int connfd)
{
	char sendBuff[BYTE];
	char recvBuff[BYTE];
	char conteudo[BYTE];
	int tamBuff=0;

	
	snprintf(sendBuff, sizeof(sendBuff), "\033[44mMostrar arquivo, digite o nome:\033[40m\n");
	send(connfd,sendBuff,strlen(sendBuff), 0);
	
	tamBuff = recv(connfd,recvBuff,BYTE, 0);
	recvBuff[tamBuff] = 0x00;
	retENT(recvBuff);
 
	FILE *arquivo; 
	
	if(arquivo = fopen(recvBuff,"r"))
	{ 
		memset(sendBuff, 0, sizeof(sendBuff)); // preenche área de memoria com 0
		memset(conteudo, 0, sizeof(sendBuff)); // preenche área de memoria com 0
		
		fread(conteudo, sizeof(char),BYTE-3,arquivo);
		strcat(sendBuff,"\n\n");
		strcat(sendBuff,conteudo);
		strcat(sendBuff,"\n");
		send(connfd,sendBuff,strlen(sendBuff), 0);
	}else
		{			
		snprintf(sendBuff, sizeof(sendBuff), "\033[41mNão foi possivel abrir o arquivo.\033[40m\n");
		send(connfd,sendBuff,strlen(sendBuff), 0);
		}
	fclose(arquivo);
}



void CMD(int connfd)
{
	char sendBuff[BYTE];
	char recvBuff[BYTE];
	int tamBuff=0;
	
	snprintf(sendBuff, sizeof(sendBuff), "\033[44mDigite o comando:\033[40m\n");
	send(connfd,sendBuff,strlen(sendBuff), 0);
	
	tamBuff = recv(connfd,recvBuff,BYTE, 0);
	recvBuff[tamBuff] = 0x00;
	retENT(recvBuff);

	char comando[BYTE]  = "";
	strcat(comando,recvBuff);
	
	if (system(comando) == 0)
	{
		snprintf(sendBuff, sizeof(sendBuff), "\033[42mOK\033[40m \n");
		send(connfd,sendBuff,strlen(sendBuff), 0);
	}else
		{			
		snprintf(sendBuff, sizeof(sendBuff), "\033[41mErro ao chamar comando.\033[40m\n");
		send(connfd,sendBuff,strlen(sendBuff), 0);
		}
		
}



void Invalido(int connfd)
{
	char sendBuff[BYTE];
	char recvBuff[BYTE];
	int tamBuff=0;
	
	snprintf(sendBuff, sizeof(sendBuff), "\033[41mComando inválido. Ajuda -h\033[40m\n");
	send(connfd,sendBuff,strlen(sendBuff), 0);
}



void retENT(char *recvBuff)	// remove ENTER do final do buffer
{
	
	if (recvBuff[strlen(recvBuff)-1] == 10)
		recvBuff[strlen(recvBuff)-1] = 0x00;	
}


/*

		Em linux:

		cd home/
		gcc -o servidor.exe servidor.c
		./servidor.exe
	----------------------------------------------
		Em Windows:

		cd \
		cd cygwin64
		cd home
		ls
		gcc servidor.c -o servidor.exe
		servidor.exe
		
		\033[41m Erro ao chamar comando. \033[40m

*/
