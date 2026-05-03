# BiliVideoPlayer

## 项目简介

本项目基于 Qt6/C++17 开发的仿哔哩哔哩桌面端视频客户端，采用单例模式 + 集中式数据中心（DataCenter）实现 MVC 分层架构，将 UI 展示、业务逻辑与网络通信解耦。整合 mpv 播放内核与 FFmpeg 工具链，实现 HLS 流媒体播放、实时弹幕系统、多角色权限管理等核心功能。通过 Qt 信号槽机制驱动全异步事件流，结合 QNetworkAccessManager 封装 HTTP 通信层，搭建 MockServer 辅助全链路功能验证。

**开发环境与技术栈**：C++17、Qt6（Widgets / Network）、mpv C API、FFmpeg、CMake、QSS、JSON 序列化

# 构建

使用QtCreater6.7.3、mingw_64

客户端在client/BiliVideoPlayer

服务端在MockServer，用于测试客户端，真正的服务端未实现

---

构建后在build/*/config.ini 文件中配置ip和端口

邮箱登录：

> 账号：256652753@qq.com
>
> 验证码：123456

密码登录：

> 账号：zhangsan@qq.com
>
> 密码：123456



# 下载

Release页面下载客户端和服务端

## 界面展示

### 首页

![image-20260226150706380](./docs/image-20260226150706380.png)

点击标签分类后界面

![image-20260226150810475](./docs/image-20260226150810475.png)

搜索视频

![image-20260226150834112](./docs/image-20260226150834112.png)

### 我的

![image-20260226150855091](./docs/image-20260226150855091.png)





上传视频

![image-20260226150923327](./docs/image-20260226150923327.png)



![image-20260226150931664](./docs/image-20260226150931664.png)

修改个人信息

![image-20260226150955877](./docs/image-20260226150955877.png)

### 系统

审核管理

![image-20260226151111806](./docs/image-20260226151111806.png)

角色管理

![image-20260226151149491](./docs/image-20260226151149491.png)

新增后台用户

![image-20260226151312066](./docs/image-20260226151312066.png)

退出登录

![image-20260226151206757](./docs/image-20260226151206757.png)

### 播放页面

![image-20260226152030111](./docs/image-20260226152030111.png)



###  登录/注册

邮箱登录

![image-20260226151506390](./docs/image-20260226151506390.png)

密码登录

![image-20260226151514882](./docs/image-20260226151514882.png)



无登录状态

![image-20260226151408071](./docs/image-20260226151408071.png)