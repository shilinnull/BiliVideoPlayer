#include <curl/curl.h>
#include <iostream>
#include <sstream>

size_t callback(char *buffer, size_t size, size_t nitems, void *userdata){
    std::stringstream *ss = (std::stringstream*)userdata;
    ss->read(buffer, size * nitems);
    return ss->gcount();
}

int main()
{
    const std::string username = "shilinnull@163.com";
    const std::string password = "VWtv6PzsPT7EEy9n";
    const std::string url = "smtps://smtp.163.com:465";
    const std::string from = "shilinnull@163.com";
    const std::string to = "256652753@qq.com";
    // 1. 初始化全局配置
    auto ret = curl_global_init(CURL_GLOBAL_DEFAULT);
    if (ret != CURLE_OK) {
        std::cout << "curl_global_init failed: " << curl_easy_strerror(ret) << std::endl;
        return -1;
    }
    // 2. 构造操作句柄
    auto curl = curl_easy_init();
    if (curl == nullptr) {
        std::cout << "curl_easy_init failed" << std::endl;
        return -1;
    }
    // 3. 设置请求参数：url，username，password，body, from, to
    ret = curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    if (ret != CURLE_OK) {
        std::cout << "curl_easy_setopt CURLOPT_URL failed: " << curl_easy_strerror(ret) << std::endl;
        return -1;
    }
    ret = curl_easy_setopt(curl, CURLOPT_USERNAME, username.c_str());
    if (ret != CURLE_OK) {
        std::cout << "curl_easy_setopt CURLOPT_USERNAME failed: " << curl_easy_strerror(ret) << std::endl;
        return -1;
    }
    ret = curl_easy_setopt(curl, CURLOPT_PASSWORD, password.c_str());
    if (ret != CURLE_OK) {
        std::cout << "curl_easy_setopt CURLOPT_PASSWORD failed: " << curl_easy_strerror(ret) << std::endl;
        return -1;
    }
    ret = curl_easy_setopt(curl, CURLOPT_MAIL_FROM, from.c_str());
    if (ret != CURLE_OK) {
        std::cout << "curl_easy_setopt CURLOPT_MAIL_FROM failed: " << curl_easy_strerror(ret) << std::endl;
        return -1;
    }
    struct curl_slist *cs = nullptr;
    cs = curl_slist_append(cs, to.c_str());
    ret = curl_easy_setopt(curl, CURLOPT_MAIL_RCPT, cs);
    if (ret != CURLE_OK) {
        std::cout << "curl_easy_setopt CURLOPT_MAIL_RCPT failed: " << curl_easy_strerror(ret) << std::endl;
        return -1;
    }
    std::stringstream ss;
    // ss << "From: " << from << "\r\n";
    // ss << "To: " << to << "\r\n"; //如果有多个收件人，则收件人之间以,间隔
    ss << "Subject: 验证码\r\n";
    ss << "Content-Type: text/html\r\n";
    ss << "\r\n";
    ss << "<html><body><p>你的验证码: <b>5678</b></p><p>验证码将在5分钟后失效.</p></body></html>\r\n";
    ret = curl_easy_setopt(curl, CURLOPT_READDATA, (void*)&ss);
    if (ret != CURLE_OK) {
        std::cout << "curl_easy_setopt CURLOPT_READDATA failed: " << curl_easy_strerror(ret) << std::endl;
        return -1;
    }
    ret = curl_easy_setopt(curl, CURLOPT_READFUNCTION, callback);
    if (ret != CURLE_OK) {
        std::cout << "curl_easy_setopt CURLOPT_READFUNCTION failed: " << curl_easy_strerror(ret) << std::endl;
        return -1;
    }
    ret = curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
    if (ret != CURLE_OK) {
        std::cout << "curl_easy_setopt CURLOPT_UPLOAD failed: " << curl_easy_strerror(ret) << std::endl;
        return -1;
    }
    // 4. 执行请求
    ret = curl_easy_perform(curl);
    if (ret!= CURLE_OK) {
        std::cout << "curl_easy_perform failed: " << curl_easy_strerror(ret) << std::endl;
        return -1;
    }
    // 5. 清理资源
    curl_slist_free_all(cs);
    curl_easy_cleanup(curl);
    curl_global_cleanup();
    return 0;
}