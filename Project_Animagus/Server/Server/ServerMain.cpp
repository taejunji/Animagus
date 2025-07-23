#include "pch.h"
#include "GameServer.h"
#include "DBManager.h"
#include "TextDBManager.h"

using namespace std;

BOOL WINAPI ConsoleHandler(DWORD signal)
{
    if (signal == CTRL_CLOSE_EVENT) {
        std::cout << "[INFO] Close event detected. Cleaning up...\n";

#ifdef _DBMODE
        auto& instance = DBManager::GetInstance();
#elif
        auto& instance = TextDBManager::GetInstance();
#endif
        instance.DBDisconnect();

        Sleep(100);
    }
    return TRUE;
}

int main()
{
    SetConsoleCtrlHandler(ConsoleHandler, TRUE);

    GameServerRef gameserver = std::make_shared<GameServer>();

    if (false == gameserver->Initialize()) {
        std::cerr << "[GameServer] Initialization failed." << std::endl;
        return -1;
    }

    gameserver->Run();

    gameserver->Shutdown();
    return 0;
}