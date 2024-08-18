#include <MessageManager.hpp>


MessageManager messageManager;


void MessageManager::InsertMessages(std::vector<MessageBaseType*>& messages){
    if(!messages.empty()){
        mtxMessageQueue.lock();
        messageQueue.insert(messageQueue.end(), std::make_move_iterator(messages.begin()), std::make_move_iterator(messages.end()));
        mtxMessageQueue.unlock();
        messages.clear();
    }
}

std::vector<MessageBaseType*> MessageManager::FetchMessages(void){
    std::vector<MessageBaseType*> result;
    mtxMessageQueue.lock();
    result.swap(messageQueue);
    mtxMessageQueue.unlock();
    return result;
}

