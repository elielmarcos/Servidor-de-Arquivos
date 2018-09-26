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

void Ajuda(int connfd);
void Criar_DIR(int connfd);
void Remover_DIR(int connfd);
void Entrar_DIR(int connfd, DIR **current_dir);
void Sair_DIR(int connfd, DIR **current_dir);
void Mostrar_DIR(int connfd, DIR *current_dir);
void Criar_FILE(int connfd);
void Remover_FILE(int connfd);
void Escrever_FILE(int connfd);
void Mostrar_FILE(int connfd);
void CMD(int connfd);
void Invalido(int connfd);
void retENT(char *recvBuff);



int main(int argc, char *argv[]){
	
	setlocale(LC_ALL, "Portuguese");
	
	/*Listen File Descriptor (listenfd) and Conection File Descriptor (connfd)*/

	int listenfd = 0, connfd = 0, sktbind = 0, sktlisten = 0, *id_socket;
	struct sockaddr_in serv_addr; // por alocação automática

	char sendBuff[BYTE];
	char recvBuff[BYTE];
	char current_dir_name[BYTE];
	int tamBuff=0;

	/* Zera a struct*/
	listenfd = socket(AF_INET, SOCK_STREAM,6); // AF_INET  Arpa Internet Protocols, SOCK_STREAM socket por stream, 0  Internet Protocol
	
	if (listenfd == -1) // verifica se ocorreu erro na criação do socket descritor
	{
		printf("Erro: Criar socket descritor.\n");
	}else
		printf("Criado socket descritor!\n");
	
	
	memset(&serv_addr, 0, sizeof(serv_addr)); // ou poderia usar o bzero
	memset(sendBuff, 0, sizeof(sendBuff)); // preenche área de memoria com 0
	memset(recvBuff, 0, sizeof(recvBuff)); // preenche área de memoria com 0

	/*Instancia os campos do Struct*/
	serv_addr.sin_family = AF_INET; // familia
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY); // endereço
	serv_addr.sin_port = htons(PORTA); // porta

	/* Associa um endereço ao descritor do socket */
	sktbind = bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)); // associa esse soquete a um porta na sua máquina local
	
	char mybuf[80];
	inet_ntop(AF_INET, &serv_addr.sin_addr, mybuf, 80);
	printf("Socket IP:PORTA = %d, %s, %d\n", serv_addr.sin_family, mybuf, ntohs(serv_addr.sin_port));
	
	
	if (sktbind == -1) // verifica se ocorreu erro na associção do socket a um endereço
	{
		printf("Erro: Bind socket.\n");
	}else
		printf("Bind socket!\n");	
	
	sktlisten = listen(listenfd, BACKLOG);	// fila para escutar os clientes da conexão socket
	
	if (sktlisten == -1) // verifica se ocorreu erro na fila
	{
		printf("Erro: Listen socket.\n");
	}else
		printf("Listen socket!\n");	

	
	
	while(1){
		
		printf("Aguardando conexão.\n");
		
		while(connfd = accept(listenfd, (struct sockaddr*)NULL, NULL))
		{
			id_socket = malloc(sizeof(byte)); // aloca memoria para o conteudo do endereço ID do socket.
			*id_socket = 0xffffcbd4; // atribuir o endereço ID do socket, pois connfd carrega em seu conteúdo o endereço de memória do socket e não o ID.
			
			printf("Cliente %i conectado! \n",id_socket);
			//printf("%x\n",&connfd);
			
			DIR *current_dir = NULL;
			char current_dir_name[BYTE];

			getcwd(current_dir_name, sizeof(current_dir_name)); // getcwd - obtém o nome do caminho do diretório de trabalho atual
			current_dir = opendir(current_dir_name);	// opendir - abre um diretório

			printf(">Diretório atual: %s\n\n",current_dir_name);

			snprintf(sendBuff, sizeof(sendBuff), "Conectado!\n> Diretório atual: %s\n",current_dir_name);
			send(connfd,sendBuff,strlen(sendBuff), 0);
		
			do
			{
				
				tamBuff = recv(connfd,recvBuff,BYTE, 0);
				recvBuff[tamBuff] = 0x00;
				
				retENT(recvBuff);

				
				if (tamBuff < 0) // erro na recepção de mensagem
				{
					printf("Erro: Buffer de entrada.\n");
					snprintf(recvBuff, sizeof(recvBuff), "sair");
					tamBuff = strlen(recvBuff);

				}else
				
				if (strcmp(recvBuff,"cdir") == 0)
				{
					printf("> %s\n",recvBuff);
					Criar_DIR(connfd);
				}else
				
				if (strcmp(recvBuff,"rdir") == 0)
				{
					printf("> %s\n",recvBuff);
					Remover_DIR(connfd);
				}else
				
				if (strcmp(recvBuff,"edir") == 0)
				{
					printf("> %s\n",recvBuff);
					Entrar_DIR(connfd,&current_dir);
				}else
					
				if (strcmp(recvBuff,"sdir") == 0)
				{
					printf("> %s\n",recvBuff);
					Sair_DIR(connfd,&current_dir);
				}else

				if (strcmp(recvBuff,"mdir") == 0)
				{
					printf("> %s\n",recvBuff);
					Mostrar_DIR(connfd,current_dir);
				}else
				
				if (strcmp(recvBuff,"cfile") == 0)
				{
					printf("> %s\n",recvBuff);
					Criar_FILE(connfd);
				}else
				
				if (strcmp(recvBuff,"rfile") == 0)
				{
					printf("> %s\n",recvBuff);
					Remover_FILE(connfd);
				}else
				
				if (strcmp(recvBuff,"efile") == 0)
				{
					printf("> %s\n",recvBuff);
					Escrever_FILE(connfd);
				}else

				if (strcmp(recvBuff,"mfile") == 0)
				{
					printf("> %s\n",recvBuff);
					Mostrar_FILE(connfd);
				}else
				
				if (strcmp(recvBuff,"cmd") == 0)
				{
					printf("> %s\n",recvBuff);
					CMD(connfd);		
				}else
				
				if (strcmp(recvBuff,"-h") == 0)
				{
					printf("> %s\n",recvBuff);
					Ajuda(connfd);
				}else
					
				if (strcmp(recvBuff,"sair") == 0)
				{
					printf("> %s\n",recvBuff);		
				}else
					{
						printf("> Cliente digitou comando inválido.\n");
						Invalido(connfd);				
					}
			}while(strcmp(recvBuff,"sair") != 0);
		
			printf("Cliente %i desconectado.\n", id_socket);
			close(connfd);
			sleep(1);
		}

	}
}




