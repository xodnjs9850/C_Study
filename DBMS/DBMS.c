#include <stdio.h>
#include <stdlib.h> 
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <dirent.h>

typedef struct users {
    char userName[20];
    struct users* nextUser;
    struct databases* database;
} users;

typedef struct databases {
    char databaseName[20];
    struct databases* nextDB;
    struct tables* userTable;
} dbs;

typedef struct tables {
    char tableName[20];
    struct tables* nextTable;
    struct field* tableField;
} table;

typedef struct field {
    int dataType;
    int size;
    char filedName[20];
    struct data* fieldData;
    struct field* nextfield;
} field;

typedef struct data {
    int number;
    char str[100];
    double realNum;
    struct data* nextData;
    int checkCount;
} data;

// 첫번째 유저와 마지막 유저 선언
users* startUser = NULL;
users* endUser = NULL;

// 유저 정보 전역 변수
char* userID;
char* userPW;

// 현재 유저와 사용중인 DB를 가리키는 커서
users* userCursor;
dbs* dbCursor;
table* tableCursor;

// 필드 정보
char tempName[30] = { 0, };
int tempType = 0;
int tempSize = 0;

// 기본 디렉터리 경로
char* directoryPath = "/home/kim/dbms/users";

// 새로운 유저 생성 함수
int createUser(char name[], char pass[]) {

    users* userTemp = startUser;
    while (userTemp != NULL)
    {
        if (strcmp(userTemp->userName, name) == 0)
        {
            //printf("[%s] is already!!\n", name);
            return -1;
        }
        userTemp = userTemp->nextUser;
    }

    users* newUser;
    newUser = (users*)malloc(sizeof(users));
    strcpy(newUser->userName, name); 

    newUser->nextUser = NULL;
    newUser->database = NULL;

    // 첫번째 유저가 NULL인 경우
    if (startUser == NULL) {
        startUser = newUser;
        endUser = newUser;
    }
    // 마지막 유저 구조체를 이용하여 새로운 유저 
    // 구조체로 endUser 변경
    else {
        endUser->nextUser = newUser;
        endUser = newUser;
    }

    return 1;
}

void createUserFile(char name[], char pass[]) {

    char userPath[100] = {0, };
    strcpy(userPath, directoryPath);
    mkdir(userPath, 0755);

    sprintf(userPath, "%s/%s", userPath, name);
    mkdir(userPath, 0755);  

    char msg[100] = {0, };
    char userFile[100] = "users.txt";
    char temp[100] = {0,};
    sprintf(msg, "%s %s\n", name, pass);
    sprintf(temp, "%s/%s", directoryPath, userFile);

    FILE *file;
    file = fopen(temp, "a+");

    char str[100] = {0, };
    int flag = 0;
    while (fgets(str, sizeof(str), file) != NULL)
    {
        char* tempStr = strtok(str, " ");
        userID = tempStr;
        tempStr = strtok(NULL, " \n");
        userPW = tempStr;

        if (strcmp(name, userID) == 0)
        {
            flag = 1;
            break;
        } 
    }
    if (!flag) fputs(msg, file);
    fclose(file);
}

// 입력받은 문자열을 기반으로 한명의 유저를 찾아 반환
users* searchUser(char targetUser[]) {

    users* temp = NULL;
    users* startCopy = startUser;
    bool flag = false;

    // user 탐색일 경우
    while (startCopy != NULL) {
        if (strcmp(targetUser, startCopy->userName) == 0)
        {
            temp = startCopy;
            flag = true;
            break;
        }
        startCopy = startCopy->nextUser;
    }

    if (!flag)
    {
        printf("error : The user [%s] does not exist!\n", targetUser);
        return;
    }
    return temp;
}

// 유저 로그인 함수
void userLogin(char* id, char* pw) {

    FILE* fp = NULL;

    //printf("id : %s\n", id);
    //printf("pw : %s\n", pw);

    char fileName[20] = "users.txt";
    char temp[100] = {0, }; 
    sprintf(temp, "%s/%s", directoryPath, fileName);
    fp = fopen(temp, "r");
    if(fp == NULL) printf("error : can't open file!!\n");

    char str[100] = {0, };
    int flag = 0;
    while (fgets(str, sizeof(str), fp) != NULL)
    {
        char* tempStr = strtok(str, " ");
        userID = tempStr;

        tempStr = strtok(NULL, " \n");
        userPW = tempStr;

        if (!strcmp(id, userID) && !strcmp(pw, userPW))
        {
            userCursor = searchUser(userID);
            flag = 1;
            break;
        } 
    }

    if (flag)
    {
        printf("WellCome Login success!!\n");
        fclose(fp);
    } 
    else 
    {
        printf("Login Failed!! check your id or password\n");
        fclose(fp);
    }
    
}

// 특정 DB를 찾아 DB 구조체 반환
dbs* searchDB(char dbName[], users* target) {

    // 타겟 유저의 DB 정보를 가져와 temp에 저장
    dbs* temp = NULL;
    temp = target->database;
    bool flag = false;

    // temp의 DB 이름과 유저가 보내준 DB 이름이 일치할 경우
    // 해당 DB 구조체를 반환
    // 일치하지 않으면 마지막 DB NULL 값을 반환
    while (temp != NULL)
    {
        if (strcmp(dbName, temp->databaseName) == 0) {
            flag = true;
            break;
        }
        temp = temp->nextDB;
    }
    if (!flag)
    {
        printf("error : The DB [%s] does not exist!\n", dbName);
        return;
    }
    //printf("success!\n");
    return temp;
}

// 새로운 DB 생성 함수
int createDB(char name[], users* target) {

    // 유저가 존재하지 않을 경우
    if (target == NULL)
    {
        printf("error : the user does not exist!\n");
        return -1;
    }
    dbs* dbTemp = target->database;
    while (dbTemp != NULL)
    {
        if (strcmp(dbTemp->databaseName, name)==0) {
            printf("error : [%s] already in use! please input another name!\n", name);
            return -1;
        }
        dbTemp = dbTemp->nextDB;
    }
    
    // 새로운 DB 생성
    int flag = 0;
    dbs* newDB;
    newDB = (dbs*)malloc(sizeof(dbs));
    strcpy(newDB->databaseName, name);
    newDB->userTable = NULL;
    newDB->nextDB = NULL;

    if (target->database == NULL)
    {
        target->database = newDB;
    }
    else
    {
        dbs* findDB = target->database;
        while (findDB->nextDB != NULL)
        {
            findDB = findDB->nextDB;
        }
        findDB->nextDB = newDB;
    }

    return 1;

}

void createDBFile(char name[], users* target) {

    char userPath[100] = {0, };
    char DBpath[100] = {0, };

    sprintf(userPath, "%s/%s", directoryPath, userCursor->userName);
    sprintf(DBpath, "%s/%s", userPath, name);
    mkdir(DBpath, 0755);

    char msg[100] = {0, };
    char temp[100] = {0,};
	char dbFile[100] = "databases.txt";
	sprintf(msg, "%s\n", name);
	sprintf(temp, "%s/%s", userPath, dbFile);

    FILE *file;
	file = fopen(temp, "a+");
	fputs(msg, file);
	fclose(file);

    printf("success!\n");

}

// 특정 유저의 모든 DB를 출력하는 함수
void displayDB(users* target) {

    if (target->database == NULL)
    {
        printf("error : The DB does not exist!\n");
        return;
    }
    // 타겟 유저의 DB를 가져와 temp에 저장
    dbs* temp = NULL;
    temp = target->database;

    printf("---------------Databases----------------\n");
    while (temp != NULL)
    {
        printf("%s\n", temp->databaseName);
        temp = temp->nextDB;
    }
    printf("----------------------------------------\n");
}

// 새로운 테이블 필드를 생성하는 함수
void createField(table* target) {

    // 새로운 필드 선언 및 초기화
    field* newField;
    newField = (field*)malloc(sizeof(field));
    newField->dataType = tempType;
    newField->size = tempSize;
    strcpy(newField->filedName, tempName);
    newField->nextfield = NULL;
    newField->fieldData = NULL;

    int flag = 0;

    if (target->tableField == NULL)
    {
        target->tableField = newField;
    }
    else {
        field* findLast = target->tableField;
        while (findLast->nextfield != NULL)
        {
            findLast = findLast->nextfield;
        }

        findLast->nextfield = newField;
    }
}

