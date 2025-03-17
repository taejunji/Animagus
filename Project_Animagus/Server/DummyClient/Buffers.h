#pragma once

/*--------------
    SendBuffer
----------------*/

class SendBuffer
{
public:
    SendBuffer(BYTE* buffer, uint32 allocSize);
    ~SendBuffer();

    BYTE*           Buffer() { return _buffer; }
    uint32			AllocSize() { return _allocSize; }
    int32			WriteSize() { return _writeSize; }

    void            Close(uint32 writeSize);

private:
    BYTE*           _buffer;
    uint32			_allocSize = 0;
    uint32			_writeSize = 0;	// 실질적으로 버퍼를 사용하는 크기
};


/*--------------
    RecvBuffer
----------------*/

class RecvBuffer
{
    enum { BUFFER_COUNT = 10 };

public:
    RecvBuffer(int32 bufferSize);
    ~RecvBuffer();

    void			Clean();
    bool			OnRead(int32 numOfBytes);
    bool			OnWrite(int32 numOfBytes);

    BYTE*           ReadPos() { return &_buffer[_readPos]; }
    BYTE*           WritePos() { return &_buffer[_writePos]; }
    int32			DataSize() { return _writePos - _readPos; }
    int32			FreeSize() { return _capacity - _writePos; }

private:
    int32			_capacity = 0;
    int32			_bufferSize = 0;
    int32			_readPos = 0;
    int32			_writePos = 0;
    std::vector<BYTE>	_buffer;
};


