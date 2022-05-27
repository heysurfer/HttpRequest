#include "HttpRequest.hpp"

int main()
{
    HTTP::RequestInfo Req;
    Req.URL = "https://api.ipify.org?format=json";
    Req.Method = HTTP::Get;
    auto value = HTTP::SendRequest(Req);
    if (value.Success) {
        printf("Response Body : %s\n", value.Body.c_str());
        printf("Response Status Code : %i\n" , value.http_code);
    }
    else
        printf("Curl Failed: %s\n", value.FailInfo.c_str());
}