void createFieldFile(table* target) {

    char tablePath[100] = {0, };
    char fieldPath[100] = {0, };

    sprintf(tablePath, "%s/%s/%s/%s", directoryPath, userCursor->userName,
         dbCursor->databaseName, target->tableName);

    sprintf(fieldPath, "%s/%s", tablePath, tempName);
    mkdir(fieldPath, 0755);

    char msg[100] = {0, };
    char temp[100] = {0, };
    char fieldFile[100] = "field.txt";

    char type[20] = {0, };
    if (tempType == 0)
    {
        strcpy(type, "INT");
    }
    else
    {
        strcpy(type, "VARCHAR");
    }

    sprintf(msg, "%s %s %d\n", tempName,
            type, tempSize);
    sprintf(temp, "%s/%s", tablePath, fieldFile);

    FILE *file;
    file = fopen(temp, "a+");
    fputs(msg, file);
    fclose(file);
}

// 필드 이름을 받아 해당 필드를 탐색 후 반환하는 함수
field* findOneField(char* fieldName, table* target) {

    field* fieldTemp = target->tableField;
    data* dataTemp = fieldTemp->fieldData;

    while (fieldTemp != NULL)
    {
        if (!strcmp(fieldTemp->filedName, fieldName))
        {
            return fieldTemp;
        }
        fieldTemp = fieldTemp->nextfield;
    }
    return;
}

// value를 입력받아 해당 value 구조체에 플래그 전환
void checkFlag(char* value, char* fieldName[], int fieldType, field* target) {

    int number = 0;
    char* str = NULL;
    data* checkData = target->fieldData;

    if (fieldType == 0) number = atoi(value);
    else str = value;

    char* nameTemp[10] = { 0, };
    int nameCount = 0;

    while (*fieldName)
    {
        nameTemp[nameCount++] = *fieldName++;
    }

    int layer = 0;
    while (checkData != NULL)
    {
        if (fieldType == 0)
        {
            if (number == checkData->number) {

                field* firstField = tableCursor->tableField;
                data* firstData = firstField->fieldData;

                for (int i = 0; i < nameCount; i++)
                {
                    field* findField = findOneField(nameTemp[i], tableCursor);

                    if (!strcmp(findField->filedName, nameTemp[i]))
                    {
                        firstData = findField->fieldData;
                        for (int y = 0; y < layer; y++)
                        {
                            firstData = firstData->nextData;
                        }
                        firstData->checkCount = firstData->checkCount + 1;
                    }
                }
            }
        }
        else if (!strcmp(checkData->str, str))
        {
            field* firstField = tableCursor->tableField;
            data* firstData = firstField->fieldData;

            for (int i = 0; i < nameCount; i++)
            {
                field* findField = findOneField(nameTemp[i], tableCursor);

                if (!strcmp(findField->filedName, nameTemp[i]))
                {
                    firstData = findField->fieldData;
                    for (int y = 0; y < layer; y++)
                    {
                        firstData = firstData->nextData;
                    }
                    firstData->checkCount = firstData->checkCount + 1;
                }
            }
        }
        checkData = checkData->nextData;
        layer++;
    }
}

// "!=" 일 경우
void reverceCheck(char* value, char* fieldName[], int fieldType, field* target) {

    int number = 0;
    char* str = NULL;
    data* checkData = target->fieldData;

    if (fieldType == 0) number = atoi(value);
    else str = value;

    char* nameTemp[10] = { 0, };
    int nameCount = 0;

    while (*fieldName)
    {
        nameTemp[nameCount++] = *fieldName++;
    }

    int layer = 0;
    while (checkData != NULL)
    {
        if (fieldType == 0)
        {
            if (number != checkData->number) {

                field* firstField = tableCursor->tableField;
                data* firstData = firstField->fieldData;

                for (int i = 0; i < nameCount; i++)
                {
                    field* findField = findOneField(nameTemp[i], tableCursor);
                    if (!strcmp(findField->filedName, nameTemp[i]))
                    {
                        firstData = findField->fieldData;
                        for (int y = 0; y < layer; y++)
                        {
                            firstData = firstData->nextData;
                        }
                        firstData->checkCount = firstData->checkCount + 1;
                    }
                }
            }
        }
        else if (strcmp(checkData->str, str))
        {
            field* firstField = tableCursor->tableField;
            data* firstData = firstField->fieldData;

            for (int i = 0; i < nameCount; i++)
            {
                field* findField = findOneField(nameTemp[i], tableCursor);
                if (!strcmp(findField->filedName, nameTemp[i]))
                {
                    firstData = findField->fieldData;
                    for (int y = 0; y < layer; y++)
                    {
                        firstData = firstData->nextData;
                    }
                    firstData->checkCount = firstData->checkCount + 1;
                }
            }
        }
        checkData = checkData->nextData;
        layer++;
    }

}

// 출력 후 내부 초기화 함수
void removeValue(table* target) {

    field* fieldTemp = target->tableField;
    data* dataTemp = fieldTemp->fieldData;

    while (fieldTemp != NULL)
    {
        while (dataTemp != NULL)
        {
            dataTemp->checkCount = 0;
            dataTemp = dataTemp->nextData;
        }
        fieldTemp = fieldTemp->nextfield;
    }
}

// where 조건에 부합하는 값 출력
void whereDisplay(char* fieldName[], table* target) {

    field* fieldTemp = target->tableField;
    data* dataTemp = fieldTemp->fieldData;

    if (fieldTemp == NULL) printf("error!\n");

    int valueCount = 0;
    while (dataTemp != NULL)
    {
        valueCount++;
        dataTemp = dataTemp->nextData;
    }

    dataTemp = fieldTemp->fieldData;
    char* nameTemp[10] = { 0, };
    int count = 0;

    while (*fieldName)
    {
        nameTemp[count++] = *fieldName++;
    }

    bool space = false;
    for (int i = 0; i < valueCount; i++)
    {
        for (int z = 0; z < count; z++)
        {
            fieldTemp = findOneField(nameTemp[z], target);
            if (!strcmp(fieldTemp->filedName, nameTemp[z]))
            {
                dataTemp = fieldTemp->fieldData;
                for (int y = 0; y < i; y++)
                {
                    dataTemp = dataTemp->nextData;
                }
                if (fieldTemp->dataType == 0)
                {
                    if (dataTemp->checkCount >= 1)
                    {
                        printf("%d\t", dataTemp->number);
                        space = true;
                    }
                    dataTemp->checkCount = 0;
                }
                else if (fieldTemp->dataType == 1)
                {
                    if (dataTemp->checkCount >= 1)
                    {
                        printf("%s\t", dataTemp->str);
                        space = true;
                    }
                    dataTemp->checkCount = 0;
                }
            }
        }
        if (space)
        {
            printf("\n");
            space = false;
        }
    }
}

// where AND 조건에 부합시 값 출력
void whereAndDisplay(char* fieldName[], table* target) {
    field* fieldTemp = target->tableField;
    data* dataTemp = fieldTemp->fieldData;

    int valueCount = 0;
    while (dataTemp != NULL)
    {
        valueCount++;
        dataTemp = dataTemp->nextData;
    }

    dataTemp = fieldTemp->fieldData;
    char* nameTemp[10] = { 0, };
    int count = 0;

    while (*fieldName)
    {
        nameTemp[count++] = *fieldName++;
    }

    bool space = false;
    for (int i = 0; i < valueCount; i++)
    {
        for (int z = 0; z < count; z++)
        {
            fieldTemp = findOneField(nameTemp[z], target);
            if (!strcmp(fieldTemp->filedName, nameTemp[z]))
            {
                dataTemp = fieldTemp->fieldData;
                for (int y = 0; y < i; y++)
                {
                    dataTemp = dataTemp->nextData;
                }
                if (fieldTemp->dataType == 0)
                {
                    if (dataTemp->checkCount == 2)
                    {
                        printf("%d\t", dataTemp->number);
                        space = true;
                    }
                    dataTemp->checkCount = 0;
                }
                else if (fieldTemp->dataType == 1)
                {
                    if (dataTemp->checkCount == 2)
                    {
                        printf("%s\t", dataTemp->str);
                        space = true;
                    }
                    dataTemp->checkCount = 0;
                }
            }
        }
        if (space)
        {
            printf("\n");
            space = false;
        }
    }
}

