// schedule.h
json_object *getSchedule(int pk);
json_object *getAllSchedule();
int addSchedule(json_object *request);
int deleteSchedule(int pk);
int modifySchedule(json_object *request, int pk);

// feedback.h
json_object *getFeedback(int pk);
json_object *getAllFeedbacks();
int addFeedback(json_object *request);
int deleteFeedback(int pk);
int modifyFeedback(json_object *request, int pk);

// user.h
json_object *getUser(int pk);
json_object *getAllUsers();
int addUser(json_object *request);
int deleteUser(int pk);
int modifyUser(json_object *request, int pk);

json_object *getSchedule(int pk)
{
	char query[256];
	char *ptr;
	double distance, price, ratePerKm;

	json_object *schedules = json_object_new_object();
	json_object *list_of_schedules = json_object_new_object();
	json_object *object = json_object_new_object();

	//json_object *idNum;
	json_object *trainIdNum;
	json_object *departureCityString;
    json_object *arrivalCityString;
    json_object *departureDatetimeString;
	json_object *arrivalDatetimeString;
	json_object *distanceDouble;
	json_object *priceDouble;

	json_object_object_add(schedules, "action", json_object_new_string("get-all-schedule"));
	sprintf(query, "select * from schedules join trains on trains_id=trains.id where schedule.id = %d", pk);
	
	mysql_query(conn, query);

	result = mysql_use_result(conn);
	while((row = mysql_fetch_row(result)) != NULL)
	{

		distance = strtod(row[6], &ptr);
		ratePerKm = strtod(row[10], &ptr);
		price = distance * ratePerKm;
		//idNum = json_object_new_int(row[0]);
		trainIdNum = json_object_new_int(atoi(row[1]));
		departureCityString = json_object_new_string(row[2]);
		arrivalCityString = json_object_new_string(row[3]);
		departureDatetimeString = json_object_new_string(row[4]);
		arrivalDatetimeString = json_object_new_string(row[5]);
		distanceDouble = json_object_new_double(distance);
		priceDouble = json_object_new_double(price);

		json_object_object_add(object, "train-id", trainIdNum);
		json_object_object_add(object, "departure-city", departureCityString);
		json_object_object_add(object, "arrival-city", arrivalCityString);
		json_object_object_add(object, "departure-datetime", departureDatetimeString);
		json_object_object_add(object, "arrival-datetime", arrivalDatetimeString);
		json_object_object_add(object, "distance", distanceDouble);
		json_object_object_add(object, "price", priceDouble);

		json_object_array_add(list_of_schedules, object);

	}
    json_object_object_add(schedules, "result", list_of_schedules);

    return schedules;
}

json_object *getAllSchedules()
{
	char query[256];
	char *ptr;
	double distance, price, ratePerKm;

	json_object *schedules = json_object_new_object();
	json_object *list_of_schedules = json_object_new_object();
	json_object *object = json_object_new_object();

	//json_object *idNum;
	json_object *trainIdNum;
	json_object *departureCityString;
    json_object *arrivalCityString;
    json_object *departureDatetimeString;
	json_object *arrivalDatetimeString;
	json_object *distanceDouble;
	json_object *priceDouble;

	json_object_object_add(schedules, "action", json_object_new_string("get-all-schedule"));
	sprintf(query, "select * from schedules join trains on trains_id=trains.id");

	mysql_query(conn, query);

	result = mysql_use_result(conn);
	while((row = mysql_fetch_row(result)) != NULL)
	{

		distance = strtod(row[6], &ptr);
		ratePerKm = strtod(row[10], &ptr);
		price = distance * ratePerKm;
		//idNum = json_object_new_int(row[0]);
		trainIdNum = json_object_new_int(atoi(row[1]));
		departureCityString = json_object_new_string(row[2]);
		arrivalCityString = json_object_new_string(row[3]);
		departureDatetimeString = json_object_new_string(row[4]);
		arrivalDatetimeString = json_object_new_string(row[5]);
		distanceDouble = json_object_new_double(distance);
		priceDouble = json_object_new_double(price);

		json_object_object_add(object, "train-id", trainIdNum);
		json_object_object_add(object, "departure-city", departureCityString);
		json_object_object_add(object, "arrival-city", arrivalCityString);
		json_object_object_add(object, "departure-datetime", departureDatetimeString);
		json_object_object_add(object, "arrival-datetime", arrivalDatetimeString);
		json_object_object_add(object, "distance", distanceDouble);
		json_object_object_add(object, "price", priceDouble);

		json_object_array_add(list_of_schedules, object);

	}
    json_object_object_add(schedules, "result", list_of_schedules);

    return schedules;
}