void Ajuda(int connfd)
{
	char sendBuff[BYTE];
	char recvBuff[BYTE];
	int tamBuff=0;
	
	snprintf(sendBuff, sizeof(sendBuff), "\n\t ########## AJUDA ##########\n\t  cdir    -  cria diretório\n\t  rdir    -remove diretório\n\t  edir    - entra diretório\n\t  sdir    -  sair diretório\n\t  mdir    -mostra diretório\n\t  cfile   -    cria arquivo\n\t  rfile   -  remove arquivo\n\t  efile   - escreve arquivo\n\t  mfile   -  mostra arquivo\n\t  cmd     -  comando prompt\n\t  sair    -        encerrar\n\t ###########################\n");
	send(connfd,sendBuff,strlen(sendBuff), 0);
}



void Criar_DIR(int connfd)
{
	char sendBuff[BYTE];
	char recvBuff[BYTE];
	//char current_dir_name[BYTE];
	int tamBuff=0;

	//getcwd(current_dir_name, sizeof(current_dir_name));
	//printf("%s\n",current_dir_name);
	
	snprintf(sendBuff, sizeof(sendBuff), "Criar diretório, digite o nome: \n");
	send(connfd,sendBuff,strlen(sendBuff), 0);
	
	tamBuff = recv(connfd,recvBuff,BYTE, 0);
	recvBuff[tamBuff] = 0x00;
	retENT(recvBuff);
	
	char comando[BYTE]  = "mkdir ";
	strcat(comando,recvBuff);
		
	if (system(comando) == 0)
	{
		snprintf(sendBuff, sizeof(sendBuff), "Diretório criado com sucesso. \n");
		send(connfd,sendBuff,strlen(sendBuff), 0);
	}else
		{			
		snprintf(sendBuff, sizeof(sendBuff), " Erro ao criar diretório. \n");
		send(connfd,sendBuff,strlen(sendBuff), 0);
		}
}



