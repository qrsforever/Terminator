#include "LogPool.h"

#include "LogFilter.h"
#include "RingBuffer.h"

#include <pthread.h>

namespace IBox {

static pthread_mutex_t g_mutex = PTHREAD_MUTEX_INITIALIZER;

LogPool::LogPool()
	: m_filterHead(0)
{
}

LogPool::~LogPool()
{
    if (m_filterHead)
        delete m_filterHead;
}

bool 
LogPool::attachFilter(LogFilter* filter, int index)
{
    pthread_mutex_lock(&g_mutex);

    if (!m_filterHead)
        m_filterHead = filter;
    else {
        LogFilter* pre = m_filterHead;
        int i;
    
        for (i = 0; i < index; i++) {
            if (pre->m_next)
                pre = pre->m_next;
            else
                break;
        }
        filter->m_next = pre->m_next;
        pre->m_next = filter;
    }

    pthread_mutex_unlock(&g_mutex);
    return true;
}

bool 
LogPool::detachFilter(LogFilter* filter)
{
    LogFilter* pre = m_filterHead;

    if (!filter || !pre)
        return false;

    pthread_mutex_lock(&g_mutex);

    if (m_filterHead == filter)
        m_filterHead = filter->m_next;
    else {
        for (; pre; pre = pre->m_next)
            if (pre->m_next == filter) {
                pre->m_next = filter->m_next;
                break;
            }
    }

    pthread_mutex_unlock(&g_mutex);
    return true;
}

bool 
LogPool::onDataArrive()
{
    return sendEmptyMessage(MC_DataArrive);
}

bool 
LogPool::onError()
{
    return sendEmptyMessage(MC_Error);
}

bool 
LogPool::onEnd()
{
    return sendEmptyMessage(MC_End);
}

void 
LogPool::handleMessage(Message* msg)
{
    switch (msg->what) {
        case MC_DataArrive:
            receiveData();
            break;
		case MC_End:
			receiveEnd();
			break;
        case MC_Error:
            receiveError();
            break;
		default:
            break;
    }
}

void 
LogPool::receiveData()
{
    removeMessages(MC_DataArrive);
    while (1) {
        uint8_t* bufPointer;
        uint32_t bufLength;

        m_ringBuffer->getReadHead(&bufPointer, &bufLength);
        if (bufLength == 0)
            break;
        int blockSize = *((int *)(bufPointer + 0));
        int dateSize = *((int *)(bufPointer + 4));
        //printf("receiveData: %p, %d !!!!!!!!!!!!!!\n", bufPointer, bufLength);
        //fwrite(bufPointer + 8, dateSize, 1, stdout);

        pthread_mutex_lock(&g_mutex);

        LogFilter* tmp = m_filterHead;
        for (; tmp; tmp = tmp->m_next) {
            if (tmp->pushBlock(bufPointer + 8, dateSize))
                break;
        }

        pthread_mutex_unlock(&g_mutex);

        m_ringBuffer->submitRead(bufPointer, blockSize);
    }
}

void 
LogPool::receiveError()
{
}

void 
LogPool::receiveEnd()
{
}

} // namespace IBox
