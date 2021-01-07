#if !defined(REQUEST_RESPONSE_H)
#define REQUEST_RESPONSE_H

#include <json-c/json.h>

json_object *request;
json_object *response;

void request_init();
void response_init();

void request_init()
{
    request = json_object_new_object();
    json_object_object_add(request, "action", NULL);
    json_object_object_add(request, "payload", NULL);
}

void response_init()
{
    response = json_object_new_object();
    json_object_object_add(response, "action", NULL);
    json_object_object_add(response, "payload", NULL);
}

#endif // REQUEST_RESPONSE_H
