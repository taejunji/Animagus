#pragma once
#include "pch.h"


//-----------------------------------------
//	            IocpObject
// 소켓핸들 관리, 해당 소켓에 대한 이벤트 처리
//-----------------------------------------

class IocpObject : public std::enable_shared_from_this<IocpObject>
{
public:
    virtual HANDLE	GetHandle() abstract;
    virtual void	Dispatch(class IocpEvent* iocpEvent, int32 numOfBytes = 0) abstract;
};


//----------------
//	 IocpCore
//----------------

class IocpCore
{
public:
    IocpCore();
    ~IocpCore();

    HANDLE		GetHandle() { return _iocpHandle; }

    bool		Register(IocpObjectRef iocpObject);		// 소켓을 CP에 등록하는 함수
    bool		Dispatch(uint32 timeoutMs = INFINITE);	// CP를 감시하면서 대기하는 함수, 작업스레드 원청느낌

private:
    HANDLE		_iocpHandle;
};

