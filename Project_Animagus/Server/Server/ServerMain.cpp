#include "pch.h"
#include "GameServer.h"


using namespace std;

int main()
{
    GameServerRef gameserver = std::make_shared<GameServer>();

    if (false == gameserver->Initialize()) {
        std::cerr << "[GameServer] Initialization failed." << std::endl;
        return -1;
    }

    gameserver->Run();

    gameserver->Shutdown();
    return 0;
}