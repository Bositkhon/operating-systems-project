#if !defined(SCHEDULE_H)
#define SCHEDULE_H

struct Schedule
{
    unsigned int id;
    unsigned int train_id;
    char *departure_city;
    char *arrival_city;
    char *departure_datetime;
    char *arrival_datetime;
    double distance;
};


#endif // SCHEDULE_H
