#include <scheduler.h>
#include <sendReceiveReply.h>
#include <bwio.h>
#include <panic.h>

extern Scheduler* scheduler;

void initializeCOMM(COMM* com) {
    int i=0;
    initializeQueue("senderFQ", &(com->senderFQ));
    initializeQueue("receiverFQ", &(com->receiverFQ));
    for (i=0; i<MAX_SENDER; i++) {
        push(&(com->senderFQ), com->sendPool + i);
    }
    for (i=0; i<MAX_RECEIVER; i++) {
        push(&(com->receiverFQ), com->receivePool + i);
    }
    initializeMap(&(com->senderRequestTable));
    initializeMap(&(com->receiverTable));
    initializeMap(&(com->senderReplyTable));
}


int min(int a, int b) {
    return a<b?a:b;
}

int SendMsg(COMM* com, Sender* sender, Receiver* receiver) {
    ASSERT(sender->sourceLength <= receiver->receiveLength, "receiver will not receive everything");
    int length = min(sender->sourceLength, receiver->receiveLength);
    int i;
    for(i=0;i<length;i++){
        receiver->receiveBuffer[i] = sender->source[i];
    }
    //Block for cleanup
    //Note on this operation: For this to be true, crucial that we have stable order
    removeMap(&(com->senderRequestTable), receiver->tId);
    insertMap(&(com->senderReplyTable), sender->tId, sender);
    removeMap(&(com->receiverTable), receiver->tId);

    push(&(com->receiverFQ), receiver);


    Task* task = getTask(scheduler, receiver->tId);

    //stores in R1, R2, keeping R0 safe so we can just dereference it
    task->stackEntry[1] = i;
    task->stackEntry[2] = sender->tId;
    if (task->status == BLOCKED) {
        insertTaskToQueue(scheduler, task);
    }
    return i;
}

int replyMsg(COMM* com, const char* reply, int length, Sender* sender) {
    int mlen = min(length, sender->receiveLength);
    int i;
    for(i=0;i<mlen;i++){
        sender->receiveBuffer[i] = reply[i];
    }
    //Clean up
    removeMap(&(com->senderReplyTable), sender->tId);
    // free the object
    push(&(com->senderFQ), sender);
    Task* task = getTask(scheduler, sender->tId);
    task->stackEntry[1] = i;
    insertTaskToQueue(scheduler, task);

    return i;
}

int processSender(COMM* com, Sender* sender) {
    Receiver* target = getMap(&(com->receiverTable), sender->requestTId);
    if(!target){
        Task* receiverTask = getTask(scheduler, sender->requestTId);
        if (receiverTask && receiverTask->status != EXITED) {
            // receiver task exists
            insertMap(&(com->senderRequestTable), sender->requestTId, sender);
            return 0;
        } else {
            // receiver task doesn't exist
            push(&(com->senderFQ), sender);
            return -1;
        }
    } else {
        return SendMsg(com, sender, target);
    }
    return 0;
}

void processReceiver(COMM* com, Receiver* receiver) {
    Sender* sender = getMap(&(com->senderRequestTable), receiver->tId);
    if (sender) {
        SendMsg(com, sender, receiver);
    } else {
        //blocks untill something wakes it up
        scheduler->currentTask->status = BLOCKED;
        insertMap(&(com->receiverTable), receiver->tId, receiver);
    }
}

int insertSender(COMM* com, int tId, int requestTId, const char* source, int length, char* receive, int rlength) {
    Sender* sender = pop(&(com->senderFQ));
    if (!sender)
        return -1;
    sender->tId = tId;
    sender->requestTId = requestTId;
    sender->source = (char*) source;
    sender->sourceLength = length;
    sender->receiveBuffer = receive;
    sender->receiveLength = rlength;
    return processSender(com, sender);
}

void insertReceiver(COMM* com, int tId, char* receive, int length) {
    Receiver* receiver = pop(&(com->receiverFQ));
    ASSERT(receiver, "receiverFQ ran out");
    receiver->tId = tId;
    receiver->receiveBuffer = receive;
    receiver->receiveLength = length;
    processReceiver(com, receiver);
}

int reply(COMM* com, const char* reply, int length, int tId) {
    Task* task = getTask(scheduler, tId);
    if(!task){
        return -1;
    }
    Sender* sender = getMap(&(com->senderReplyTable), tId);
    if(!sender){
        return -2;
    }
    return replyMsg(com, reply, length, sender);
}
