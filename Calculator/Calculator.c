#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <malloc.h>
#include <stdlib.h>

// 스택 최대 크기 지정
#define MAX_SIZE 100

typedef struct Node{
   char data;
   struct Node *next;
} Node;

typedef struct StackType{

   Node *top;
   
} StackType; 


// char형 배열 선언
char mathArr[MAX_SIZE]= {0, };

// 후위연산 배열
char postArr[MAX_SIZE] = {0, };

// 정수형 배열
double intArr[MAX_SIZE] = {0, };
int intCount = 0;

// '-' 처리를 위한 플래그 값
bool minusFlag = false;

//  스택 초기화 함수
// 스택의 최상단 요소 top
// 기본값이 -1인 이유는 스택배열에 값을 추가하면 top 변수에
// +1을 해주는데 배열의 첫번째 인덱스가 0이기에 스택이 비어있다면 -1이 들어있어야 한다.

void initStack(StackType *s) {
   s->top = NULL; 
}

// 스택이 비어있다면 원소 제거 X
int isEmpty(StackType *s) {
   
   if(s->top == NULL) {
      return 1;
   }
   return 0;   
}

// 스택에 push, pop을 수행해도 기존에 있던 데이터에 덮어쓰는 개념!!
// 스택 최상단에 값 추가
void push(StackType *s, char c) {
   
   Node* newnode = (Node *)malloc(sizeof(Node));
   newnode->data = c;
   newnode->next = s-> top;   
   
   s->top = newnode;
}

// 스택 최상단의 값 반환
// 리턴값은 변경될 수 있음.
char pop(StackType *s) {
   
   if(!isEmpty(s)) {
      
      Node *temp;

      temp = s -> top;
      char data = temp->data;
      
      s -> top = temp->next;
      free(temp);
      return data;
   }
   
}

// 스택 최상단 값 확인
char peek(StackType *s) {
   
   if(!isEmpty(s)) {
      
      Node *temp;
      temp = s -> top;
      char data = temp -> data;
      return data;
   }
}

// 연산자인지 아닌지 확인
int isOper(char op) {
   if (op == '+' || op == '-' || op == '*' || op == '/') return 1;
   else return 0;
}

// 연산자 우선순위 정책
int opPolicy(char op) {

   if (op == '(' || op == ')') return 0;
   if (op == '+' || op == '-') return 1;
   if (op == '*' || op == '/') return 2;
}

//c가 '0'~'9'면 1, 아니면 0 
int selfDigit(char c)
{
   if (c >= '0' && c <= '9')
      return 1;
   return 0;
}


// 문자를 정수로 변경하는 atoi 함수
double selfAtoi(char copy[]) {

   int i = 0;
   double result = 0;

   // 문자열을 숫자로 변환
   while (selfDigit(copy[i])) {
      result = (result * 10) + (copy[i] - '0');
      i++;
   }

   if (minusFlag) {
      result = -1 * result;
      minusFlag = false;
   }

   return result;
}


// 후위표기법으로 변경
void postFix(char arr[]) {

   StackType s;

   //스택 초기화
   initStack(&s);
   int i = 0;

   while (*arr) {

      // 괄호가 나오면 push
      if (*arr == '(') {
         push(&s, *arr);
         arr++;
      }
      // 닫는 괄호일 경우 스택에서 '(' 나올때 까지 pop
      else if (*arr == ')') {
         while (peek(&s) != '(') {
            postArr[i++] = pop(&s);
            postArr[i++] = ' ';
         }
         //  마지막 남은 '(' 연산자 삭제용 pop
         pop(&s);
         arr++;
      }
      // 연산기호일 경우 연산기호 우선순위에 따라 pop과 push
      else if (isOper(*arr)) {

         if (*(arr-1) == '(' && *(arr+1) == '(') {
         
            push(&s, *arr);
         
            postArr[i++] = '0';
            postArr[i++] = ' ';
         
            arr++;
         }

         else if (*arr == '-' && *(arr - 1) == '(') {
            
            postArr[i++] = '!';
            arr++;
         }
       
         else if (*arr == '-' && *(arr + 1) == '-') {
            postArr[i++] = '!';
            arr++;
         }
         
         else{
            
            while (!isEmpty(&s) &&
            opPolicy(peek(&s)) >= opPolicy(*arr)) {

            postArr[i++] = pop(&s);
            postArr[i++] = ' ';
         }

         push(&s, *arr);
         arr++;
            
         }

      }

      // 숫자일 경우 배열에 저장
      else if (*arr >= '0' && *arr <= '9') {
         do {
            postArr[i++] = *arr++;
         } while (*arr >= '0' && *arr <= '9');

         postArr[i++] = ' ';
      }
      else {
         arr++;
      }
   }

   // 스택에 남아있는 연산기호 pop하여 배열에 저장
   while (!isEmpty(&s)) {
      postArr[i++] = pop(&s);
      postArr[i++] = ' ';
   }
   postArr[--i] = '\0';

}


