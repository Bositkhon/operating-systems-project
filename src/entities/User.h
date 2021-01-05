#if !defined(USER_H)
#define USER_H

struct User
{
    unsigned int id;
    char *email;
    char *password;
	char *first_name;
	char *last_name;
	char *gender;
	char *phone;
	char *birth_day;
	char *passport_serial;
	char *passport_number;
};


#endif // USER_H