// 테이블 필드를 찾아 value 값을 출력해주는 함수
void displayField(char* fieldName[], table* target) {
    // 타겟 유저의 table 정보를 가져와 temp에 저장
    field* fieldTemp = target->tableField;
    data* dataTemp = fieldTemp->fieldData;

    if (fieldTemp == NULL || dataTemp == NULL) return;

    int valueCount = 0;
    while (dataTemp != NULL)
    {
        valueCount++;
        dataTemp = dataTemp->nextData;
    }

    dataTemp = fieldTemp->fieldData;
    char* nameTemp[10] = { 0, };
    int count = 0;

    while (*fieldName)
    {
        nameTemp[count++] = *fieldName++;
    }

    for (int i = 0; i < valueCount; i++)
    {
        for (int z = 0; z < count; z++)
        {
            fieldTemp = findOneField(nameTemp[z], target);
            if (!strcmp(fieldTemp->filedName, nameTemp[z]))
            {
                dataTemp = fieldTemp->fieldData;
                for (int y = 0; y < i; y++)
                {
                    dataTemp = dataTemp->nextData;
                }

                if (fieldTemp->dataType == 0)
                {
                    printf("%d\t", dataTemp->number);
                }
                else if (fieldTemp->dataType == 1)
                {
                    printf("%s\t", dataTemp->str);
                }
            }
        }
        printf("\n");
    }
}

// 테이블 필드에 테이터를 삽입하는 함수
void insertValue(char value[], field* target) {

    field* fieldTemp = target;

    data* newData = NULL;
    newData = (data*)malloc(sizeof(data));
    newData->nextData = NULL;
    newData->checkCount = 0;

    int typeTemp = fieldTemp->dataType;
    int sizeTemp = fieldTemp->size;

    // value가 int인 경우
    if (typeTemp == 0)
    {
        int num = atoi(value);
        newData->number = num;
    }
    // value가 char[]인 경우
    else if (typeTemp == 1)
    {
        strcpy(newData->str, value);
    }

    if (fieldTemp->fieldData == NULL)
    {
        fieldTemp->fieldData = newData;
    }
    else
    {
        data *lastData = fieldTemp->fieldData;
        while (lastData->nextData != NULL)
        {
            lastData = lastData->nextData;
        }
        lastData->nextData = newData;
    }

}

void insertValueFile(char value[], char tableName[], field* target) {

    field* fieldTemp = target;
    char fieldPath[100] = {0, };

    sprintf(fieldPath, "%s/%s/%s/%s/%s", directoryPath,
            userCursor->userName, dbCursor->databaseName,
            tableName, fieldTemp->filedName);

    char msg[100] = {0, };
    char temp[100] = {0, };
    char valueFile[100] = "value.txt";
    sprintf(temp, "%s/%s", fieldPath, valueFile);
    sprintf(msg, "%s\n", value);

    FILE *file;
    file = fopen(temp, "a+");
    fputs(msg, file);
    fclose(file);

}

// 테이블 필드 정보를 츨력하는 함수
void displayDesc(table* target) {

    // 타켓 table를 받아 Field 정보를 temp에 저장
    field* temp = target->tableField;
    printf("---------------Table Field---------------\n");
    while (temp != NULL)
    {
        char typeTemp[10];
        if (temp->dataType == 0)
        {
            strcpy(typeTemp, "INT");
        }
        else if (temp->dataType == 1) {
            strcpy(typeTemp, "VARCHAR");
        }
        else
        {
            strcpy(typeTemp, "DOUBLE");
        }

        printf("NAME : %s\tTYPE : %s\tSIZE : %d\n", temp->filedName, typeTemp, temp->size);
        temp = temp->nextfield;
    }
    printf("-----------------------------------------\n");

}

// 새로운 테이블 생성 함수
table* createTable(char name[], dbs* target) {

    table* newTable;
    newTable = (table*)malloc(sizeof(table));
    strcpy(newTable->tableName, name);
    newTable->nextTable = NULL;
    newTable->tableField = NULL;

    if (target == NULL)
    {
        printf("error : The DB does not exist!\n");
        free(newTable);
        return;
    }

    table* tableTemp = target->userTable;
    while (tableTemp != NULL)
    {
        if (!strcmp(name, tableTemp->tableName))
        {
            printf("error : the [%s] is already input another name!!\n", name);
            free(newTable);
            return;
        }
        tableTemp = tableTemp->nextTable;
    }

    if (target->userTable == NULL)
    {
        target->userTable = newTable;
        return newTable;
    }
    else
    {
        table* lastTable = target->userTable;
        while (lastTable->nextTable != NULL)
        {
            lastTable = lastTable->nextTable;
        }
        lastTable->nextTable = newTable;
        return newTable;
    }

}

void createTableFile(char name[], dbs* target) {

    char DBpath[100] = {0, };
    char tablePath[100] = {0,};

    sprintf(DBpath, "%s/%s/%s", directoryPath, userCursor->userName, dbCursor->databaseName);

    sprintf(tablePath, "%s/%s", DBpath, name);
    mkdir(tablePath, 0755);

    char msg[100] = {0,};
    char temp[100] = {0,};
    char tableFile[100] = "tables.txt";
    sprintf(msg, "%s\n", name);
    sprintf(temp, "%s/%s", DBpath, tableFile);

    FILE *file;
    file = fopen(temp, "a+");
    fputs(msg, file);
    fclose(file);
}

// 테이블 정보를 모두 출력하는 함수
void displayTable(dbs* target) {

    // 타겟 DB를 받아 Table 정보를 temp에 저장
    table* temp = target->userTable;

    printf("-----------------Tables-----------------\n");
    while (temp != NULL)
    {
        printf("table name : %s\n", temp->tableName);
        temp = temp->nextTable;
    }
    printf("----------------------------------------\n");

}

// 특정 table 하나를 찾아 table 구조체 반환
table* searchTable(char tableName[], dbs* target) {

    if (target == NULL)
    {
        printf("error : The DB not found!\n");
        return;
    }

    table* temp = NULL;
    temp = target->userTable;
    int flag = 0;

    while (temp != NULL)
    {
        if (strcmp(tableName, temp->tableName) == 0) {
            flag = 1;
            break;
        }
        temp = temp->nextTable;
    }
    if (!flag)
    {
        printf("error : The table [%s] does not exist!\n", tableName);
        return;
    }
    return temp;
}

// 특정 테이블 삭제 함수
void dropTable(char name[], dbs* targetDB) {

    table* targetTable = searchTable(name, targetDB);
    table* next = targetTable->nextTable;

    table* firstTable = targetDB->userTable;
    table* preTable = NULL;

    char* dirPath[512] = {0, };
    sprintf(dirPath, "%s/%s/%s/%s", directoryPath, userCursor->userName
        , targetDB->databaseName, targetTable->tableName);

    //dropTableFile(dirPath, targetTable);

    while (firstTable != NULL)
    {
        if (!strcmp(firstTable->tableName, name))
        {
            preTable = firstTable;
        }

        else if (!strcmp(firstTable->nextTable->tableName, name))
        {
            preTable = firstTable->nextTable;
            break;
        }
        firstTable = firstTable->nextTable;
        if (firstTable == NULL || firstTable->nextTable == NULL) break;
    }

    field* lastField = targetTable->tableField;
    data* lastData = lastField->fieldData;

    while (lastField != NULL)
    {
        while (lastData != NULL)
        {
            data* removeData = lastData;
            lastData = lastData->nextData;

            free(removeData);
        }
        field* removeField = lastField;
        lastField = lastField->nextfield;
        free(removeField);
        
		if (lastField == NULL) break;

        lastData = lastField->fieldData;
    }

    if (!strcmp(targetDB->userTable->tableName, targetTable->tableName))
    {
        targetDB->userTable = next;
        free(targetTable);
    }
    else if (targetTable->nextTable == NULL) {

        preTable->nextTable = NULL;
        free(targetTable);
    }
    else
    {
        preTable->nextTable = next;
        free(targetTable);
    }

}

