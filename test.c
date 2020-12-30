#define MAX_ARGUMENTS 100

struct input
{
    char *methodName; // "login"
    char *arguments[MAX_ARGUMENTS]; // ["bositkhon", "password"]
};

struct rowOutput
{
    char *methodName;
    int rowCount;
    char **records;
};

struct simpleOutput
{
    char *methodName;
    char *arguments[MAX_ARGUMENTS];
};

void parseInputCommand (char *input) {
    // login:bositkhon:password
}

struct input *inputCommand;
if (inputCommand->methodName == 'login') {
    char *login = inputCommand->arguments[0];
    char *password = inputCommand->arguments[1];

    login(login, password);
}

// login:bositkon:password

// storeTrain:title:max_seats:rate_per_km:avg_speed

// getAllSchedules:train_id:departure_city:arrival_city:departure_time:arrival_time:distance

// SELECT * FROM schedules
/*
[
    [

    ],
    [

    ]
]
*/


char string[50] = "Hello! We are learning about strtok";
// Extract the first token
char *columns = ["methodName", "username", "password"];
char * token = strtok(string, " ");
// loop through the string to extract all other tokens
struct input inputCommand;
int index = 0;
while( token != NULL ) {
    printf( " %s\n", token ); //printing each token
    if (columns[i] == "methodName") {
        strcpy(inputCommand.methodName, token)
    } else if (columns[i] == "username") {
        strcpy(inputCommand.arguments[0], token)
    } else if (columns[i] == "password") {
        strcpy(inputCommand.arguments[1], token)
    }
    token = strtok(NULL, " ");
    index++;
}