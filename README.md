# HttpRequest
```c++
HttpClient ("https://example.com/").setMethod(HttpClient::Get)->execute() -> return Response Class

class Response {
    public:
        std::string error = "";
        std::string body = "";
        long response_code;
        bool Success = false;
};
```