int addSchedule(json_object *request)
{
	const int train_id = json_object_get_int(json_object_object_get(request, "train-id"));
	const char *departure_city = json_object_get_string(json_object_object_get(request, "departure-city"));
	const char *arrival_city = json_object_get_string(json_object_object_get(request, "arrival-city"));
	const char *departure_datetime = json_object_get_string(json_object_object_get(request, "departure-datetime"));
	const char *arrival_datetime = json_object_get_string(json_object_object_get(request, "arrival-datetime"));
	const double distance = json_object_get_double(json_object_object_get(request, "distance"));

	char query[256];

	sprintf(query, 
		"insert into schedules (train_id, departure_city, arrival_city, departure_datetime, arrival_datetime, distance) values (%d, '%s', '%s', '%s', '%s', %lf)", 
			train_id, departure_city, arrival_city, departure_datetime, arrival_datetime, distance);

	return mysql_query(conn, query);
}

int deleteSchedule(int pk)
{
	char query[256];

	sprintf(query, "delete from schedules where id=%d", pk);//TODO after booking deletion
	return mysql_query(conn, query);
}

int modifySchedule(json_object *request, int pk)
{
	const int train_id = json_object_get_int(json_object_object_get(request, "train-id"));
	const char *departure_city = json_object_get_string(json_object_object_get(request, "departure-city"));
	const char *arrival_city = json_object_get_string(json_object_object_get(request, "arrival-city"));
	const char *departure_datetime = json_object_get_string(json_object_object_get(request, "departure-datetime"));
	const char *arrival_datetime = json_object_get_string(json_object_object_get(request, "arrival-datetime"));
	const double distance = json_object_get_double(json_object_object_get(request, "distance"));
	char query[256];

	sprintf(query, "update schedules set train_id=%d, departure_city='%s', arrival_city='%s', departure_datetime='%s', arrival_datetime='%s', distance=%lf where id=%d",
			train_id, departure_city, arrival_city, departure_datetime, arrival_datetime, distance, pk);
	
	return mysql_query(conn, query);
}

json_object * getFeedback(int pk)
{
    char query[256];

    json_object *feedback = json_object_new_object();
    json_object *list_of_feedbacks = json_object_new_object();
    json_object *object = json_object_new_object();

    json_object *idNum;
    json_object *userIdNum;
    json_object *bodyString;

    json_object_object_add(feedback, "action", json_object_new_string("get-feedback"));

    sprintf(query, "select * from feedbacks where id=%d", pk);
    mysql_query(conn, query);

    result = mysql_use_result(conn);
    while((row = mysql_fetch_row(result) != NULL))
    {
        idNum = json_object_new_int(atoi(row[0]));
        userIdNum = json_object_new_int(atoi(row[1]));
        bodyString = json_object_new_string(row[2]);

        json_object_object_add(object, "id", idNum);
        json_object_object_add(object, "user-id", userIdNum);
        json_object_object_add(object, "body", bodyString);

        json_object_array_add(list_of_feedbacks, object);
    }

    json_object_object_add(feedback, "result", list_of_feedbacks);

    return feedback;

}

