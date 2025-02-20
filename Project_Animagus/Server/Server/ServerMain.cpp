#include "pch.h"
#include "GameServer.h"


using namespace std;

int main()
{
    GameServer gameserver;

    if (!gameserver.Initialize()) {
        std::cerr << "[GameServer] Initialization failed." << std::endl;
        return -1;
    }

    gameserver.Run();


    gameserver.Shutdown();
    return 0;
}