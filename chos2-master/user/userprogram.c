#include <syslib.h>
#include <userprogram.h>
#include <bwio.h>
#include <nameServer.h>
#include <map.h>
#include <scheduler.h>

void receiver() {
    int who;
    char buf[100];
    Receive(&who, buf, 40);
    bwprintf(COM2, "[receiver]\tReceive(=%d, =%s, 40)\r\n", who, buf);
    Reply(who, "Who the fuck are ye?", 40);
}

void sender() {
    char buf[100];
    int ret = Send(2, "hello", 40, buf, 40);
    bwprintf(COM2, "[sender]\tSend(2, hello, 40, =%s, 40)=%d\r\n", buf, ret);
}

void user_main() {
    bwprintf(COM2, "[user_main]\tCreate(-1, receiver)=%d\r\n", Create(-1, receiver));
    bwprintf(COM2, "[user_main]\tCreate(-1, sender)=%d\r\n", Create(-1, sender));
}

void shitTest(){
    Destroy();
}

void FireStrike(){
    int i=0;
    int exp = 0;
    for(i = 0; i<10000; i++){
        int id = Create(2, shitTest);
        if(id>0){
            exp++;
        }
    }
    bwprintf(COM2, "Created %d tasks\r\n", exp);
}

void Registar1(){
    RegisterAs("Register1");
    int status = WhoIs("Register1");
    bwprintf(COM2, "Register1 is %d\r\n", status);

    status = WhoIs("Register2");
    bwprintf(COM2, "Register2 is %d\r\n", status);
}

void Registar2(){
    RegisterAs("Register2");

}

void Registar3(){
    RegisterAs("Register3");

}

void NameServerTest(){
    Create(5, Registar1);
    Create(5, Registar2);
    Create(5, Registar3);

    int status = WhoIs("Register1");
    bwprintf(COM2, "Register1 is %d\r\n", status);
    status = WhoIs("Register2");
    bwprintf(COM2, "Register2 is %d\r\n", status);
    status = WhoIs("Register3");
    bwprintf(COM2, "Register3 is %d\r\n", status);
}

void BaseTest(){
    bwprintf(COM2, "I'm here!\r\n");
    Yield();    
    bwprintf(COM2, "I'm back!\r\n");
    Yield();    
    bwprintf(COM2, "There and back again!\r\n");
}