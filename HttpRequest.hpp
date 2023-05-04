#include <curl/curl.h>
#include <string>

class HttpClient {
public:
    enum HttpRequestMethod { Get = 0, Post, Delete, PUT, Head, PATCH };

    HttpClient(const std::string& url) :
        m_url(url) {
        this->curl = curl_easy_init();
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "GET");

    }

    ~HttpClient()
    {
        curl_easy_cleanup(curl);
    }

    class Response {
    public:
        std::string error = "";
        std::string body = "";
        long response_code;
        bool Success = false;
    };

    HttpClient* setMethod(HttpRequestMethod method)
    {
        switch (method) {
        case HttpRequestMethod::Delete: {
            curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "DELETE");
            break;
        }
        case HttpRequestMethod::Get: {
            curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "GET");
            break;
        }
        case HttpRequestMethod::Head: {
            curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "HEAD");
            break;
        }
        case HttpRequestMethod::PATCH: {
            curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PATCH");
            break;
        }
        case HttpRequestMethod::Post: {
            curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "POST");
            break;
        }
        case HttpRequestMethod::PUT: {
            curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT");
            break;
        }
        }
        return this;
    }

    HttpClient::Response execute()
    {
        std::string target_data = "";
        HttpClient::Response response;
        if (curl) {
            curl_easy_setopt(curl, CURLOPT_URL, m_url.c_str());
            curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
            curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, 3350);
            curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, HttpClient::write_callback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response.body);

            if (param.size() > 0 || !jsonData.empty())
            {
                if (param.size() > 0)
                {
                    for (const auto x : param) {
                        target_data += x.first + "=" + x.second + "&";
                    }
                }
                else if (!jsonData.empty())
                    target_data = jsonData;

                curl_easy_setopt(curl, CURLOPT_POSTFIELDS, target_data.c_str());
            }

            if (header.size() > 0)
            {
                for (auto x : header)
                {
                    auto str = x.first + ": " + x.second;
                    chunk = curl_slist_append(chunk, str.data());
                }
                curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);

            }

            CURLcode res = curl_easy_perform(curl);

            if (res != CURLE_OK) {
                response.error = curl_easy_strerror(res);
                std::cerr << "curl_easy_perform() failed: " << response.error << std::endl;
                response.Success = false;
            }
            else
            {
                response.Success = true;
                curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response.response_code);
            }
            if (chunk != NULL)
                curl_slist_free_all(chunk);

        }

        return  response;
    }

    HttpClient* addJson(const std::string& jsonData_str)
    {
        addHeader("Content-Type", "application/json");
        this->jsonData = (jsonData_str);
        return this;
    }

    HttpClient* addParam(const std::string& key, const std::string& value)
    {
        param.insert(std::make_pair(key, value));
        return this;
    }

    HttpClient* addHeader(const std::string& key, const std::string& value)
    {
        header.emplace(std::make_pair(key, value));
        return this;
    }

    HttpClient* setUserAgent(const std::string& userAgent_str) {
        addHeader("User-Agent", userAgent_str);
        return this;
    }

private:
    using Param = std::unordered_map<std::string, std::string>;
    using Headers = std::unordered_map<std::string, std::string>;

    Headers header;
    Param param;

    std::string m_url;
    std::string userAgent;
    std::string jsonData;

    CURL* curl;

    static size_t write_callback(char* ptr, size_t size, size_t nmemb, std::string* data) {
        data->append(ptr, size * nmemb);
        return size * nmemb;
    }

    struct curl_slist* chunk = NULL;
};
