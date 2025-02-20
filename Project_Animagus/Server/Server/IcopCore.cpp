#include "IocpCore.h"
#include "IocpEvent.h"


IocpCore::IocpCore()
{
    _iocpHandle = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 0);
    if (_iocpHandle != INVALID_HANDLE_VALUE)
    {
        error_display(::GetLastError());
        exit(-1);
    }
}

IocpCore::~IocpCore()
{
    ::CloseHandle(_iocpHandle);
}

bool IocpCore::Register(IocpObjectRef iocpObject)
{
    return ::CreateIoCompletionPort(iocpObject->GetHandle(), _iocpHandle, /*key*/0, 0);
}

bool IocpCore::Dispatch(uint32 timeoutMs)
{
    DWORD numOfBytes = 0;
    ULONG_PTR key = 0;
    //IocpObject* iocpObject = nullptr;
    IocpEvent* iocpEvent = nullptr;

    if (::GetQueuedCompletionStatus(_iocpHandle, OUT & numOfBytes, OUT & key,
        OUT reinterpret_cast<LPOVERLAPPED*>(&iocpEvent), timeoutMs))
    {
        IocpObjectRef iocpObject = iocpEvent->owner;
        iocpObject->Dispatch(iocpEvent, numOfBytes);
    }
    else
    {
        int errorCode = ::WSAGetLastError();
        switch (errorCode)
        {
        case WAIT_TIMEOUT:
            return false;
        default:    // 그 비동기 뭐드라 그거 펜딩
            IocpObjectRef iocpObject = iocpEvent->owner;
            iocpObject->Dispatch(iocpEvent, numOfBytes);
            break;
        }
    }

    return true;
}
