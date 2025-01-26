#include <queue>
#include <string>

#include "../Entry.h"

namespace Sider::Storage::Entry
{
    class CQueueEntry : public QueueEntry
    {
        private:
        std::queue<std::string> m_queue;

        public:
        CQueueEntry(std::queue<std::string> queue) :
            m_queue(queue)
        {}

        Type type() const override
        {
            return Type::QUEUE;
        }

        bool isExpired() override
        {
            return m_queue.empty();
        }

        void queue(std::string value) override
        {
            m_queue.push(value);
        }

        void dequeue() override
        {
            m_queue.pop();
        }

        const std::string get() override
        {
            return m_queue.front();
        }
    };

    QueueEntry* initQueueEntry()
    {
        return new CQueueEntry(std::queue<std::string>());
    }
}