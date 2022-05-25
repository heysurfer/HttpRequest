#pragma once
#include <string>
#pragma region Include
#ifdef _MSC_VER
#pragma warning(disable:4996)
#define _CRT_SECURE_NO_WARNINGS
#endif
#ifndef HTTPREQUEST_HPP
#define HTTPREQUEST_HPP
#if defined _DEBUG
#define CURL_STATICLIB
#ifdef _MSC_VER
#pragma comment(lib, "Normaliz.lib")
#endif
#include "curl/curl/curl.h"
#else
#include "curl/curl/curl.h"
#endif
size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp)
{
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}
size_t write_data(void* ptr, size_t size, size_t nmemb, FILE* stream) {
    size_t written = fwrite(ptr, size, nmemb, stream);
    return written;
}
enum HttpRequestMethod { Get = 0, Post, Delete, PUT, Head } ;
struct ResponseInfo {
    std::string Body;
    int http_code;
    bool Success;
    std::string FailInfo;
};
struct RequestInfo {
    int Method = 0;
    bool Json = false;
    std::string CustomHeader, CustomUserAgent, PostData, URL = "";
};
bool DownloadFile(const std::string Url, const std::string Location)
{
    CURL* curl;
    CURLcode res;
    curl_global_init(CURL_GLOBAL_ALL);
    curl = curl_easy_init();
    long http_code = 0;
    if (curl)
    {
        FILE* fp;
        fp = fopen(Location.c_str(), "wb");
        curl_easy_setopt(curl, CURLOPT_URL, Url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &write_data);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &fp);
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, true);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, false);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 0L);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, 10000);
        curl_easy_setopt(curl, CURLOPT_PROXY, false);
        curl_easy_setopt(curl, CURLOPT_MAXREDIRS, 50L);
        res = curl_easy_perform(curl);
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
        curl_easy_cleanup(curl);
        curl_global_cleanup();
        fclose(fp);
        if (res == CURLE_OK)
            return true;
        else
            return false;
    }
}
ResponseInfo SendRequest(RequestInfo ReqINFO)
{
    ResponseInfo Value;
    CURL* curl;
    CURLcode res;
    curl_global_init(CURL_GLOBAL_ALL);
    curl = curl_easy_init();
    long http_code = 0;
    if (curl) {
        struct curl_slist* slist1;
        slist1 = NULL;
        curl_easy_setopt(curl, CURLOPT_URL, ReqINFO.URL.c_str());
        if (ReqINFO.Method == HttpRequestMethod::Get)
            curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);
        if (ReqINFO.Method == HttpRequestMethod::Post)
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, ReqINFO.PostData);
        if (ReqINFO.Method == HttpRequestMethod::Delete)
            curl_easy_setopt(curl, CURLOPT_HTTPGET, "DELETE");
        if (ReqINFO.Method == HttpRequestMethod::PUT)
            curl_easy_setopt(curl, CURLOPT_HTTPGET, "PUT");
        if (ReqINFO.Method == HttpRequestMethod::Head)
            curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "HEAD");
        if (ReqINFO.Json)
        {
            slist1 = curl_slist_append(slist1, "Content-Type: application/json");
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, slist1);
        }
        if (ReqINFO.CustomHeader != "" && !ReqINFO.Json)
        {
            slist1 = curl_slist_append(slist1, ReqINFO.CustomHeader.c_str());
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, slist1);
        }
        if (ReqINFO.CustomUserAgent != " ")
        {
            curl_easy_setopt(curl, CURLOPT_USERAGENT, ReqINFO.CustomUserAgent.c_str());
        }
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &Value.Body);
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, true);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, false);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 0L);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, 10000);
        curl_easy_setopt(curl, CURLOPT_PROXY, false);
        curl_easy_setopt(curl, CURLOPT_MAXREDIRS, 50L);
        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            Value.Success = false;
            Value.FailInfo = curl_easy_strerror(res);
            goto Failed;
        }
        Value.Success = true;
    Failed:
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
        Value.http_code = (int)http_code;
        curl_easy_cleanup(curl);
        curl_global_cleanup();
        curl_slist_free_all(slist1);
    }
    else
        Value.Success = false;

    return Value;
}
#endif
