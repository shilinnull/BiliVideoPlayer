#include <functional>
#include <iostream>
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/http/constants.hpp>
#include <websocketpp/logger/levels.hpp>
#include <websocketpp/server.hpp>

using namespace std;
using websocketsvr_t = websocketpp::server<websocketpp::config::asio>;

void onHttp(websocketsvr_t *server, websocketpp::connection_hdl h) {
    cout<<"处理http请求"<<endl;
    websocketsvr_t::connection_ptr conn = server->get_con_from_hdl(h);
    std::stringstream ss;
    ss << "<!doctype html><html><head>"
        << "<title>hello websocket</title><body>"
        << "<h1>hello websocketpp</h1>"
        << "</body></head></html>";
    conn->set_body(ss.str());
    conn->set_status(websocketpp::http::status_code::ok);
}
void onOpen(websocketsvr_t *server, websocketpp::connection_hdl h) {
    cout<<"连接成功"<<endl;
}
void onClose(websocketsvr_t *server, websocketpp::connection_hdl h) {
    cout<<"连接关闭"<<endl;
}
void onMessage(websocketsvr_t *server, websocketpp::connection_hdl h, websocketsvr_t::message_ptr msg) {
    cout << "收到消息" << msg->get_payload() << endl;
    server->send(h, msg->get_payload(), websocketpp::frame::opcode::text);
}

int main() {
    websocketsvr_t server;
    server.set_access_channels(websocketpp::log::alevel::none);
    server.init_asio();
    server.set_http_handler(bind(&onHttp, &server, placeholders::_1));
    server.set_open_handler(bind(&onOpen, &server, placeholders::_1));
    server.set_close_handler(bind(&onClose, &server, placeholders::_1));
    server.set_message_handler(bind(&onMessage, &server, placeholders::_1, placeholders::_2));
    server.listen(8000);
    server.start_accept();
    server.run();
    return 0;
}


