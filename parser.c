#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_ARGUMENTS 1000

struct ioCommand
{
    char *methodName;
    char *arguments[MAX_ARGUMENTS];
};


int main()
{
   // клиент отправляет сокету
    
  char string[50] = "login:bositkhon:password";
  char *token = strtok(string, ":");
  
  struct ioCommand cmd;
  
  char *columns[2] = {"methodName", "arguments"};
  int index = 0;
  int argIndex = 0; 
  
  while( token != NULL ) {
    if (columns[index] == "methodName") {
      cmd.methodName = token;
      index++;
    } else if(columns[index] == "arguments") {
      cmd.arguments[argIndex++] = token;
    }
    token = strtok(NULL, ":");
  }
  
  printf("%s, %s, %s\n", cmd.methodName, cmd.arguments[0], cmd.arguments[1]);

    //char * token = strtok(command, ":");
    // loop through the string to extract all other tokens
//    struct ioCommand com;

 //   int index = 0;
   // int argIndex = 0;
  //  while( token != NULL ) {
        //printf( " %s\n", token ); //printing each token
        //if (columns[index] == "methodName") {
        //    strcpy(com.methodName, token);
        //} else if (columns[index] == "username") {
        //    strcpy(com.arguments[0], token);
        //    argIndex++;
        //}
        
      //  token = strtok(NULL, " ");
        //index++;
    //}

}
