#if !defined(USER_H)
#define USER_H

struct User
{
    const char *id;
    const char *email;
    const char *password;
	const char *first_name;
	const char *last_name;
	const char *gender;
	const char *phone;
	const char *birth_day;
	const char *passport_serial;
	const char *passport_number;
};


#endif // USER_H
