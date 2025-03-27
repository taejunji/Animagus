#include "pch.h"
#include "ClientService.h"

using namespace std;

int main()
{
    ClientServiceRef client = std::make_shared<ClientService>(1000);

    if (false == client->Initialize()) {
        std::cerr << "[Client] Initialization failed." << std::endl;
        return -1;
    }

    client->Start();

    return 0;
}