void Remover_DIR(int connfd)
{
	char sendBuff[BYTE];
	char recvBuff[BYTE];
	//char current_dir_name[BYTE];
	int tamBuff=0;

	//getcwd(current_dir_name, sizeof(current_dir_name));
	//printf("%s\n",current_dir_name);
	
	snprintf(sendBuff, sizeof(sendBuff), "Remover diretório, digite o nome: \n");
	send(connfd,sendBuff,strlen(sendBuff), 0);
	
	tamBuff = recv(connfd,recvBuff,BYTE, 0);
	recvBuff[tamBuff] = 0x00;
	retENT(recvBuff);
	
	char comando[BYTE]  = "rmdir ";
	strcat(comando,recvBuff);
		
	if (system(comando) == 0)
	{
		snprintf(sendBuff, sizeof(sendBuff), "Diretório removido com sucesso. \n");
		send(connfd,sendBuff,strlen(sendBuff), 0);
	}else
		{			
		snprintf(sendBuff, sizeof(sendBuff), " Erro ao remover diretório. \n");
		send(connfd,sendBuff,strlen(sendBuff), 0);
		}	
}  



void Entrar_DIR(int connfd, DIR **current_dir)
{
	char sendBuff[BYTE];
	char recvBuff[BYTE];
	char current_dir_name[BYTE];
	char pasta[BYTE];
	int tamBuff=0;

	getcwd(current_dir_name, sizeof(current_dir_name));
	
	snprintf(sendBuff, sizeof(sendBuff), "Entrar em diretório, digite o nome: \n");
	send(connfd,sendBuff,strlen(sendBuff), 0);
	
	tamBuff = recv(connfd,recvBuff,BYTE, 0);
	recvBuff[tamBuff] = 0x00;
	retENT(recvBuff);
	
	memset(pasta, 0, sizeof(pasta)); // preenche área de memoria com 0
	
    strcpy(pasta,current_dir_name);
	strcat(pasta,"/");
	strcat(pasta,recvBuff);
	
    if (chdir(pasta) == 0)	// chdir - altera o diretório de trabalho
	{
        getcwd(current_dir_name, sizeof(current_dir_name)); // getcwd - obtém o nome do caminho do diretório de trabalho atual
        *current_dir = opendir(current_dir_name);	// opendir - abre um diretório
		printf("Entrou no diretório> %s \n",current_dir_name);
		
		snprintf(sendBuff, sizeof(sendBuff), "> Diretório atual: %s\n",current_dir_name);
		send(connfd,sendBuff,strlen(sendBuff), 0);
	}else
		{			
		snprintf(sendBuff, sizeof(sendBuff), " Erro ao entrar em diretório. \n");
		send(connfd,sendBuff,strlen(sendBuff), 0);
		}
} 



void Sair_DIR(int connfd, DIR **current_dir)
{
	char sendBuff[BYTE];
	char recvBuff[BYTE];
	char current_dir_name[BYTE];
	char pasta[BYTE];
	int tamBuff=0;

	getcwd(current_dir_name, sizeof(current_dir_name));
	
	memset(pasta, 0, sizeof(pasta)); // preenche área de memoria com 0
	
    strcpy(pasta,current_dir_name);
	strcat(pasta,"/..");
	
    if (chdir(pasta) == 0)	// chdir - altera o diretório de trabalho
	{
        getcwd(current_dir_name, sizeof(current_dir_name)); // getcwd - obtém o nome do caminho do diretório de trabalho atual
        *current_dir = opendir(current_dir_name);	// opendir - abre um diretório
		printf("Retornou no diretório> %s \n",current_dir_name);
		
		snprintf(sendBuff, sizeof(sendBuff), "Diretório atual > %s\n",current_dir_name);
		send(connfd,sendBuff,strlen(sendBuff), 0);
	}else
		{			
		snprintf(sendBuff, sizeof(sendBuff), " Erro ao retornar em diretório. \n");
		send(connfd,sendBuff,strlen(sendBuff), 0);
		}
}



