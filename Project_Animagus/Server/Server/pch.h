#pragma once
#include <iostream>
#include <memory>
#include <functional>

#include <vector>
#include <array>
#include <string>
#include <list>
#include <queue>
#include <stack>
#include <set>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <concurrent_priority_queue.h>

#include <thread>
#include <chrono>
#include <mutex>
#include <atomic>

#include <WS2tcpip.h>
#include <MSWSock.h>
#include <windows.h>

#include "Types.h"

#pragma comment (lib, "WS2_32.lib")
#pragma comment (lib, "MSWSock.lib")

//const char* SERVER_IP = "127.0.0.1";
#define SERVER_PORT  7777

//using namespace std;
//using namespace chrono;

constexpr int BUFSIZE = 1048;
const int  MAX_NAME_SIZE = 20;
const int  MAX_USER = 5000;

struct PacketHeader
{
    uint16 size;
    uint16 id; // 프로토콜ID
};


void error_display(int err_no);

