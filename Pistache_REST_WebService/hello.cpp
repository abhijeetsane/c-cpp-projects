#include <iostream>
#include "pistache/endpoint.h"
#include "pistache/tcp.h"

using namespace Pistache;
unsigned int port = 9080;

class HelloHandler : public Http::Handler {
  public:
    HTTP_PROTOTYPE(HelloHandler)
    void onRequest(const Http::Request& request, Http::ResponseWriter response) {
        response.send(Http::Code::Ok, "Hello World");
    }
};

int main()
{
    Pistache::Address addr(Pistache::Ipv4::any(), Pistache::Port(port));
    auto opts = Pistache::Http::Endpoint::options().threads(2).flags(Tcp::Options::ReuseAddr);
    Http::Endpoint server(addr);
    server.init(opts);
    server.setHandler(Http::make_handler<HelloHandler>());
    std::cout << "Starting RESTful appserver using Pistache on port  " << port << std::endl;
    server.serve();
    server.shutdown();
}