void Mostrar_DIR(int connfd, DIR *current_dir)
{
	char sendBuff[BYTE];
	char recvBuff[BYTE];
	char current_dir_name[BYTE];
	struct dirent *dir = NULL;
	
	getcwd(current_dir_name, sizeof(current_dir_name));
	
	memset(sendBuff, 0, sizeof(sendBuff)); // preenche área de memoria com 0
	
	strcat(sendBuff,"Diretótio > ");
	strcat(sendBuff,current_dir_name);
	strcat(sendBuff,"\n\n\t");
	while(dir = readdir(current_dir)){
		strcat(sendBuff, dir->d_name);
		strcat(sendBuff, "\t\n\t");
	}
	strcat(sendBuff, "\n");
	rewinddir(current_dir);	// rewinddir - redefine a posição de um fluxo de diretório para o início de um diretório
	send(connfd,sendBuff,strlen(sendBuff),0);
} 



void Criar_FILE(int connfd)
{
	char sendBuff[BYTE];
	char recvBuff[BYTE];
	//char current_dir_name[BYTE];
	int tamBuff=0;

	//getcwd(current_dir_name, sizeof(current_dir_name));
	//printf("%s\n",current_dir_name);
	
	snprintf(sendBuff, sizeof(sendBuff), "Criar arquivo, digite o nome: \n");
	send(connfd,sendBuff,strlen(sendBuff), 0);
	
	tamBuff = recv(connfd,recvBuff,BYTE, 0);
	recvBuff[tamBuff] = 0x00;
	retENT(recvBuff);
	
	char comando[BYTE]  = "touch ";
	strcat(comando,recvBuff);
		
	if (system(comando) == 0)
	{
		snprintf(sendBuff, sizeof(sendBuff), "Arquivo criado com sucesso. \n");
		send(connfd,sendBuff,strlen(sendBuff), 0);
	}else
		{			
		snprintf(sendBuff, sizeof(sendBuff), " Erro ao criar arquivo. \n");
		send(connfd,sendBuff,strlen(sendBuff), 0);
		}
}



void Remover_FILE(int connfd)
{
	char sendBuff[BYTE];
	char recvBuff[BYTE];
	//char current_dir_name[BYTE];
	int tamBuff=0;

	//getcwd(current_dir_name, sizeof(current_dir_name));
	//printf("%s\n",current_dir_name);
	
	snprintf(sendBuff, sizeof(sendBuff), "Remover arquivo, digite o nome: \n");
	send(connfd,sendBuff,strlen(sendBuff), 0);
	
	tamBuff = recv(connfd,recvBuff,BYTE, 0);
	recvBuff[tamBuff] = 0x00;
	retENT(recvBuff);
	
	char comando[BYTE]  = "rm ";
	strcat(comando,recvBuff);
	
	if (system(comando) == 0)
	{
		snprintf(sendBuff, sizeof(sendBuff), "Arquivo removido com sucesso. \n");
		send(connfd,sendBuff,strlen(sendBuff), 0);
	}else
		{			
		snprintf(sendBuff, sizeof(sendBuff), "Erro ao remover arquivo. \n");
		send(connfd,sendBuff,strlen(sendBuff), 0);
		}
}  



