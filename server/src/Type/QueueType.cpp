#include <queue>
#include <string>

#include "../Command/Command.h"
#include "../Storage/Entry.h"
#include "../Storage/Storage.h"

namespace Sider::Storage::Entry
{
    class QueueEntry : public Entry, public NonPartitionedEntry
    {
        private:
        std::queue<std::string> m_queue;

        public:
        QueueEntry() :
            m_queue(std::queue<std::string>())
        {}

        Type type() const override
        {
            return Type::QUEUE;
        }

        bool isExpired() override
        {
            return m_queue.empty();
        }

        void queue(std::string value)
        {
            m_queue.push(value);
        }

        void dequeue()
        {
            m_queue.pop();
        }

        const std::string get() override
        {
            return m_queue.front();
        }
    };
}

namespace Sider::Command
{
    using namespace Sider::Storage;

    class QueueCommand : public Command
    {
        private:
        const Entry::Id id;
        const std::string value;

        public:
        QueueCommand(Entry::Id id, std::string value) :
            id(id),
            value(value)
        {}

        Result execute(Sider::Storage::Storage* storage) override
        {
            // todo add test when returned type is not QueueEntry

            Entry::Entry* entry = storage->find(id);

            if (entry == nullptr) {
                Entry::QueueEntry* queue = new Entry::QueueEntry();
                queue->queue(value);
                storage->add(id, queue);
            } else {
                Entry::QueueEntry* queue = static_cast<Entry::QueueEntry*>(entry);
                queue->queue(value);
            }

            return Result::ok();
        }
    };

    Command* queue(std::string scope, std::string key, std::string value)
    {
        return new QueueCommand(Entry::Id{scope, key}, value);
    }
}

namespace Sider::Command
{
    using namespace Sider::Storage;

    class DequeueCommand : public Command
    {
        private:
        const Entry::Id id;

        public:
        DequeueCommand(Entry::Id id) :
            id(id)
        {}

        Result execute(Sider::Storage::Storage* storage) override
        {
            Entry::Entry* entry = storage->find(id);

            if (entry) {
                Entry::QueueEntry* queue = static_cast<Entry::QueueEntry*>(entry);
                queue->dequeue();
            }

            return Result::ok();
        }
    };

    Command* dequeue(std::string scope, std::string key)
    {
        return new DequeueCommand(Entry::Id{scope, key});
    }
}