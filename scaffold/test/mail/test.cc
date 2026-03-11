#include <memory>
#include "../../source/mail.h"
#include "../../source/log.h"


int main()
{
    Bililog::Bililog_init();
    // 1. 构造邮件服务配置
    Bilicode::mail_settings settings = {
        .username = "shilinnull@163.com",
        .password = "VWtv6PzsPT7EEy9n",
        .url = "smtps://smtp.163.com:465",
        .from = "shilinnull@163.com",
    };
    // 2. 实例化邮件客户端对象
    auto mail = std::make_unique<Bilicode::MailClient>(settings);
    // 3. 发送邮件
    bool ret = mail->send("270988047@qq.com", "6789");
    if (!ret) {
        std::cout << "发送邮件失败！" << std::endl;
    }
    return 0;
}