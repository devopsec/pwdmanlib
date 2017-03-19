#include "server/response.h"

int main() {
    int *len;
    len = getResponseLen(ok_response);
    printf("length of ok_response: %i\n", *len);
    len = getResponseLen(bad_request_response);
    printf("length of bad_request_response: %i\n", *len);
    len = getResponseLen(bad_method_response_template);
    printf("length of bad_method_response_template before insert: %i\n", *len);
    char *resp = createReponse(bad_method_response_template, "POST\0");
    printf("response after formatting template: \n%s", resp);
    len = getResponseLen(resp);
    printf("response length after formatting template: %i\n", *len);
    free(resp);
}