json_object * getAllFeedbacks()
{
    char query[256];

    json_object *feedback = json_object_new_object();
    json_object *list_of_feedbacks = json_object_new_object();
    json_object *object = json_object_new_object();

    json_object *idNum;
    json_object *userIdNum;
    json_object *bodyString;

    json_object_object_add(feedback, "action", json_object_new_string("get-all-feedbacks"));

    sprintf(query, "select * from feedbacks");
    mysql_query(conn, query);

    result = mysql_use_result(conn);
    while((row = mysql_fetch_row(result) != NULL))
    {
        idNum = json_object_new_int(atoi(row[0]));
        userIdNum = json_object_new_int(atoi(row[1]));
        bodyString = json_object_new_string(row[2]);

        json_object_object_add(object, "id", idNum);
        json_object_object_add(object, "user-id", userIdNum);
        json_object_object_add(object, "body", bodyString);

        json_object_array_add(list_of_feedbacks, object);
    }

    json_object_object_add(feedback, "result", list_of_feedbacks);

    return feedback;

}

int addFeedback(json_object *request)
{
	const int user_id = json_object_get_int(json_object_object_get(request, "user-id"));
	const char *body = json_object_get_string(json_object_object_get(request, "body"));
	
    char query[256];

    sprintf(query, "insert into feedbacks (user_id, body) values (%d, '%s')", user_id, body);

    return mysql_query(conn, query);

}

int deleteFeedback(int pk)
{
	char query[256];

	sprintf(query, "delete from feedbacks where id=%d", pk);
	return mysql_query(conn, query);
}

int modifyFeedback(json_object *request, int pk)
{
    const int id = json_object_get_int(json_object_object_get(request, "id"));
	const int user_id = json_object_get_int(json_object_object_get(request, "user-id"));
	const char *body = json_object_get_string(json_object_object_get(request, "body"));
	
    char query[256];

    sprintf(query, "update feedbacks set id=%d, user_id=%d, body='%s'", id, user_id, body);

    return mysql_query(conn, query);
}

json_object * getUser(int pk)
{
    char query[256];

    json_object *user = json_object_new_object();
    json_object *list_of_users = json_object_new_object();
    json_object *object = json_object_new_object();

    json_object *idNum;
    json_object *emailString;
    json_object *passwordString;
    json_object *firstNameString;
    json_object *lastNameString;
    json_object *genderString;
    json_object *phoneString;
    json_object *birthDateString;
    json_object *passportSerieString;
    json_object *passportNumberString;

    json_object_object_add(user, "action", json_object_new_string("get-user"));

    sprintf(query, "select * from user where id=%d", pk);
    mysql_query(conn, query);

    result = mysql_use_result(conn);
    while((row = mysql_fetch_row(result) != NULL))
    {
        idNum = json_object_new_int(atoi(row[0]));
        emailString = json_object_new_string(row[1]);
        passwordString = json_object_new_string(row[2]);
        firstNameString = json_object_new_string(row[3]);
        lastNameString = json_object_new_string(row[4]);
        genderString = json_object_new_string(row[5]);
        phoneString = json_object_new_string(row[6]);
        birthDateString = json_object_new_string(row[7]);
        passportSerieString = json_object_new_string(row[8]);
        passportNumberString = json_object_new_string(row[9]);

        json_object_object_add(object, "id", idNum);
        json_object_object_add(object, "email", emailString);
        json_object_object_add(object, "password", passwordString);
        json_object_object_add(object, "first-name", firstNameString);
        json_object_object_add(object, "last-name", lastNameString);
        json_object_object_add(object, "gender", genderString);
        json_object_object_add(object, "phone", phoneString);
        json_object_object_add(object, "birth-date", birthDateString);
        json_object_object_add(object, "passport-serie", passportSerieString);
        json_object_object_add(object, "passport-number", passportNumberString);

        json_object_array_add(list_of_users, object);
    }

    json_object_object_add(user, "result", list_of_users);

    return user;

}