void dropTableFile(char tablePath[], table* target) {

    DIR *dirPT = NULL;
    struct dirent *file = NULL;
    field* fieldTemp = target->tableField;

    dirPT = opendir(tablePath);

    while ((file = readdir(dirPT)) != NULL)
    {
        if (strcmp(file->d_name, ".") == 0 || strcmp(file->d_name, "..") == 0)
        {
            continue;
        }

        while (fieldTemp != NULL)
        {
            char filedPath[512] = {0, };
            sprintf(filedPath, "%s/%s/%s/%s/%s", directoryPath, userCursor->userName
                , dbCursor->databaseName, target->tableName, fieldTemp->filedName);

            dropFiledFile(filedPath, target);
            fieldTemp = fieldTemp->nextfield;

            if (fieldTemp == NULL)
            {
                char txtPath[512] = { 0, };
                sprintf(txtPath, "%s/%s/%s/%s/%s", directoryPath, userCursor->userName
                    , dbCursor->databaseName, target->tableName, "field.txt");
                unlink(txtPath);
                break;
            }
        }     
    }
    char path[512] = {0, };
    sprintf(path, "%s/%s/%s/%s", directoryPath, userCursor->userName
        , dbCursor->databaseName, target->tableName);

    rmdir(path);
    closedir(dirPT);
}

void dropFiledFile(char fieldPath[], table* target) {

    DIR *dirPT = NULL;
    struct dirent *file = NULL;

    dirPT = opendir(fieldPath);

    char valuePath[512] = {
        0,
    };
    sprintf(valuePath, "%s/%s", fieldPath, "value.txt");
    unlink(valuePath);
    rmdir(fieldPath);
    closedir(dirPT);

}

void dropDatabaseFile(char dbPath[], dbs* target) {

    DIR *dirPT = NULL;
    struct dirent *file = NULL;

    table* tableTemp = target->userTable;
    dirPT = opendir(dbPath);

    while ((file = readdir(dirPT)) != NULL)
    {
        if (strcmp(file->d_name, ".") == 0 || strcmp(file->d_name, "..") == 0)
        {
            continue;
        }

        while (tableTemp != NULL)
        {
            char tablePath[512] = {0, };
            sprintf(tablePath, "%s/%s/%s/%s", directoryPath, userCursor->userName
                , target->databaseName, tableTemp->tableName);

            dropTableFile(tablePath, tableTemp);
            tableTemp = tableTemp->nextTable;
            
            if(tableTemp == NULL) {
                char txtPath[512] = {0, };
                sprintf(txtPath, "%s/%s/%s/%s", directoryPath, userCursor->userName
                    , target->databaseName, "tables.txt");
                unlink(txtPath);
                break;
            }
        }        
    }
    char path[512] = {0, };
    sprintf(path, "%s/%s/%s", directoryPath, userCursor->userName
        , target->databaseName);

    rmdir(path);
    closedir(dirPT);

}

// 특정 데이터베이스 삭제
void dropDatabase(char dbName[], users* target) {

    dbs* targetDB = searchDB(dbName, target);
    table* firstTable = targetDB->userTable;

    char* dbPath[512] = {0, };
    sprintf(dbPath, "%s/%s/%s", directoryPath, userCursor->userName
        , targetDB->databaseName);

    dropDatabaseFile(dbPath, targetDB);
    //printf("3333\n");

    while (firstTable != NULL)
    {
        char* tableName = firstTable->tableName;
        table* fieldTemp = firstTable->nextTable;
        dropTable(tableName, targetDB);

        firstTable = fieldTemp;
        if (firstTable == NULL)
        {
            break;
        }
    }

    dbs* firstDB = target->database;
    dbs* next = targetDB->nextDB;
    dbs* pre;
    while (firstDB != NULL)
    {
        if (!strcmp(firstDB->databaseName, dbName))
        {
            pre = firstDB;
            break;
        }
        else if (!strcmp(firstDB->nextDB->databaseName, targetDB->databaseName))
        {
            pre = firstDB->nextDB;
            break;
        }

        firstDB = firstDB->nextDB;
        if (firstDB == NULL || firstDB->nextDB == NULL) break;
    }

    if (!strcmp(targetDB->databaseName, target->database->databaseName))
    {
        target->database = next;
        free(targetDB);
    }
    else if (targetDB->nextDB == NULL)
    {
        pre->nextDB = NULL;
        free(targetDB);
    }
    else
    {
        pre->nextDB = next;
        free(targetDB);
    }
}

void deleteFiledFile(table* target) {

    field* fieldTemp = target->tableField;
    data* dataTemp = fieldTemp->fieldData;

    while (fieldTemp != NULL)
    {
        char valuePath[100] = {0, };
        sprintf(valuePath, "%s/%s/%s/%s/%s/%s", directoryPath, userCursor->userName
            , dbCursor->databaseName, target->tableName
            , fieldTemp->filedName, "value.txt");

        FILE *valueFP = fopen(valuePath, "w+");
        while (dataTemp != NULL)
        {
            char msg[512] = {0,};
            if (fieldTemp->dataType == 0)
            {
                sprintf(msg, "%d\n", dataTemp->number);
                fputs(msg, valueFP);
            }
            else if (fieldTemp->dataType == 1)
            {
                sprintf(msg, "%s\n", dataTemp->str);
                fputs(msg, valueFP);
            }

            dataTemp = dataTemp->nextData;
        }
        
        fclose(valueFP);
        fieldTemp = fieldTemp->nextfield;
        if (fieldTemp == NULL) break;    
        dataTemp = fieldTemp->fieldData;
    }  

}

// 특정 조건에 맞는 열을 삭제하는 함수
void deleteField(table* target) {

    field* fieldTemp = target->tableField;
    data* dataTemp = fieldTemp->fieldData;

    int layer = 0;
    int num = 0;
    char* str;

    while (fieldTemp != NULL)
    {
        while (dataTemp != NULL)
        {
            int flag = 0;
            data* preData;
            if (dataTemp->checkCount >= 1)
            {
                field* firstField = target->tableField;
                while (firstField != NULL)
                {
                    data* pre = firstField->fieldData;
                    for (int i = 0; i < layer - 1; i++)
                    {
                        pre = pre->nextData;
                    }
                    data* targetData = pre->nextData;

                    if (layer == 0)
                    {
                        firstField->fieldData = pre->nextData;
                        free(pre);
                    }
                    else
                    {
                        data* next = targetData->nextData;
                        pre->nextData = next;
                        free(targetData);
                    }

                    firstField = firstField->nextfield;
                }
            }
            else
            {
                dataTemp = dataTemp->nextData;
                layer++;
            }
            dataTemp = fieldTemp->fieldData;
            for (int i = 0; i < layer; i++)
            {
                dataTemp = dataTemp->nextData;
            }

        }
        fieldTemp = fieldTemp->nextfield;
        layer = 0;
        if (fieldTemp == NULL) break;
        dataTemp = fieldTemp->fieldData;
    }    

}

// 해당 테이블 모든 테이터 삭제
void removeALL(table* target) {
	
	field* fieldTemp = target->tableField;

    while (fieldTemp != NULL)
    {
        char valuePath[100] = {0, };
        sprintf(valuePath, "%s/%s/%s/%s/%s/%s", directoryPath, userCursor->userName
            , dbCursor->databaseName, target->tableName
            , fieldTemp->filedName, "value.txt");

        FILE *valueFP = fopen(valuePath, "w");
        fclose(valueFP);

        data* dataTemp = fieldTemp->fieldData;
        while (dataTemp != NULL)
        {
            data* removeData = dataTemp;
            dataTemp = dataTemp->nextData;
            free(removeData);
        }
        fieldTemp->fieldData = NULL;

        fieldTemp = fieldTemp->nextfield;
        if (fieldTemp == NULL)
        {
            break;
        }
    }
    
}

