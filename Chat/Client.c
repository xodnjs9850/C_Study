#include <stdio.h>

#include <stdlib.h>

#include <string.h>

#include <unistd.h>

#include <arpa/inet.h>

#include <sys/socket.h>

#include <pthread.h>

#include <mongoc.h>

#include <bson.h>



#define SERVER "mongodb://localhost:27017/"

#define BUF_SIZE 1024



void errorHandler(char *msg);

void * sendMessage(void * msg);

void * receiveMessage(void * msg);

void * dbConnect(void * msg);



// 접속할 서버의 ip와 port 정보

char ipAddr[50] = "127.0.0.1";

int portNum = 4444;



// 채팅 메시지 배열

char message[BUF_SIZE];

char userName[20];



int main(int argc, char* argv[]) {



    int sock;

    struct sockaddr_in serv_adr;

    

    // 두가지 쓰레드를 사용한 이유는

    // 클라이언트는 언제나 메시지를 보낼 수 있어야하며

    // 서버가 메시지를 보낼 경우 바로 출력을 해야한다.

    // 즉, 두가지의 함수가 동시에 돌아야한다.

    pthread_t sendThread, recvThread;

    void * threadReturn;



    // 소켓 생성 및 오류 발생 시 처리

    sock = socket(PF_INET, SOCK_STREAM, 0);

    if(sock == -1) {

        errorHandler("socket() error!");

    }



    // 소켓 정보 초기화

    memset(&serv_adr, 0, sizeof(serv_adr));

    serv_adr.sin_family = AF_INET;

    serv_adr.sin_addr.s_addr = inet_addr(ipAddr);

    serv_adr.sin_port = htons(portNum);



    // 서버에 연결 요청 전송 및 오류 발생 시 처리

    if (connect(sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr)) == -1) {

        errorHandler("connect() error!!");

    } 



    printf("=================채팅 프로그램=================\n");

    printf("사용할 ID를 입력하세요. ");

    printf(">> ");

    scanf("%s", userName);



    // 쓰레드 생성

    pthread_create(&sendThread, NULL, sendMessage, (void*)&sock);

    pthread_create(&recvThread, NULL, receiveMessage, (void*)&sock);

    pthread_join(sendThread, &threadReturn);

    pthread_join(recvThread, &threadReturn);

    

    close(sock);

    return 0;



}



void * dbConnect(void * msg) {

    // 몽고 db를 연동하기 위한 구조체 

    // 한번에 하나의 스레드를 사용?

    mongoc_client_t *client;

    mongoc_collection_t *collection;

    mongoc_cursor_t *cursor;

    bson_error_t error;

    const bson_t *doc;

    bson_t *query;

    char *str;



    bson_iter_t iter;

    bson_iter_t value;



    // 몽고 db c driver 초기화 및 db 클라이언트 생성

    mongoc_init();

    client = mongoc_client_new (SERVER);



    // 컬렉션에 접근할 객체 생성

    collection = mongoc_client_get_collection(client, "chat", "chat_col");



    query = bson_new ();

    cursor = mongoc_collection_find_with_opts (collection, query, NULL, NULL);



    while (mongoc_cursor_next (cursor, &doc))

    {

        bson_iter_init(&iter, doc);

        bson_iter_find_descendant (&iter, "msg", &value);

        printf ("%s\n", bson_iter_utf8(&value, NULL));

        bson_free (str);

    }

    

    // 할당된 메모리 해제

    bson_destroy(query);



    // 할당된 메모리 해제

    mongoc_cursor_destroy(cursor);



    // 콜렉션 설정 해제

    mongoc_collection_destroy(collection);



    // 몽고 클라이언트 해제

    mongoc_client_destroy(client);



    // 몽고 C driver 사용 후 남아있는 할당메모리 해제

    mongoc_cleanup();

}







// 메시지 전송 함수

void * sendMessage(void * msg) {



    dbConnect(msg);



    fputs("메시지를 입력하세요.(exit를 입력하면 종료)\n >> ", stdout);

    fgets(message, BUF_SIZE, stdin);



    int sock = *((int *)msg);

    char userMsg[20 + BUF_SIZE];

    

    while (1)

    {

        fgets(message, BUF_SIZE, stdin);



        if (!strcmp(message, "exit\n")) {

            close(sock);

            exit(0);

        }

        

        // 메시지를 보낸 유저 이름과 메시지를 같이 저장

        sprintf(userMsg, "[%s] : %s", userName, message);

        

        write(sock, userMsg, strlen(userMsg));



    }



    return 0;



}



// 서버가 전송한 메시지를 받는 함수

void * receiveMessage(void * msg) {

    int sock= *((int *)msg);

    char userMsg[20 + BUF_SIZE];

    int strLen;



    while (1)

    {

        strLen=read(sock, userMsg, 20 + BUF_SIZE - 1);

        if(strLen == -1) {

            return (void*)-1;

        }



        userMsg[strLen] = 0;

        fputs(userMsg, stdout);

    }



    return NULL;

    

}



void errorHandler(char *msg) {

    fputs(msg, stderr);

    fputc('\n', stderr);

    exit(1);

}



