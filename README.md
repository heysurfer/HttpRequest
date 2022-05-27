

Example Get Request:
```c++
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
```

Example Post Request:
```c++
    HTTP::RequestInfo Req;
    Req.URL = "URL";
    Req.Method = HTTP::Post;
    Req.PostData = "post1=hello&post2=aa";
    auto value = HTTP::SendRequest(Req);
    if (value.Success) {
        printf("Response Body : %s\n", value.Body.c_str());
        printf("Response Status Code : %i\n" , value.http_code);
    }
    else
        printf("Curl Failed: %s\n", value.FailInfo.c_str());

```

Example Download:
```c++
    if (HTTP::DownloadFile("URL", "LOCATION"))
        printf("Success Download\n");
    else
        printf("Failed To Download\n");

```