// and 조건 삭제
void dropAND(table* target) {

    field* fieldTemp = target->tableField;
    data* dataTemp = fieldTemp->fieldData;

    int layer = 0;
    int num = 0;
    char* str;

    while (fieldTemp != NULL)
    {
        while (dataTemp != NULL)
        {
            int flag = 0;
            if (dataTemp->checkCount == 2)
            {
                field* firstField = target->tableField;
                while (firstField != NULL)
                {
                    data* pre = firstField->fieldData;
                    for (int i = 0; i < layer - 1; i++)
                    {
                        pre = pre->nextData;
                    }

                    data* targetData = pre->nextData;

                    if (layer == 0)
                    {
                        firstField->fieldData = pre->nextData;
                        free(pre);
                    }
                    else if (targetData->nextData == NULL)
                    {
                        pre->nextData = NULL;
                        free(targetData);
                    }
                    else
                    {
                        data* next = targetData->nextData;
                        pre->nextData = next;
                        free(targetData);
                    }
                    firstField = firstField->nextfield;
                }
                return;
            }
            dataTemp = dataTemp->nextData;
            if (dataTemp == NULL) break;
            layer++;
        }
        fieldTemp = fieldTemp->nextfield;
        layer = 0;
        if (fieldTemp == NULL) break;
        dataTemp = fieldTemp->fieldData;
    }

    fieldTemp = target->tableField;
    dataTemp = fieldTemp->fieldData;   

}

// 필드의 값을 변경하는 함수
void updateValue(char* value, char* targetname, table* target) {

    field* fieldTemp = target->tableField;
    field* targetField = findOneField(targetname, target);
    data* dataTemp = fieldTemp->fieldData;

    int num = 0;
    char* str;
    int layer = 0;

    while (fieldTemp != NULL)
    {
        char valuePath[100] = {0, };
        sprintf(valuePath, "%s/%s/%s/%s/%s/%s", directoryPath, userCursor->userName
            , dbCursor->databaseName, target->tableName, fieldTemp->filedName, "value.txt");
    
        FILE *valueFP = fopen(valuePath, "w+");

        while (dataTemp != NULL)
        {
            if (dataTemp->checkCount >= 1)
            {
                data* targetData = targetField->fieldData;
                for (int i = 0; i < layer; i++)
                {
                    targetData = targetData->nextData;
                }

                if (targetField->dataType == 0)
                {
                    num = atoi(value);
                    targetData->number = num;
                }
                else if (targetField->dataType == 1)
                {
                    strcpy(targetData->str, value);
                }
                dataTemp->checkCount = 0;
            }
            char msg[512] = {0,};
            if (fieldTemp->dataType == 0)
            {
                sprintf(msg, "%d\n", dataTemp->number);
                fputs(msg, valueFP);
            } 
            else
            {
                sprintf(msg, "%s\n", dataTemp->str);
                fputs(msg, valueFP);
            }

            dataTemp = dataTemp->nextData;
            layer++;
        }
        layer = 0;
        fieldTemp = fieldTemp->nextfield;
        if (fieldTemp == NULL) break;
        dataTemp = fieldTemp->fieldData;
        fclose(valueFP);
    }

}

// AND 조건일 경우 값을 변경하는 함수
void updateAND(char* value, char* targetname, table* target) {

    field* fieldTemp = target->tableField;
    field* targetField = findOneField(targetname, target);
    data* dataTemp = fieldTemp->fieldData;

    int num = 0;
    char* str;
    int layer = 0;

    while (fieldTemp != NULL)
    {
        char valuePath[100] = {0, };
        sprintf(valuePath, "%s/%s/%s/%s/%s/%s", directoryPath, userCursor->userName
            , dbCursor->databaseName, target->tableName, fieldTemp->filedName, "value.txt");
    
        FILE *valueFP = fopen(valuePath, "w+");

        while (dataTemp != NULL)
        {
            if (dataTemp->checkCount == 2)
            {
                data* targetData = targetField->fieldData;
                for (int i = 0; i < layer; i++)
                {
                    targetData = targetData->nextData;
                }

                if (targetField->dataType == 0)
                {
                    num = atoi(value);
                    targetData->number = num;
                }
                else if (targetField->dataType == 1)
                {
                    strcpy(targetData->str, value);
                }
                dataTemp->checkCount = 0;
            }

            char msg[512] = {0,};
            if (fieldTemp->dataType == 0)
            {
                sprintf(msg, "%d\n", dataTemp->number);
                fputs(msg, valueFP);
            } 
            else
            {
                sprintf(msg, "%s\n", dataTemp->str);
                fputs(msg, valueFP);
            }

            dataTemp = dataTemp->nextData;
            layer++;
        }
        layer = 0;
        fieldTemp = fieldTemp->nextfield;
        if (fieldTemp == NULL) break;
        dataTemp = fieldTemp->fieldData;
        fclose(valueFP);
    }
}

