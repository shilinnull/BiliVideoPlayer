#ifndef TESTHTTP_H
#define TESTHTTP_H

#include "../netclient/netclient.h"

void TestHelloRequest(network::NetClient& httpClient)
{
    httpClient.hello();
}

void TestPingRequest(network::NetClient& httpClient)
{
    httpClient.ping();
}


#endif // TESTHTTP_H
