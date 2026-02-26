#include <httplib.h>
#include <iostream>

using namespace std;

int main() {
    httplib::Server svr;

    svr.Get("/hello", [](const httplib::Request &req, httplib::Response &rsp){
        cout << req.method << '\n';
        cout << req.path << '\n';
        cout << req.body << '\n';
        for(auto it : req.headers) {
            cout << it.first << ":" << it.second << '\n';
        }
        cout << '\n';
        for(auto it : req.params) {
            cout << it.first << ":" << it.second << '\n';
        }
        string html_body = "<html><body><h1>Hello World</h1></body></html>";
        rsp.set_content(html_body, "text/html");
        rsp.status = 200;
    });

    svr.Get(R"(/numbers/(\d+))", [](const httplib::Request &req, httplib::Response &rsp){
        cout << req.method << '\n';
        cout << req.path << '\n';
        for(auto it : req.matches) {
            cout << it << '\n';
        }
        rsp.status = 200;
    });

    svr.listen("0.0.0.0", 8000);
    return 0;
}