// 사용자의 입력값을 파싱하는 함수
void inputParser(char msg[], users* target) {

    // 맨 앞을(DELETE, CREATE, USE 등)를 잘라 first에 저장
    char* tempStr;
    char temp[100];
    strcpy(temp, msg);
    tempStr = strtok(temp, " ");

    while (tempStr != NULL)
    {
        if (!strcmp(tempStr, "CREATE"))
        {
            tempStr = strtok(NULL, " ");
            if (!strcmp(tempStr, "DATABASE"))
            {
                tempStr = strtok(NULL, " ;");
                createDB(tempStr, target);
                createDBFile(tempStr, target);
                break;
            }             
            else if (!strcmp(tempStr, "TABLE"))
            {
                char* tableName = NULL;
                tempStr = strtok(NULL, " (");
                tableName = tempStr;

                table* newTable = createTable(tableName, dbCursor);
                createTableFile(tableName, dbCursor);
                if (newTable == NULL) return;

                int count = 0;
                int flag = 0;
                while (tempStr != NULL)
                {
                    tempStr = strtok(NULL, ";\n,() ");
                    if (count == 0 && tempStr != NULL)
                    {
                        strcpy(tempName, tempStr);
                        count++;
                    }
                    else if (count == 1) {
                        if (!strcmp(tempStr, "INT\0"))
                        {
                            tempType = 0;
                        }
                        else {
                            tempType = 1;
                        }
                        count++;
                    }
                    else if (count == 2) {
                        tempSize = atoi(tempStr);
                        count++;
                    }
                    if (count > 2)
                    {
                        createField(newTable);
                        createFieldFile(newTable);
                        count = 0;
                    }
                }               
            }
            else if (!strcmp(tempStr, "USER")) {

                tempStr = strtok(NULL, " '");
                userID = tempStr;

                tempStr = strtok(NULL, " ");
                if (!strcmp(tempStr, "PASSWORD"))
                {
                    tempStr = strtok(NULL, " '\n;");
                    userPW = tempStr;
                }
                if (createUser(userID, userPW) == 1)
                {
                    createUserFile(userID, userPW);
                    printf("success!!\n");
                } else 
                {
                    printf("[%s] is already!!\n", userID);
                }
            }
        }
        else if (!strcmp(tempStr, "USE"))
        {
            tempStr = strtok(NULL, " (;");
            char* dbName = tempStr;

            dbCursor = searchDB(dbName, target);
        }
        else if (!strcmp(tempStr, "SHOW"))
        {
            tempStr = strtok(NULL, " ;");
            if (!strcmp(tempStr, "DATABASES"))
            {
                displayDB(target);
            }
            else if (!strcmp(tempStr, "TABLES"))
            {
                displayTable(dbCursor);
            }
        }
        else if (!strcmp(tempStr, "INSERT"))
        {
            tempStr = strtok(NULL, " ");

            if (!strcmp(tempStr, "INTO"))
            {
                tempStr = strtok(NULL, " ");
                char* tableName = tempStr;
                table* target = searchTable(tableName, dbCursor);
                field* firstField = target->tableField;

                while (tempStr != NULL)
                {
                    tempStr = strtok(NULL, " (,)';\n");
                    if (!strcmp(tempStr, "VALUES"))
                    {
                        int count = 0;
                        char value[512] = { 0, };
                        while (tempStr != NULL) {
                            tempStr = strtok(NULL, " (,)';\n");
                            if (tempStr == NULL) break;
                            
                            strcpy(value, tempStr);
                            insertValue(value, firstField);
                            insertValueFile(value, target->tableName, firstField);
                            firstField = firstField->nextfield;

                        }
                    }
                }
            }
        }
        else if (!strcmp(tempStr, "SELECT"))
        {
            char* fieldName[10] = { 0, };
            int count = 0;
            char* tableName;

            tempStr = strtok(NULL, " ,");
            if (!strcmp(tempStr, "*"))
            {
                while (strcmp(tempStr, "FROM"))
                {
                    tempStr = strtok(NULL, ", ");
                }

                tempStr = strtok(NULL, ", ;\n");
                tableName = tempStr;
                tableCursor = searchTable(tableName, dbCursor);

                field* fieldTemp = tableCursor->tableField;
                count = 0;
                int i = 0;
                while (*fieldName)
                {
                    memset(fieldName[i], 0, sizeof(fieldName[i]));
                    i++;
                }
                while (fieldTemp != NULL)
                {
                    fieldName[count++] = fieldTemp->filedName;
                    fieldTemp = fieldTemp->nextfield;
                }
                tempStr = strtok(NULL, " \n");
            }
            else
            {
                while (strcmp(tempStr, "FROM"))
                {
                    fieldName[count++] = tempStr;
                    tempStr = strtok(NULL, ", ");
                }

                tempStr = strtok(NULL, ", ;\n");
                tableName = tempStr;
                tempStr = strtok(NULL, ", ;\n");

            }
            if (tempStr == NULL)
            {
                displayField(fieldName, tableCursor);
            }
            else if (!strcmp(tempStr, "WHERE"))
            {
                int andFlag = 0;
                while (tempStr != NULL)
                {
                    tempStr = strtok(NULL, " ';\n");
                    if (tempStr == NULL) break;

                    field* findField = findOneField(tempStr, tableCursor);
                    int fieldType = findField->dataType;
                    if (!strcmp(findField->filedName, tempStr))
                    {
                        tempStr = strtok(NULL, " ");
                        if (!strcmp(tempStr, "="))
                        {
                            tempStr = strtok(NULL, " ';\n");
                            checkFlag(tempStr, fieldName, fieldType, findField);

                            tempStr = strtok(NULL, " ");
                            if (!strcmp(tempStr, "OR"))
                            {
                                tempStr = strtok(NULL, " ");
                                findField = findOneField(tempStr, tableCursor);
                                fieldType = findField->dataType;

                                tempStr = strtok(NULL, " ");
                                if (!strcmp(tempStr, "="))
                                {
                                    tempStr = strtok(NULL, " ';\n");
                                    checkFlag(tempStr, fieldName, fieldType, findField);
                                    andFlag = 0;
                                    break;
                                }
                                else if (!strcmp(tempStr, "!=")) {
                                    tempStr = strtok(NULL, " ';\n");
                                    reverceCheck(tempStr, fieldName, fieldType, findField);
                                    andFlag = 0;
                                    break;
                                }
                            }
                            else if (!strcmp(tempStr, "AND"))
                            {
                                tempStr = strtok(NULL, " ");
                                findField = findOneField(tempStr, tableCursor);
                                fieldType = findField->dataType;

                                tempStr = strtok(NULL, " ");
                                if (!strcmp(tempStr, "="))
                                {
                                    tempStr = strtok(NULL, " ';\n");
                                    checkFlag(tempStr, fieldName, fieldType, findField);
                                    andFlag = 1;
                                    break;
                                }
                                else if (!strcmp(tempStr, "!=")) {
                                    tempStr = strtok(NULL, " ';\n");
                                    reverceCheck(tempStr, fieldName, fieldType, findField);
                                    andFlag = 1;
                                    break;
                                }
                            }
                        }
                        else if (!strcmp(tempStr, "!="))
                        {
                            tempStr = strtok(NULL, " ';\n");
                            reverceCheck(tempStr, fieldName, fieldType, findField);
                            andFlag = 0;

                            tempStr = strtok(NULL, " ");
                            if (!strcmp(tempStr, "OR"))
                            {
                                tempStr = strtok(NULL, " ");
                                findField = findOneField(tempStr, tableCursor);
                                fieldType = findField->dataType;

                                tempStr = strtok(NULL, " ");
                                if (!strcmp(tempStr, "="))
                                {
                                    tempStr = strtok(NULL, " ';\n");
                                    checkFlag(tempStr, fieldName, fieldType, findField);
                                    andFlag = 0;
                                    break;
                                }
                                else if (!strcmp(tempStr, "!=")) {
                                    tempStr = strtok(NULL, " ';\n");
                                    reverceCheck(tempStr, fieldName, fieldType, findField);
                                    andFlag = 1;
                                    break;
                                }
                            }
                            else if (!strcmp(tempStr, "AND"))
                            {
                                tempStr = strtok(NULL, " ");
                                findField = findOneField(tempStr, tableCursor);
                                fieldType = findField->dataType;

                                tempStr = strtok(NULL, " ");
                                if (!strcmp(tempStr, "="))
                                {
                                    tempStr = strtok(NULL, " ';\n");
                                    checkFlag(tempStr, fieldName, fieldType, findField);
                                    andFlag = 1;
                                    break;
                                }
                                else if (!strcmp(tempStr, "!=")) {

                                    tempStr = strtok(NULL, " ';\n");
                                    reverceCheck(tempStr, fieldName, fieldType, findField);
                                    andFlag = 1;
                                    break;
                                }
                            }
                        }
                    }
                }
                if (andFlag)
                {
                    whereAndDisplay(fieldName, tableCursor);
                    andFlag = 0;
                }
                else
                {
                    whereDisplay(fieldName, tableCursor);
                }
            }
        }
        else if (!strcmp(tempStr, "UPDATE"))
        {
            char* fieldName[10] = { 0, };
            int count = 0;
            int andFlag = 0;

            tempStr = strtok(NULL, " ");
            char* tableName = tempStr;
            char* newValue;
            char* setField;

            tableCursor = searchTable(tableName, dbCursor);
            tempStr = strtok(NULL, " ");

            if (!strcmp(tempStr, "SET"))
            {
                tempStr = strtok(NULL, " ");
                setField = tempStr;
                field* setField = findOneField(fieldName, tableCursor);
                tempStr = strtok(NULL, " ");

                if (!strcmp(tempStr, "="))
                {
                    tempStr = strtok(NULL, " '");
                    newValue = tempStr;
                }

                field* targetField;
                int targetCount = 0;
                char* targetFieldName[10] = { 0, };
                char* oldValue;

                tempStr = strtok(NULL, " ");
                while (!strcmp(tempStr, "WHERE"))
                {
                    tempStr = strtok(NULL, " ");
                    targetFieldName[targetCount++] = tempStr;
                    targetField = findOneField(tempStr, tableCursor);
                    char* fieldType = targetField->dataType;

                    tempStr = strtok(NULL, " ");
                    if (!strcmp(tempStr, "="))
                    {
                        tempStr = strtok(NULL, " ';\n");
                        oldValue = tempStr;
                        checkFlag(tempStr, targetFieldName, fieldType, targetField);

                        tempStr = strtok(NULL, " ");
                        if (!strcmp(tempStr, "OR"))
                        {
                            tempStr = strtok(NULL, " ");
                            targetField = findOneField(tempStr, tableCursor);
                            fieldType = targetField->dataType;

                            targetFieldName[0] = targetField->filedName;
                            tempStr = strtok(NULL, " ");
                            if (!strcmp(tempStr, "="))
                            {
                                tempStr = strtok(NULL, " ';\n");
                                checkFlag(tempStr, targetFieldName, fieldType, targetField);
                                andFlag = 0;
                                break;
                            }
                            else if (!strcmp(tempStr, "!=")) {
                                tempStr = strtok(NULL, " ';\n");
                                reverceCheck(tempStr, targetFieldName, fieldType, targetField);
                                andFlag = 0;
                                break;
                            }
                        }
                        else if (!strcmp(tempStr, "AND"))
                        {
                            tempStr = strtok(NULL, " ");
                            targetField = findOneField(tempStr, tableCursor);
                            fieldType = targetField->dataType;

                            targetFieldName[0] = targetField->filedName;
                            tempStr = strtok(NULL, " ");
                            if (!strcmp(tempStr, "="))
                            {
                                tempStr = strtok(NULL, " ';\n");
                                checkFlag(tempStr, targetFieldName, fieldType, targetField);
                                andFlag = 1;
                                break;
                            }
                            else if (!strcmp(tempStr, "!=")) {
                                tempStr = strtok(NULL, " ';\n");
                                reverceCheck(tempStr, targetFieldName, fieldType, targetField);
                                andFlag = 1;
                                break;
                            }
                        }
                    }
                    else if (!strcmp(tempStr, "!="))
                    {
                        tempStr = strtok(NULL, " ';\n");
                        oldValue = tempStr;
                        reverceCheck(tempStr, targetFieldName, fieldType, targetField);

                        tempStr = strtok(NULL, " ");
                        if (!strcmp(tempStr, "OR"))
                        {
                            tempStr = strtok(NULL, " ");
                            targetField = findOneField(tempStr, tableCursor);
                            fieldType = targetField->dataType;

                            targetFieldName[0] = targetField->filedName;
                            tempStr = strtok(NULL, " ");
                            if (!strcmp(tempStr, "="))
                            {
                                tempStr = strtok(NULL, " ';\n");
                                checkFlag(tempStr, targetFieldName, fieldType, targetField);
                                andFlag = 0;
                                break;
                            }
                            else if (!strcmp(tempStr, "!=")) {
                                tempStr = strtok(NULL, " ';\n");
                                reverceCheck(tempStr, targetFieldName, fieldType, targetField);
                                andFlag = 1;
                                break;
                            }
                        }
                        else if (!strcmp(tempStr, "AND"))
                        {
                            tempStr = strtok(NULL, " ");
                            targetField = findOneField(tempStr, tableCursor);
                            fieldType = targetField->dataType;

                            targetFieldName[0] = targetField->filedName;
                            tempStr = strtok(NULL, " ");
                            if (!strcmp(tempStr, "="))
                            {
                                tempStr = strtok(NULL, " ';\n");
                                checkFlag(tempStr, targetFieldName, fieldType, targetField);
                                andFlag = 1;
                                break;

                            }
                            else if (!strcmp(tempStr, "!=")) {
                                tempStr = strtok(NULL, " ';\n");
                                reverceCheck(tempStr, targetFieldName, fieldType, targetField);
                                andFlag = 1;
                                break;
                            }
                        }
                    }
                }
            }
            if (andFlag)
            {
                updateAND(newValue, setField, tableCursor);
                andFlag = 0;
            }
            else
            {
                updateValue(newValue, setField, tableCursor);
            }

            field* fieldTemp = tableCursor->tableField;
            int i = 0;
            while (fieldTemp != NULL)
            {
                fieldName[i++] = fieldTemp->filedName;
                fieldTemp = fieldTemp->nextfield;
            }
            displayField(fieldName, tableCursor);
            removeValue(tableCursor);
        }
        else if (!strcmp(tempStr, "DELETE"))
        {
            int andFlag = 0;
            tempStr = strtok(NULL, " ");
            tempStr = strtok(NULL, " ;\n");

            char* tableName = tempStr;
            tableCursor = searchTable(tableName, dbCursor);

            tempStr = strtok(NULL, " ';\n");
            if (tempStr == NULL)
            {
                removeALL(tableCursor);
                break;
            }
            else if (!strcmp(tempStr, "WHERE"))
            {
                int targetCount = 0;
                char* targetFieldName[10] = { 0, };

                tempStr = strtok(NULL, " ");
                targetFieldName[targetCount++] = tempStr;

                field* targetField = findOneField(tempStr, tableCursor);
                int fieldType = targetField->dataType;

                tempStr = strtok(NULL, " ");
                if (!strcmp(tempStr, "="))
                {
                    tempStr = strtok(NULL, " ';\n");
                    checkFlag(tempStr, targetFieldName, fieldType, targetField);

                    tempStr = strtok(NULL, " ");
                    if (!strcmp(tempStr, "OR"))
                    {
                        tempStr = strtok(NULL, " ");
                        targetField = findOneField(tempStr, tableCursor);
                        fieldType = targetField->dataType;

                        targetFieldName[0] = targetField->filedName;
                        tempStr = strtok(NULL, " ");
                        if (!strcmp(tempStr, "="))
                        {
                            tempStr = strtok(NULL, " ';\n");
                            checkFlag(tempStr, targetFieldName, fieldType, targetField);
                            andFlag = 0;
                        }
                        else if (!strcmp(tempStr, "!=")) {
                            tempStr = strtok(NULL, " ';\n");
                            reverceCheck(tempStr, targetFieldName, fieldType, targetField);
                            andFlag = 0;
                        }
                    }
                    else if (!strcmp(tempStr, "AND"))
                    {
                        tempStr = strtok(NULL, " ");
                        targetField = findOneField(tempStr, tableCursor);
                        fieldType = targetField->dataType;

                        targetFieldName[0] = targetField->filedName;
                        tempStr = strtok(NULL, " ");
                        if (!strcmp(tempStr, "="))
                        {
                            tempStr = strtok(NULL, " ';\n");
                            checkFlag(tempStr, targetFieldName, fieldType, targetField);
                            andFlag = 1;
                        }
                        else if (!strcmp(tempStr, "!=")) {
                            tempStr = strtok(NULL, " ';\n");
                            reverceCheck(tempStr, targetFieldName, fieldType, targetField);
                            andFlag = 1;
                        }
                    }
                }
                else if (!strcmp(tempStr, "!="))
                {
                    tempStr = strtok(NULL, " ';\n");
                    reverceCheck(tempStr, targetFieldName, fieldType, targetField);

                    tempStr = strtok(NULL, " ");
                    if (!strcmp(tempStr, "OR"))
                    {
                        tempStr = strtok(NULL, " ");
                        targetField = findOneField(tempStr, tableCursor);
                        fieldType = targetField->dataType;

                        targetFieldName[0] = targetField->filedName;
                        tempStr = strtok(NULL, " ");
                        if (!strcmp(tempStr, "="))
                        {
                            tempStr = strtok(NULL, " ';\n");
                            checkFlag(tempStr, targetFieldName, fieldType, targetField);
                            andFlag = 0;
                        }
                        else if (!strcmp(tempStr, "!=")) {
                            tempStr = strtok(NULL, " ';\n");
                            reverceCheck(tempStr, targetFieldName, fieldType, targetField);
                            andFlag = 1;
                        }
                    }
                    else if (!strcmp(tempStr, "AND"))
                    {
                        tempStr = strtok(NULL, " ");
                        targetField = findOneField(tempStr, tableCursor);
                        fieldType = targetField->dataType;

                        targetFieldName[0] = targetField->filedName;
                        tempStr = strtok(NULL, " ");
                        if (!strcmp(tempStr, "="))
                        {
                            tempStr = strtok(NULL, " ';\n");
                            checkFlag(tempStr, targetFieldName, fieldType, targetField);
                            andFlag = 1;

                        }
                        else if (!strcmp(tempStr, "!=")) {
                            tempStr = strtok(NULL, " ';\n");
                            reverceCheck(tempStr, targetFieldName, fieldType, targetField);
                            andFlag = 1;
                        }
                    }
                }
            }
            if (andFlag)
            {
                dropAND(tableCursor);
                deleteFiledFile(tableCursor);
            }
            else {
                deleteField(tableCursor);
                deleteFiledFile(tableCursor);
            }

            field* fieldTemp = tableCursor->tableField;
            char* fieldName[10] = { 0, };
            int i = 0;
            while (fieldTemp != NULL)
            {
                fieldName[i++] = fieldTemp->filedName;
                fieldTemp = fieldTemp->nextfield;
            }
            displayField(fieldName, tableCursor);
            removeValue(tableCursor);
        }
        else if (!strcmp(tempStr, "DROP")) {
            tempStr = strtok(NULL, " ");
            if (!strcmp(tempStr, "DATABASE"))
            {
                tempStr = strtok(NULL, " ;\n");
                dropDatabase(tempStr, userCursor);
                dbCursor = userCursor->database;

                char dbPath[512] = {0, };
                sprintf(dbPath, "%s/%s/%s", directoryPath, userCursor->userName
                    , "databases.txt");

                FILE* dbFP;
                dbFP = fopen(dbPath, "w+");
                dbs* dbTemp = userCursor->database;
                while (dbTemp != NULL)
                {
                    char str[512] = {0, };
                    sprintf(str, "%s\n", dbTemp->databaseName);
                    printf("db name : %s\n", dbTemp->databaseName);
                    fputs(str, dbFP);
                    dbTemp = dbTemp->nextDB;

                    if (dbTemp == NULL) break;
                    
                }             
                fclose(dbFP);

            }
            else if (!strcmp(tempStr, "TABLE"))
            {
                tempStr = strtok(NULL, " ;\n");

                table* targetTable = searchTable(tempStr, dbCursor);

                char *dirPath[512] = {0, };
                sprintf(dirPath, "%s/%s/%s/%s", directoryPath, userCursor->userName, dbCursor->databaseName, targetTable->tableName);

                dropTableFile(dirPath, targetTable);
                
                dropTable(tempStr, dbCursor);
                dbCursor = userCursor->database;

                char tablePath[512] = {0, };
                sprintf(tablePath, "%s/%s/%s/%s", directoryPath, userCursor->userName
                    , dbCursor->databaseName, "tables.txt");

                FILE* tableFP;
                tableFP = fopen(tablePath, "w+");
                table* tableTemp = dbCursor->userTable;
                while (tableTemp != NULL)
                {
                    char str[512] = {0, };
                    sprintf(str, "%s\n", tableTemp->tableName);
                    printf("table name : %s\n", tableTemp->tableName);
                    fputs(str, tableFP);
                    tableTemp = tableTemp->nextTable;
                    
                    if (tableTemp == NULL) break;
                }             
                fclose(tableFP);

            }
        }
        else if (!strcmp(tempStr, "DESC"))
        {
            tempStr = strtok(NULL, " ;\n");
            tableCursor = searchTable(tempStr, dbCursor);
            displayDesc(tableCursor);
        }
        else if (strcmp(tempStr, "logout\n") == 0)
        {
            char id[100] = {0, };
            char pass[100] = {0, };
            printf("input your id\n>> ");
            scanf("%s", id);
            printf("input your password\n>> ");
            scanf("%s", pass);
            userLogin(id, pass);

        }
        

        tempStr = strtok(NULL, " ");
    }
    printf("------------------------------------------------------------------\n");

}

