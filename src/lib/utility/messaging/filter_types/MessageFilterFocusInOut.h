#ifndef MESSAGE_FILTER_FOCUS_IN_OUT_H
#define MESSAGE_FILTER_FOCUS_IN_OUT_H

#include "utility/messaging/MessageFilter.h"
#include "utility/messaging/type/MessageFocusIn.h"
#include "utility/messaging/type/MessageFocusOut.h"

class MessageFilterFocusInOut
	: public MessageFilter
{
	void filter(MessageQueue::MessageBufferType* messageBuffer) override
	{
		MessageBase* message = messageBuffer->front().get();
		if (message->getType() == MessageFocusIn::getStaticType())
		{
			for (auto it = messageBuffer->begin() + 1; it != messageBuffer->end(); it++)
			{
				if ((*it)->getType() == MessageFocusOut::getStaticType() &&
					dynamic_cast<MessageFocusIn*>(message)->tokenIds == dynamic_cast<MessageFocusOut*>(it->get())->tokenIds)
				{
					messageBuffer->erase(it);
					messageBuffer->pop_front();
					return;
				}
			}
		}
	}
};

#endif // MESSAGE_FILTER_FOCUS_IN_OUT_H