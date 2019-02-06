#include <stdio.h>
#include <string.h>

int checker(char input[],char check[])
{
    int i,result=1;
    for(i=0; input[i]!='\0' && check[i]!='\0'; i++) {
        if(input[i] != check[i]) {
            result=0;
            break;
        }
    }
    return result;
}

int main() {
    char input[100] = "Hello";
    char input1[30] = "Hello";

    if (strcmp(input,input1) == 0){
        printf("Hello\n");
    }
}