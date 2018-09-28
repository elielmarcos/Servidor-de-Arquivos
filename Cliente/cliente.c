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



#define SERVER_IP "127.0.0.1"
#define BYTE 16384
#define PORTA 5000
#define TITULO "\n ====\033[43m EXPLORADOR DE ARQUIVOS CLIENTE \033[40m====\n\n"

void Aguarde(void);


int main(int argc, char *argv[])
{

	setlocale(LC_ALL, "Portuguese");
    char mensagem[BYTE], *loc;
    int tbuf, skt, escolha;
    struct sockaddr_in serv;

    /**INICIALIZA ESTRUTURA SOCKETS*/
    skt = socket(AF_INET, SOCK_STREAM, 6);
    serv.sin_family = AF_INET;
    serv.sin_addr.s_addr = inet_addr(SERVER_IP);
    serv.sin_port = htons (PORTA);
    memset (&(serv.sin_zero), 0x00, sizeof (serv.sin_zero));

    /**INICIA COMUNICAÇÃO COM SERVIDOR*/
    while(connect (skt, (struct sockaddr *)&serv, sizeof (struct sockaddr)) != 0){
        Aguarde();      ///AGUARDA SERVIDOR SE COMUNICAR
    }
	system("clear");
	printf(TITULO);
    printf("> A Conexão com o Servidor %s foi estabelecida na porta %d\n\n",SERVER_IP,PORTA);
    printf("> Envie sair pra encerrar ou -h para ajuda \n\n");


    /**RECEBE MENSAGEM DO SERVIDOR*/
    tbuf = recv (skt, mensagem, BYTE, 0);
    mensagem[tbuf] = 0x00;
    printf ("> %s\n",mensagem);


    /**LOOP DE COMUNICAÇÃO ENTRE CLIENTE E SERVIDOR*/
    do{
        ///envia
        printf("> ");
        gets(mensagem);
		if (strlen(mensagem)==0) strcpy(mensagem," ");
        send(skt, mensagem, strlen(mensagem), 0);

		if (strcmp(mensagem,"sair")!= 0)
		{
			///recebe
			tbuf = recv (skt, mensagem, BYTE, 0);
			mensagem[tbuf] = 0x00;
			printf ("> %s\n",mensagem);
		}

    }while(strcmp(mensagem,"sair")!= 0);    ///COMUNICAÇÃO SE ENCERRA QUANDO USUARIO DIGITAR sair


    /**FINALIZA CONEXÃO*/
    close(skt);
    printf (">> \033[43mA conexão com o servidor foi finalizada!!!\033[40m\n\n");
	sleep(3);
    exit(0);
}



/**************************************************************
*   FUNÇÃO RESPOSÁVEL POR IMPRIMIR MENSAGER NA TELA           *
*   ENQUANTO AGUARDA ALGUM SERVIDOR ESTABELECER COMUNICAÇÃO   *
***************************************************************/
void Aguarde(){
    int i=0;
    char ponto[12] = "";
    for(i=0; i<4;i++){
        system("clear");
        printf(TITULO);
        printf("\n\nProcurando servidor.");
        printf("\nAguarde %s\n\n", ponto);
        strcat(ponto,".");
        sleep(1);
    }
    strcpy(ponto, "");
}