json_object * getAllUsers()
{
    char query[256];

    json_object *user = json_object_new_object();
    json_object *list_of_users = json_object_new_object();
    json_object *object = json_object_new_object();

    json_object *idNum;
    json_object *emailString;
    json_object *passwordString;
    json_object *firstNameString;
    json_object *lastNameString;
    json_object *genderString;
    json_object *phoneString;
    json_object *birthDateString;
    json_object *passportSerieString;
    json_object *passportNumberString;

    json_object_object_add(user, "action", json_object_new_string("get-user"));

    sprintf(query, "select * from users");
    mysql_query(conn, query);

    result = mysql_use_result(conn);
    while((row = mysql_fetch_row(result) != NULL))
    {
        idNum = json_object_new_int(atoi(row[0]));
        emailString = json_object_new_string(row[1]);
        passwordString = json_object_new_string(row[2]);
        firstNameString = json_object_new_string(row[3]);
        lastNameString = json_object_new_string(row[4]);
        genderString = json_object_new_string(row[5]);
        phoneString = json_object_new_string(row[6]);
        birthDateString = json_object_new_string(row[7]);
        passportSerieString = json_object_new_string(row[8]);
        passportNumberString = json_object_new_string(row[9]);

        json_object_object_add(object, "id", idNum);
        json_object_object_add(object, "email", emailString);
        json_object_object_add(object, "password", passwordString);
        json_object_object_add(object, "first-name", firstNameString);
        json_object_object_add(object, "last-name", lastNameString);
        json_object_object_add(object, "gender", genderString);
        json_object_object_add(object, "phone", phoneString);
        json_object_object_add(object, "birth-date", birthDateString);
        json_object_object_add(object, "passport-serie", passportSerieString);
        json_object_object_add(object, "passport-number", passportNumberString);

        json_object_array_add(list_of_users, object);
    }

    json_object_object_add(user, "result", list_of_users);

    return user;

}

int addUser(json_object *request)
{
    const char *emailString = json_object_get_string(json_object_object_get(request, "email"));
    const char *passwordString = json_object_get_string(json_object_object_get(request, "password"));
    const char *firstNameString = json_object_get_string(json_object_object_get(request, "first-name "));
    const char *lastNameString = json_object_get_string(json_object_object_get(request, "last-name"));
    const char *genderString = json_object_get_string(json_object_object_get(request, "gender"));
    const char *phoneString = json_object_get_string(json_object_object_get(request, "phone"));
    const char *birthDateString = json_object_get_string(json_object_object_get(request, "birth-date"));
    const char *passportSerieString = json_object_get_string(json_object_object_get(request, "passport-seria"));
    const char *passportNumberString = json_object_get_string(json_object_object_get(request, "passport-number"));
	
    char query[256];

    sprintf(query, "insert into users (email, password, first_name, last_name, gender, phone, birth_date, passport_seria, passport_number) values ('%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s')",
        emailString, passwordString, firstNameString, lastNameString, genderString, phoneString, birthDateString, passportSerieString, passportNumberString);

    return mysql_query(conn, query);

}

int removeUser(int pk)
{
    char query[256];

    sprintf(query, "delete from users where id=%d", pk);
    return mysql_query(conn, query);
}

int modifyUser(json_object *request, int pk)
{
    const char *emailString = json_object_get_string(json_object_object_get(request, "email"));
    const char *passwordString = json_object_get_string(json_object_object_get(request, "password"));
    const char *firstNameString = json_object_get_string(json_object_object_get(request, "first-name "));
    const char *lastNameString = json_object_get_string(json_object_object_get(request, "last-name"));
    const char *genderString = json_object_get_string(json_object_object_get(request, "gender"));
    const char *phoneString = json_object_get_string(json_object_object_get(request, "phone"));
    const char *birthDateString = json_object_get_string(json_object_object_get(request, "birth-date"));
    const char *passportSerieString = json_object_get_string(json_object_object_get(request, "passport-seria"));
    const char *passportNumberString = json_object_get_string(json_object_object_get(request, "passport-number"));
	
    char query[256];

    sprintf(query, "update users set email='%s', password='%s', first_name='%s', last_name='%s', gender='%s', phone='%s', birth_date='%s', passport_seria='%s', passport_number='%s')",
        emailString, passwordString, firstNameString, lastNameString, genderString, phoneString, birthDateString, passportSerieString, passportNumberString);

    return mysql_query(conn, query);
}