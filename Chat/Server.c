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

#define MAX_USER 100

int portNum = 4444;



void errorHandler(char *msg);

void sendMessage(char msg[], int lenth);

void * handlingClient(void * msg);

void dbConnect(char * msg);

//void push();



int clientCount = 0;

int clientSocks[MAX_USER] = {0, };



pthread_mutex_t mutex;



int main(int argc, char *argv[]){



    printf("=======================서버 가동=======================\n");



    // 서버와 클라이언트의 소켓 변수

    int serverSock, clientScok;



    //쓰레드 선언

    pthread_t threadId;



    // 소켓의 주소를 담는 기본 구조체 역할

    struct sockaddr_in servAddr;

    struct sockaddr_in clntAddr;

    int clientAddrSize;



    // mutex 초기화

    pthread_mutex_init(&mutex, NULL);



    // 서버 소켓 생성

    serverSock = socket(PF_INET, SOCK_STREAM, 0);

    

    // 소켓 생성 오류 발생시 처리

    if (serverSock == -1)

    {

        errorHandler("socket() error!");

    }

    

    // 주소정보 초기화

    memset(&servAddr, 0, sizeof(servAddr));

    servAddr.sin_family = AF_INET;

    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);

    servAddr.sin_port = htons(portNum);



    // 주소 정보 할당 및 오류 발생시 처리

    if  (bind(serverSock, (struct sockaddr*)&servAddr, sizeof(servAddr)) == -1) {

        errorHandler("bind() error!!");

    }



    // 연결 요청 대기 및 오류 발생시 처리

    if (listen(serverSock, 5)==-1) {

        errorHandler("listen() error!!");

    }

    

    // 요청이 들어오면 허용하는 무한 반복문

    while (1)

    {

        clientAddrSize = sizeof(clntAddr);

        clientScok = accept(serverSock, (struct sockaddr*)&clntAddr, &clientAddrSize);



        // 임계영역 설정

        pthread_mutex_lock(&mutex);



        clientSocks[clientCount++] = clientScok;

        

        pthread_mutex_unlock(&mutex);



        // 쓰레드 생성

        pthread_create(&threadId, NULL, handlingClient, (void*)&clientScok);

        pthread_detach(threadId);

        printf("conneted client IP: %s\n", inet_ntoa(clntAddr.sin_addr));



    }



    close(serverSock);

    return 0;



}



void *handlingClient(void *msg){

    int clientSock = *((int*)msg);

    int strLen = 0;

    int i;

    char message[BUF_SIZE] = {0, };



    //strLen = read(clientSock, message, sizeof(message));

    

    // 클라이언트가 EOF를 전송할 때까지 무한 반복

    // EOF 값이 오면 반환으로 0이 온다.

    while ((strLen = read(clientSock, message, sizeof(message))) != 0)

    {   

        sendMessage(message, strLen);



    }





    pthread_mutex_lock(&mutex);

    

    for(int i=0; i<clientSock; i++) {

        if(clientSock == clientSocks[i]) {

            while(++i < clientSock) {

                clientSocks[i - 1] = clientSocks[i];

            }

            break;

        }    

    }

    clientSocks[clientCount] = 0;

    clientCount--;

    pthread_mutex_unlock(&mutex);

    close(clientSock);

    return NULL;   

    

}



// 연결된 모든 클라이언트에게 메시지를 전송

void sendMessage(char *msg, int lenth) {



    int i;



    //printf("send 함수 진입\n");

    //printf("%s\n", message);



    pthread_mutex_lock(&mutex);

    for (i = 0; i < clientCount; i++)

    {

        write(clientSocks[i], msg, lenth);

    }



    // db에 데이터 저장

    dbConnect(msg);



    pthread_mutex_unlock(&mutex);

}



void errorHandler(char *msg) {



    fputs(msg, stderr);

    fputc('\n', stderr);

    exit(1);



}   



void dbConnect (char * msg) {

    

    // 몽고 db를 연동하기 위한 구조체 

    // 한번에 하나의 스레드를 사용?

    mongoc_client_t *client;

    mongoc_collection_t *collection;

    mongoc_database_t *database;

    bson_error_t error;

    bson_oid_t oid;

    bson_t *doc;



    // 몽고 db c driver 초기화 및 db 클라이언트 생성

    mongoc_init();

    client = mongoc_client_new (SERVER);

    //mongoc_client_set_appname(client, "insert_example");



    database = mongoc_client_get_database(client, "chat");



    // 컬렉션에 접근할 객체 생성

    collection = mongoc_client_get_collection(client, "chat", "chat_col");

    

    // 도큐먼트(BSON 객체) 생성

    doc = bson_new();

    bson_oid_init(&oid, NULL);

    BSON_APPEND_OID(doc, "_id", &oid);

    BSON_APPEND_UTF8(doc, "msg", msg);



    // INSERT 수행

    if (!mongoc_collection_insert (collection, MONGOC_INSERT_NONE, doc, NULL, &error))

    {

        // INSERT 실패 시 에러 문구 출력

        fprintf(stderr, "%s\n", error.message);

    }



    // 할당된 메모리 해제

    bson_destroy(doc);



    // 콜렉션 설정 해제

    mongoc_collection_destroy(collection);



    // 몽고 클라이언트 해제

    mongoc_client_destroy(client);



    // 몽고 C driver 사용 후 남아있는 할당메모리 해제

    mongoc_cleanup();



}