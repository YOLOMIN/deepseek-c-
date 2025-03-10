#include <iostream>
#include <cstdlib>
#include <string>
#include <curl/curl.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

const std::string API_KEY = "*";  // 替换为你的密钥
const std::string API_URL = "https://api.deepseek.com/v1/chat/completions";

bool testPortConnectivity(const std::string& host, int port) {
    std::string command = "nc -zv " + host + " " + std::to_string(port);
    int result = system(command.c_str());
    return result == 0;
}

size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

void testApiCall(const std::string& userMessage) {
    CURL* curl;
    CURLcode res;
    std::string readBuffer;

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
    if(curl) {
        struct curl_slist *headers = NULL;
        headers = curl_slist_append(headers, "Content-Type: application/json");
        headers = curl_slist_append(headers, ("Authorization: Bearer " + API_KEY).c_str());

        json jsonData = {
            {"model", "deepseek-chat"},
            {"messages", json::array({ {{"role", "user"}, {"content", userMessage}} })},
            {"stream", false},
            {"temperature", 0.7}
        };

        std::string jsonString = jsonData.dump();
        std::cout << "请求体内容:\n" << jsonString << std::endl;

        curl_easy_setopt(curl, CURLOPT_URL, API_URL.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonString.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

        res = curl_easy_perform(curl);

        if(res != CURLE_OK) {
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
        } else {
            long http_code = 0;
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
            std::cout << "HTTP状态码: " << http_code << std::endl;
            std::cout << "响应内容: " << readBuffer << std::endl;

            // 解析JSON响应
            try {
                auto jsonResponse = json::parse(readBuffer);
                if (jsonResponse.contains("choices") && !jsonResponse["choices"].empty()) {
                    std::string content = jsonResponse["choices"][0]["message"]["content"];
                    std::cout << "返回的content内容: " << content << std::endl;
                } else {
                    std::cerr << "响应中不包含'choices'字段或'choices'为空" << std::endl;
                }
            } catch (json::parse_error& e) {
                std::cerr << "JSON解析错误: " << e.what() << std::endl;
            }
        }

        curl_easy_cleanup(curl);
    }
    curl_global_cleanup();
}

int main() {
    std::cout << "=== 测试端口443连通性 ===" << std::endl;
    if (!testPortConnectivity("api.deepseek.com", 443)) {
        std::cerr << "端口443不通" << std::endl;
        return 1;
    }

    std::string userMessage;
    while (true) {
        std::cout << "请输入聊天信息 (输入'退出'结束): ";
        std::getline(std::cin, userMessage);
        if (userMessage == "退出") {
            break;
        }

        std::cout << "=== 测试API调用 ===" << std::endl;
        testApiCall(userMessage);
    }

    std::cout << "\n=== 预期结果 ===" << std::endl;
    std::cout << "1. 若返回JSON且包含'choices'字段 → API正常" << std::endl;
    std::cout << "2. 若HTTP状态码为401 → 密钥无效" << std::endl;
    std::cout << "3. 若HTTP状态码为404 → 路径错误" << std::endl;

    return 0;
}