double culResult(double num1, double num2, char op) {
   
   double result = 0;
   switch (op) {
      //연산을 수행하고 스택에 저장 
   case '+': intArr[intCount++] = num2 + num1;
      break;
   case '-': intArr[intCount++] = num2 - num1;
      break;
   case '*': intArr[intCount++] = num2 * num1;
      break;
   case '/': intArr[intCount++] = num2 / num1;
      break;
   }
   
   result = intArr[intCount - 1];
   return result;
}

// postFix 계산 함수
double postResult(char fixArr[]) {

   StackType s;
   initStack(&s);

   char temp[MAX_SIZE] = { 0, };
   double resultArr[MAX_SIZE] = { 0, };
   double result = 0;
   double num1 = 0;
   double num2 = 0;
   char op;

   int resultCount = 0;
   int tempCount = 0;

   while (*fixArr) {

      if(*fixArr == '!') minusFlag = true;

      // 공백일 시 다음 문자로 이동
      if (*fixArr == ' ') fixArr++;

      // 숫자일 경우
      else if (*fixArr >= '0' && *fixArr <= '9') {
         temp[tempCount++] = *fixArr;
         
         if (*(fixArr + 1) == ' ') {
            temp[tempCount] = '\0';
            result = selfAtoi(temp);
            tempCount = 0;
            intArr[intCount++] = result;
         }
         push(&s, *fixArr);
         fixArr++;
      }

      else if (isOper(*fixArr)) {
         op = *fixArr;
         pop(&s);
         pop(&s);

         num1 = intArr[intCount - 1];
         num2 = intArr[intCount - 2];

         intCount = intCount - 2;
         result = culResult(num1, num2, op);
         fixArr++;
      }

      else {
         fixArr++;
      }
   }

   // 스택에 남아있는 연산기호 pop하여 배열에 저장
   while (!isEmpty(&s)) {
      op = *fixArr;

      pop(&s);
      num1 = intArr[intCount - 1];
      num2 = intArr[intCount - 2];
   }
   printf("result : %f\n", result);

}

// 입력 값 검증 함수
void checkBracat(char input[MAX_SIZE]) {
   
   int bracatCount = 0;
   
   // 입력 값 검증
   while(*input) {      
      if (*input == '(') {
         bracatCount++;
      } 
      
      else if(*input == ')') {
         bracatCount--;
      } 
      
      else if (!(*input >= '(' && *input <= '9')) {
         printf("입력한 수식이 숫자와 연산자로 이루어지지 않았습니다.\n");
         exit(1);
      }
      input++;
   }
   
   if(bracatCount != 0) {
      printf("괄호를 다시 확인하세요!!!\n");
      exit(1);
   }
}

int main() {

   printf("\n");
   printf("입력 >>> ");
   scanf("%s", mathArr);
   checkBracat(mathArr);
   postFix(mathArr);

   printf("\n");
   printf("======================================================================\n");
   printf("입력한 수식 : ");
   for (int i = 0; i < strlen(mathArr); i++) {
      printf("%c", mathArr[i]);
   }
   printf("\n");
   printf("======================================================================\n");
   postResult(postArr);

   return 0;

}