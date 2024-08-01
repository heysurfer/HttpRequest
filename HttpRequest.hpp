#ifndef HTTP_REQUEST_HPP
#define HTTP_REQUEST_HPP
#include <string>
#include <unordered_map>
#include <sstream>
#include <curl/curl.h>

class HttpClient {
public:
	enum HttpRequestMethod { Get = 0, Post, Delete, Put, Head, Patch };

	HttpClient(const std::string& url) : m_url(url), curl(curl_easy_init()) {}

	~HttpClient() {
		if (curl) {
			curl_easy_cleanup(curl);
			curl = nullptr;
		}
	}

	class Response {
	public:
		std::string error;
		std::string body;
		long response_code = 0;
		bool success = false;
		std::vector<std::string> receivedCookies;
		std::string header;
	};

	HttpClient* initCurl() {
		if (!curl) {
			curl = curl_easy_init();
		}
		return this;
	}

	HttpClient* setUrl(const std::string& url) {
		m_url = url;
		return this;
	}

	HttpClient* setProxy(const std::string& proxyInfo) {
		if (!proxyInfo.empty()) {
			m_proxyInfo = proxyInfo;
		}
		return this;
	}

	HttpClient* setMethod(HttpRequestMethod method) {
		if (curl) {
			switch (method) {
			case Get: curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L); break;
			case Post: curl_easy_setopt(curl, CURLOPT_POST, 1L); break;
			case Delete: curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "DELETE"); break;
			case Put: curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT"); break;
			case Head: curl_easy_setopt(curl, CURLOPT_NOBODY, 1L); break;
			case Patch: curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PATCH"); break;
			}
		}
		return this;
	}

	HttpClient* setData(const std::string& postData) {
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postData.c_str());
		return this;
	}

	HttpClient* addJson(const std::string& jsonData) {
		addHeader("Content-Type", "application/json");
		m_jsonData = jsonData;
		return this;
	}

	HttpClient* addParam(const std::string& key, const std::string& value) {
		param[key] = value;
		return this;
	}

	HttpClient* setAutoRedirect(bool status) {
		m_autoRedirect = status;
		return this;
	}

	HttpClient* setVerifySSL(bool status) {
		m_verifySSL = status;
		return this;
	}

	HttpClient* setTimeout(int ms) {
		m_connectTimeout = ms;
		return this;
	}

	HttpClient* addHeader(const std::string& key, const std::string& value) {
		if (!key.empty() && !value.empty()) {
			header[key] = value;
		}
		return this;
	}

	HttpClient* setUserAgent(const std::string& userAgent) {
		addHeader("User-Agent", userAgent);
		return this;
	}

	HttpClient* setCookie(const std::string& key, const std::string& value) {
		m_cookies[key] = value;
		return this;
	}

	Response execute() {
		Response response;
		if (!curl) return response;

		curl_easy_setopt(curl, CURLOPT_URL, m_url.c_str());
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response.body);
		curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, writeHeaderCallback);
		curl_easy_setopt(curl, CURLOPT_HEADERDATA, &response.header);
		curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, m_connectTimeout);
		curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
		curl_easy_setopt(curl, CURLOPT_ACCEPT_ENCODING, "");
		curl_easy_setopt(curl, CURLOPT_USERAGENT, "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/100.0.4896.79 Safari/537.36");

		if (m_autoRedirect) {
			curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
			curl_easy_setopt(curl, CURLOPT_MAXREDIRS, 5L);
			curl_easy_setopt(curl, CURLOPT_AUTOREFERER, 1L);
		}

		if (!m_proxyInfo.empty()) {
			curl_easy_setopt(curl, CURLOPT_PROXY, m_proxyInfo.c_str());
		}

		if (!m_verifySSL) {
			curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
			curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
		}
		
		std::string targetData;
		if (!m_jsonData.empty() || !param.empty()) {
			
			if (!m_jsonData.empty()) {
				targetData = m_jsonData;
			}
			else {
				for (auto it = param.begin(); it != param.end(); ++it) {
					targetData += it->first + "=" + it->second;
					if (std::next(it) != param.end()) {
						targetData += "&";
					}
				}
			}
			curl_easy_setopt(curl, CURLOPT_POSTFIELDS, targetData.c_str());
		}

		struct curl_slist* chunk = nullptr;

		if (!header.empty()) {

			for (const auto& h : header) {
				chunk = curl_slist_append(chunk, (h.first + ": " + h.second).c_str());
			}
			curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);

		}

		if (!m_cookies.empty()) {
			std::string cookie_header = "Cookie: ";
			for (const auto& cookie : m_cookies) {
				cookie_header += cookie.first + "=" + cookie.second + "; ";
			}
			chunk = curl_slist_append(chunk, cookie_header.c_str());
		}

		CURLcode res = curl_easy_perform(curl);
		if (chunk)
			curl_slist_free_all(chunk);
		if (res != CURLE_OK) {
			response.error = curl_easy_strerror(res);
			response.success = false;
		}
		else {
			response.success = true;
			curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response.response_code);
		}

		parseCookies(response);
		return response;
	}
private:

	std::string m_url;
	std::string m_jsonData;
	std::string m_proxyInfo;
	bool m_autoRedirect = true;
	bool m_verifySSL = false;
	int m_connectTimeout = 5000;

	CURL* curl = nullptr;
	std::unordered_map<std::string, std::string> header;
	std::unordered_map<std::string, std::string> m_cookies;
	std::unordered_map<std::string, std::string> param;

	static size_t writeCallback(char* ptr, size_t size, size_t nmemb, std::string* data) {
		data->append(ptr, size * nmemb);
		return size * nmemb;
	}

	static void parseCookies(Response& response) {
		auto headerLines = split(response.header, '\n');
		for (const auto& line : headerLines) {
			if (line.find("Set-Cookie:") != std::string::npos) {
				auto cookie = split(line.substr(line.find(":") + 1), ';')[0];
				response.receivedCookies.push_back(cookie);
			}
		}
	}

	static size_t writeHeaderCallback(void* contents, size_t size, size_t nmemb, void* userdata) {
		size_t totalSize = size * nmemb;
		std::string* headerData = static_cast<std::string*>(userdata);
		headerData->append(static_cast<char*>(contents), totalSize);
		return totalSize;
	}

	static std::vector<std::string> split(const std::string& s, char delim) {
		std::vector<std::string> elems;
		std::stringstream ss(s);
		std::string item;
		while (std::getline(ss, item, delim)) {
			elems.push_back(item);
		}
		return elems;
	}
};
#endif
