#include <stdexcept>
#include <string>

#include "Ttl.h"

namespace Sider::Registry::Register
{
    TtlItems::~TtlItems() = default;

    struct ListNode
    {
        private:
        const Expiration* expiration;
        ListNode* next;
        ListNode* prev;

        public:
        ListNode(const Expiration* expiration = nullptr) :
            expiration(expiration),
            next(nullptr),
            prev(nullptr)
        {
            next = prev = this;
        }

        ~ListNode()
        {
            if (expiration) {
                delete expiration;
            }
        }

        void add(const Ttl* ttl)
        {
            ListNode* newNode = new ListNode(Expiration::withTtl(ttl));
            ListNode* curr = this->next;

            while (newNode->shouldComeAfter(curr)) {
                curr = curr->next;
            }

            newNode->next = curr;
            newNode->prev = curr->prev;
            newNode->prev->next = newNode;
            curr->prev = newNode;

        }

        uint32_t clearExpired()
        {
            uint32_t count = 0;
            ListNode* curr = this->prev;

            while (curr->shouldBeRemoved()) {
                ListNode* toDelete = curr;

                toDelete->prev->next = toDelete->next;
                toDelete->next->prev = toDelete->prev;

                curr = toDelete->prev;
                delete toDelete;
                count++;
            }

            return count;
        }

        void deleteAll()
        {
            ListNode* curr = next;
            while (curr->expiration) {
                ListNode* toDelete = curr;
                curr = curr->next;
                delete toDelete;
            }

            delete this;
        }

        bool haveAllExpired() const
        {
            if (!next->expiration) {
                return false;
            }

            return next->expiration->isPassed();
        }

        private:
        bool shouldComeAfter(ListNode* that) const
        {
            return that->expiration && expiration->isEarlierThan(that->expiration);
        }

        bool shouldBeRemoved() const
        {
            return expiration && expiration->isPassed();
        }
    };

    struct ExpirableList : public TtlItems
    {
        ListNode* list;
        uint32_t calculatedValue;

        ExpirableList() :
            list(new ListNode()),
            calculatedValue(0)
        {}

        ~ExpirableList() override
        {
            list->deleteAll();
        }

        void add(const Ttl* ttl) override
        {
            list->add(ttl);

            calculatedValue++;
        }

        bool isExpired() override
        {
            return list->haveAllExpired();
        }

        uint32_t currentValue() override
        {
            calculatedValue -= list->clearExpired();

            return calculatedValue;
        }
    };

    TtlItems* createTtlItems()
    {
        return new ExpirableList();
    }
}