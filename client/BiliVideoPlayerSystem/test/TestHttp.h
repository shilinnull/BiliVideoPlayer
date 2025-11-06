#ifndef TESTHTTP_H
#define TESTHTTP_H

#include "../netclient/netclient.h"

void TestHelloRequest(netclient::NetClient& httpClient)
{
    httpClient.hello();
}

void TestPingRequest(netclient::NetClient& httpClient)
{
    httpClient.ping();
}


#endif // TESTHTTP_H
