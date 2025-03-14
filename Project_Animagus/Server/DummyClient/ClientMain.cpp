#include "pch.h"
#include "ClientService.h"

using namespace std;

int main()
{
    ClientServiceRef client = std::make_shared<ClientService>();

    if (false == client->Initialize()) {
        std::cerr << "[GameServer] Initialization failed." << std::endl;
        return -1;
    }

    client->Start();

    return 0;
}
