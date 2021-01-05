#if !defined(TRAIN_H)
#define TRAIN_H

struct Train
{
    unsigned int id;
    char *name;
    unsigned int quantity_seats;
    double rate_per_km;
    double avg_speed;
};


#endif // TRAIN_H
