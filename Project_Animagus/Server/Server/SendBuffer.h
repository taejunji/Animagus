#pragma once


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

