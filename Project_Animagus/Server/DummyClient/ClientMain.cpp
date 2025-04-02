#include "pch.h"
#include "ClientService.h"

using namespace std;

int main()
{
    this_thread::sleep_for(chrono::seconds(5));

    ClientServiceRef client = std::make_shared<ClientService>(4000);

    if (false == client->Initialize()) {
        std::cerr << "[Client] Initialization failed." << std::endl;
        return -1;
    }

    client->Start();

    client->ShutDown();
    return 0;
}
