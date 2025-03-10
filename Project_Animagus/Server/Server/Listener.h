#pragma once


/*--------------
    Listener
---------------*/

class IocpEvent;
class AcceptEvent;

class Listener : public IocpObject
{
public:
    Listener() = default;
    ~Listener();

public:
    /* 외부에서 사용 */
    bool StartAccept(GameServerRef server);
    void CloseSocket();

public:
    /* 인터페이스 구현 */
    virtual HANDLE GetHandle() override;
    virtual void Dispatch(class IocpEvent* iocpEvent, int32 numOfBytes = 0) override;

private:
    /* 수신 관련 */
    void RegisterAccept(AcceptEvent* acceptEvent);
    void ProcessAccept(AcceptEvent* acceptEvent);

protected:
    SOCKET _socket = INVALID_SOCKET;
    std::vector<AcceptEvent*> _acceptEvents;

    GameServerRef _server;      // Listener 가 멤버로 있는 Server

};
