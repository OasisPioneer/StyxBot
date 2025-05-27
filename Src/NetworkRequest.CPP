#include "NetworkRequest.HPP"

#include <curl/curl.h>

NetworkRequest::NetworkRequest() : LOG("NetworkRequest-LOG.txt")
{
    curl_global_init(CURL_GLOBAL_ALL);
}

static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    auto* Data = static_cast<std::string*>(userp);
    Data->append(static_cast<char*>(contents), size * nmemb);
    return size * nmemb;
}

std::string NetworkRequest::HTTP_GET(const std::string& URL)
{
    CURL* curl = nullptr;
    std::string ReadBuffer;

    ReadBuffer.clear();

    curl = curl_easy_init();
    if (!curl) return "";

    curl_easy_setopt(curl, CURLOPT_URL, URL.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &ReadBuffer);

    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 2L);

    CURLcode Res = curl_easy_perform(curl);
    if (Res != CURLE_OK)
    {
        LOG.Log(LoggingSystem::ERROR, "curl_easy_perform() failed: " + std::string(curl_easy_strerror(Res)));
    }

    curl_easy_cleanup(curl);
    return ReadBuffer;
}

std::string NetworkRequest::HTTP_POST(const std::string& URL, const std::string& Data)
{
    CURL *curl = nullptr;
    std::string ReadBuffer;

    ReadBuffer.clear();

    curl = curl_easy_init();
    if (!curl) return "";

    curl_easy_setopt(curl, CURLOPT_URL, URL.c_str());
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, Data.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &ReadBuffer);

    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 2L);

    CURLcode Res = curl_easy_perform(curl);
    if (Res != CURLE_OK)
    {
        LOG.Log(LoggingSystem::ERROR, "curl_easy_perform() failed: " + std::string(curl_easy_strerror(Res)));
    }

    curl_easy_cleanup(curl);
    return ReadBuffer;
}

std::string NetworkRequest::HTTP_POST_JSON(const std::string& URL, const std::string& Data)
{
    CURL *curl = nullptr;
    std::string ReadBuffer;

    ReadBuffer.clear();

    curl = curl_easy_init();
    if (!curl) return "";

    struct curl_slist* Header = nullptr;
    Header = curl_slist_append(Header, "Content-Type: application/json");

    curl_easy_setopt(curl, CURLOPT_URL, URL.c_str());
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, Data.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, Header);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &ReadBuffer);

    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 2L);

    CURLcode Res = curl_easy_perform(curl);
    if (Res != CURLE_OK)
    {
        LOG.Log(LoggingSystem::ERROR, "curl_easy_perform() failed: " + std::string(curl_easy_strerror(Res)));
    }

    curl_slist_free_all(Header);
    curl_easy_cleanup(curl);

    return ReadBuffer;
}

NetworkRequest::~NetworkRequest()
{
    curl_global_cleanup();
}
