#include "pch.h"
#include "ClientService.h"

using namespace std;

int main()
{
    this_thread::sleep_for(chrono::seconds(5));

    ClientServiceRef dummyClient = std::make_shared<ClientService>(1000);

    if (false == dummyClient->Initialize()) {
        std::cerr << "[Client] Initialization failed." << std::endl;
        return -1;
    }

    dummyClient->Start();

    dummyClient->ShutDown();
    return 0;
}
