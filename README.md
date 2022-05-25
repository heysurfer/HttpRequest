

Example Get Request:
```c++
    RequestInfo Req;
    Req.URL = "https://api.ipify.org?format=json";
    Req.Method = Get;
    auto value = SendRequest(Req);
    if (value.Success) {
        printf("Response Body : %s\n", value.Body.c_str());
        printf("Response Status Code : %i\n" , value.http_code);
    }
    else
    {
        printf("Curl Failed: %s\n", value.FailInfo.c_str());
    }
```

Example Post Request:
```c++
    RequestInfo Req;
    Req.URL = "URL";
    Req.Method = Post;
    Req.PostData = "post1=hello&post2=aa";
    auto value = SendRequest(Req);
    if (value.Success) {
        printf("Response Body : %s\n", value.Body.c_str());
        printf("Response Status Code : %i\n" , value.http_code);
    }
    else
    {
        printf("Curl Failed: %s\n", value.FailInfo.c_str());
    }

```