void loadData() {

    char userPath[100] = {0, };
    sprintf(userPath, "%s/%s", directoryPath, "users.txt");

    FILE* userFP;
    userFP = fopen(userPath, "a+");

    if(userFP == NULL) printf("error : can't open user file!!\n");

    char str[100] = {0, };
    while (fgets(str, sizeof(str), userFP) != NULL)
    {
        char* tempStr = strtok(str, " ");
        userID = tempStr;

        tempStr = strtok(NULL, " \n");
        userPW = tempStr;
        //if (strcmp(userID, "root") == 0) continue;
        int result = createUser(userID, userPW);

        if (result)
        {
            char dbPath[100] = {0, };
            users* targetUser = searchUser(userID);
            sprintf(dbPath, "%s/%s/%s", directoryPath, targetUser->userName, "databases.txt");

            FILE* dbFP;
            dbFP = fopen(dbPath, "a+");
            if(dbFP == NULL) printf("error : can't open DB file!!\n");
            
            char str[100] = {0, };
            while (fgets(str, sizeof(str), dbFP) != NULL)
            {
                char* tempStr = strtok(str, "\n");
                int result = createDB(str, targetUser);
                
                if (result)
                {
                    char tablePath[100] = {0, };
                    dbs* targetDB = searchDB(tempStr, targetUser);
                    sprintf(tablePath, "%s/%s/%s/%s", directoryPath, targetUser->userName
                        , targetDB->databaseName, "tables.txt");

                    FILE* tableFP;
                    tableFP = fopen(tablePath, "a+");
                    if(tableFP == NULL) printf("error : can't open table file!!\n");

                    char str[100] = {0, };
                    while (fgets(str, sizeof(str), tableFP) != NULL)
                    {
                        char* tempStr = strtok(str, "\n");
                        table* targetTable = createTable(tempStr, targetDB);

                        if (targetTable != NULL)
                        {
                            char fieldPath[100] = {0, };
                            sprintf(fieldPath, "%s/%s/%s/%s/%s", directoryPath, targetUser->userName
                                , targetDB->databaseName, targetTable->tableName, "field.txt");

                            FILE* fieldFP;
                            fieldFP = fopen(fieldPath, "a+");
                            if(tableFP == NULL) printf("error : can't open field file!!\n");

                            char str[100] = {0, };
                            while (fgets(str, sizeof(str), fieldFP) != NULL)
                            {
                                char* temp = strtok(str, "\n");                                
                                char* tempStr = strtok(temp, " ");
                                int count = 0;

                                while (tempStr != NULL)
                                {
                                    if (count == 0 && tempStr != NULL)
                                    {
                                        strcpy(tempName, tempStr);
                                        count++;
                                    }
                                    else if (count == 1) {
                                        if (!strcmp(tempStr, "INT\0"))
                                        {
                                            tempType = 0;
                                        }
                                        else {
                                            tempType = 1;
                                        }
                                        count++;
                                    }
                                    else if (count == 2) {
                                        tempSize = atoi(tempStr);
                                        count++;
                                    }
                                    if (count > 2)
                                    {
                                        createField(targetTable);
                                        count = 0;
                                    }
                                    tempStr = strtok(NULL, " ");                                  
                                }

                            }
                            fclose(fieldFP);

                            field* fieldTemp = targetTable->tableField;

                            while (fieldTemp != NULL)
                            {
                                char valuePath[100] = {0, };
                                FILE *valueFP;
                                sprintf(valuePath, "%s/%s/%s/%s/%s/%s", directoryPath,
                                    targetUser->userName, targetDB->databaseName,
                                    targetTable->tableName, fieldTemp->filedName, "value.txt");

                                valueFP = fopen(valuePath, "a+");

                                if (valueFP == NULL)
                                    printf("error : can't open field file!!\n");

                                char str[100] = {0, };
                                char value[100] = {0, };
                                while (fgets(str, sizeof(str), valueFP) != NULL)
                                {
                                    strcpy(value, strtok(str, "\n"));
                                    insertValue(value, fieldTemp);
                                }
                                
                                fieldTemp = fieldTemp->nextfield;
                                fclose(valueFP);
                            }
                        } 
                    }

                    fclose(tableFP);
                }  
            }
            fclose(dbFP);
        }
    }
    fclose(userFP);
    
}

int main() {

    userID = "root";
    userPW = "toor";
    createUser(userID, userPW);
    createUserFile(userID, userPW);
    userLogin(userID, userPW);
    loadData();
    //userCursor = searchUser(userID);

    char sql[100] = { 0, };
    while (1)
    {
        printf(">> ");
        fgets(sql, 100, stdin);
        if (!strcmp(sql, "exit\n")) {
            break;
        }
        else if (!strcmp(sql, "cls\n"))
        {
            system("clear");
        }
        else
        {
            inputParser(sql, userCursor);
        }
    }
}