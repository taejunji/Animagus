#include "pch.h"
#include "SendBuffer.h"


SendBuffer::SendBuffer(BYTE* buffer, uint32 allocSize)
    : _buffer(buffer), _allocSize(allocSize)
{
}

SendBuffer::~SendBuffer()
{
}

void SendBuffer::Close(uint32 writeSize)
{
    if (_allocSize < writeSize) return;
    _writeSize = writeSize;
}