void Escrever_FILE(int connfd)
{
	char sendBuff[BYTE];
	char recvBuff[BYTE];
	char conteudo[BYTE];
	//char current_dir_name[BYTE];
	int tamBuff=0;

	//getcwd(current_dir_name, sizeof(current_dir_name));
	//printf("%s\n",current_dir_name);
	
	snprintf(sendBuff, sizeof(sendBuff), "Escrever em arquivo, digite o nome: \n");
	send(connfd,sendBuff,strlen(sendBuff), 0);
	
	tamBuff = recv(connfd,recvBuff,BYTE, 0);
	recvBuff[tamBuff] = 0x00;
	retENT(recvBuff);
 
	FILE *arquivo; 
	
	if(arquivo = fopen(recvBuff,"a+")) // abertura como escrita no final e não cria novo arquivo se não existir
	{ 
		snprintf(sendBuff, sizeof(sendBuff), "Digite o texto: \n");
		send(connfd,sendBuff,strlen(sendBuff), 0);	
		
		tamBuff = recv(connfd,recvBuff,BYTE, 0);
		recvBuff[tamBuff] = 0x00;
		retENT(recvBuff);
		
		strcat(recvBuff,"\n");
		
		if(fprintf(arquivo,recvBuff) < 0)
		{
			snprintf(sendBuff, sizeof(sendBuff), "Não foi possivel escrever no arquivo.\n");
			send(connfd,sendBuff,strlen(sendBuff), 0);
		}else
			{
				snprintf(sendBuff, sizeof(sendBuff), "Arquivo salvo. \n");
				send(connfd,sendBuff,strlen(sendBuff), 0);			
			}
		
	}else
		{			
		snprintf(sendBuff, sizeof(sendBuff), "Não foi possivel abrir o arquivo.\n");
		send(connfd,sendBuff,strlen(sendBuff), 0);
		}
	fclose(arquivo);
}



void Mostrar_FILE(int connfd)
{
	char sendBuff[BYTE];
	char recvBuff[BYTE];
	char conteudo[BYTE];
	//char current_dir_name[BYTE];
	int tamBuff=0;

	//getcwd(current_dir_name, sizeof(current_dir_name));
	//printf("%s\n",current_dir_name);
	
	snprintf(sendBuff, sizeof(sendBuff), "Mostrar arquivo, digite o nome: \n");
	send(connfd,sendBuff,strlen(sendBuff), 0);
	
	tamBuff = recv(connfd,recvBuff,BYTE, 0);
	recvBuff[tamBuff] = 0x00;
	retENT(recvBuff);
 
	FILE *arquivo; 
	
	if(arquivo = fopen(recvBuff,"r"))
	{ 
		memset(sendBuff, 0, sizeof(sendBuff)); // preenche área de memoria com 0
		strcat(sendBuff,"\n");
		fread(sendBuff, sizeof(char),BYTE-2,arquivo);
		strcat(sendBuff,"\n");
		send(connfd,sendBuff,strlen(sendBuff), 0);
	}else
		{			
		snprintf(sendBuff, sizeof(sendBuff), "Não foi possivel abrir o arquivo.\n");
		send(connfd,sendBuff,strlen(sendBuff), 0);
		}
	fclose(arquivo);
}



void CMD(int connfd)
{
	char sendBuff[BYTE];
	char recvBuff[BYTE];
	int tamBuff=0;
	
	snprintf(sendBuff, sizeof(sendBuff), "Digite o comando: \n");
	send(connfd,sendBuff,strlen(sendBuff), 0);
	
	tamBuff = recv(connfd,recvBuff,BYTE, 0);
	recvBuff[tamBuff] = 0x00;
	retENT(recvBuff);

	char comando[BYTE]  = "";
	strcat(comando,recvBuff);
	
	if (system(comando) == 0)
	{
		snprintf(sendBuff, sizeof(sendBuff), "OK \n");
		send(connfd,sendBuff,strlen(sendBuff), 0);
	}else
		{			
		snprintf(sendBuff, sizeof(sendBuff), " Erro ao chamar comando. \n");
		send(connfd,sendBuff,strlen(sendBuff), 0);
		}
		
}



void Invalido(int connfd)
{
	char sendBuff[BYTE];
	char recvBuff[BYTE];
	int tamBuff=0;
	
	snprintf(sendBuff, sizeof(sendBuff), "Comando inválido. Ajuda -h \n");
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
