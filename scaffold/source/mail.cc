#include "mail.h"
#include "log.h"

namespace Bilicode {
    
    // 操作：初始化全局配置，保存服务配置信息
    MailClient::MailClient(const mail_settings& settings) : _settings(settings) {
        auto ret = curl_global_init(CURL_GLOBAL_DEFAULT);
        if (ret != CURLE_OK) {
            ERR("初始化CURL全局配置失败: {}",  curl_easy_strerror(ret));
            abort();
        }
    }
    // 操作：释放全局配置资源。
    MailClient::~MailClient() { curl_global_cleanup(); }
    // 操作：发送邮件
    bool MailClient::send(const std::string& to, const std::string& code) {
        auto curl = curl_easy_init();
        if (curl == nullptr) {
            ERR("构造CURL操作句柄失败!");
            return false;
        }
        curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 15L);  // 连接超时15秒
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30L);
        //不验证对方证书，只适用于测试开发阶段，实际运行阶段不适用
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L); // 不验证对等端SSL证书
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L); // 不验证主机名
        auto ret = curl_easy_setopt(curl, CURLOPT_URL, _settings.url.c_str());
        if (ret != CURLE_OK) {
            ERR("设置CURL的CURLOPT_URL参数失败: {}", curl_easy_strerror(ret));
            return false;
        }
        ret = curl_easy_setopt(curl, CURLOPT_USERNAME, _settings.username.c_str());
        if (ret != CURLE_OK) {
            ERR("设置CURL的CURLOPT_USERNAME参数失败: {}", curl_easy_strerror(ret));
            return false;
        }
        ret = curl_easy_setopt(curl, CURLOPT_PASSWORD, _settings.password.c_str());
        if (ret != CURLE_OK) {
            ERR("设置CURL的CURLOPT_PASSWORD参数失败: {}", curl_easy_strerror(ret));
            return false;
        }
        ret = curl_easy_setopt(curl, CURLOPT_MAIL_FROM, _settings.from.c_str());
        if (ret != CURLE_OK) {
            ERR("设置CURL的CURLOPT_MAIL_FROM参数失败: {}", curl_easy_strerror(ret));
            return false;
        }
        struct curl_slist *cs = nullptr;
        cs = curl_slist_append(cs, to.c_str());
        ret = curl_easy_setopt(curl, CURLOPT_MAIL_RCPT, cs);
        if (ret != CURLE_OK) {
            ERR("设置CURL的CURLOPT_MAIL_RCPT参数失败: {}", curl_easy_strerror(ret));
            return false;
        }
        auto body = codeBody(to, code);
        ret = curl_easy_setopt(curl, CURLOPT_READDATA, (void*)&body);
        if (ret != CURLE_OK) {
            ERR("设置CURL的CURLOPT_READDATA参数失败: {}", curl_easy_strerror(ret));
            return false;
        }
        ret = curl_easy_setopt(curl, CURLOPT_READFUNCTION, &MailClient::callback);
        if (ret != CURLE_OK) {
            ERR("设置CURL的CURLOPT_READFUNCTION参数失败: {}", curl_easy_strerror(ret));
            return false;
        }
        ret = curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
        if (ret != CURLE_OK) {
            ERR("设置CURL的CURLOPT_UPLOAD参数失败: {}", curl_easy_strerror(ret));
            return false;
        }
        ret = curl_easy_perform(curl);
        if (ret!= CURLE_OK) {
            ERR("请求邮件服务器失败: {}", curl_easy_strerror(ret));
            return false;
        }
        curl_slist_free_all(cs);
        curl_easy_cleanup(curl);
        DBG("发送邮件成功: {}-{}", to, code);
        return true;
    }
    // 操作：构造邮件正文
    std::stringstream MailClient::codeBody(const std::string& to, const std::string& code) {
        std::stringstream ss;
        ss << "Subject: " << _title << "\r\n"; //邮件标题
        ss << "Content-Type: text/html\r\n";
        ss << "\r\n";
        ss << "<html><body><p>你的验证码: <b>" << code << "</b></p><p>验证码将在5分钟后失效.</p></body></html>\r\n";
        return ss;
    }
    // 操作：curl请求处理回调
    size_t MailClient::callback(char *buffer, size_t size, size_t nitems, void *userdata) {
        std::stringstream *ss = (std::stringstream*)userdata;
        ss->read(buffer, size * nitems);
        return ss->gcount();
    }
}