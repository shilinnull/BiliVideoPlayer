#pragma once
#include <curl/curl.h>
#include <iostream>
#include <sstream>
#include <memory>

namespace Bilicode {
    struct mail_settings {
        std::string username;
        std::string password;
        std::string url;
        std::string from;
    };

    class CodeClient {
        public:
        CodeClient() = default;
        ~CodeClient() = default;
        virtual bool send(const std::string& to, const std::string& code) = 0;
    };

    class MailClient : public CodeClient {
        public:
            using ptr = std::shared_ptr<MailClient>;
            // 操作：初始化全局配置，保存服务配置信息
            MailClient(const mail_settings& settings);
            // 操作：释放全局配置资源。
            ~MailClient();
            // 操作：发送邮件
            virtual bool send(const std::string& to, const std::string& code) override;
        private:
            // 操作：构造邮件正文
            std::stringstream codeBody(const std::string& to, const std::string& code);
            // 操作：curl请求处理回调
            static size_t callback(char *buffer, size_t size, size_t nitems, void *userdata);
        private:
            const std::string _title = "验证码";
            mail_settings _settings;
    };
}