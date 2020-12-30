#include <stdio.h>
#include <string.h>

struct ioCommand
{
    char *methodName;
    char **arguments;
};


int main()
{
    // клиент отправляет сокету
    char *command = "login:bositkhon:password";

    char *columns[2] = {"methodName", "arguments"};

    char * token = strtok(command, " ");
    // loop through the string to extract all other tokens
    struct ioCommand com;

    int index = 0;
    int argIndex = 0;
    while( token != NULL ) {
        printf( " %s\n", token ); //printing each token
        if (columns[index] == "methodName") {
            strcpy(com.methodName, token);
        } else if (columns[index] == "username") {
            strcpy(com.arguments[0], token);
            argIndex++;
        }
        
        token = strtok(NULL, " ");
        index++;
    }

}