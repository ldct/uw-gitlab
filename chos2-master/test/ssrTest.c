#include <syslib.h>
#include <nameServer.h>
#include <ssrTest.h>
#include <bwio.h>

void Sender4(){
    char* sendMsg = "SOS";
    char receiveBuffer[4];
    int tId = 0;
    while(!tId)
        tId = WhoIs("Receiver");
    int i;
    for(i=0;i<1000;i++){
        Send(tId, sendMsg, 4, receiveBuffer, 4);
    }
}

void Receiver4(){
    char buffer[4];
    int tId;

    RegisterAs("Receiver");

    int i;
    for(i=0;i<1000;i++){
        Receive(&tId, buffer, 4);
        Reply(tId, buffer, 4);
    }
}


void Sender64(){
    char* sendMsg = "Ryzen Thread Ripper 64 Core ProcessorRyzen Thread Ripper 64 Core Pr";
    char* receiveBuffer[64];
    RegisterAs("Sender");
    int tId = 0;
    while(!tId)
        tId = WhoIs("Receiver");
    int i;
    for(i=0;i<1000;i++){
        Send(tId, sendMsg, 64, receiveBuffer, 64);
    }
}

void Receiver64(){
    char buffer[64];
    int tId;
    RegisterAs("Receiver");
    int i;
    for(i=0;i<1000;i++){
        Receive(&tId, buffer, 64);
        Reply(tId, buffer, 64);
    }
}


void Sender256(){
    char* sendMsg = "They targeted realtime students.Realtime students.We're a group of people who will sit for hours, days, even weeks on end performing some of the hardest, most mentally demanding tasks. Over, and over, and over all for nothing more than a little piece of paper saying we did.We'll punish our selfs doing things others would consider torture, because we think it's fun.We'll spend most if not all of our free time min maxing the performance of a fake train all to draw out a single extra millimeter of distance per second. Many of us have made careers out of doing just these things: slogging through the grind, all day, the same preprocessor directives over and over, hundreds of times to the point where we know evety little detail such that some have attained such realtime nirvana that they can literally write these kernels blindfolded. Do these people have any idea how many keyboards have been smashed, systems over heated, disks and tracks destroyed 8n frustration? All to latter be referred to as bragging rights? These people honestly think this is a battle they can win? They take our media? We're already building a new one without them. They take our ARM boxes? Realtime students aren't shy about throwing their code else where, or even making the boxes our selves. They think calling us showerless, sleepless, pizza consumers is going to change us? We've been called worse things by prepubescent 10 year olds programming with Java. They picked a fight against a group that's already grown desensitized to their strategies and methods. Who enjoy the battle of attrition they've threatened us with. Who take it as a challange when they tell us we no longer matter. Our obsession with proving we can after being told we can't is so deeply ingrained from years of dealing with no debugging/error messages and compilers laughing at how pathetic our code used to be that proving you people wrong has become a very real need; a honed reflex. Realtime students are competative, hard core, by nature. We love a challange. The worst thing you did in all of this was to challange us. You're not special, you're not original, you're not the first; this is just another poorly documented Marklin Digital train controller interface.";
    char* receiveBuffer[256];
    RegisterAs("Sender");
    int tId = 0;
    while(!tId)
        tId = WhoIs("Receiver");
    int i;
    for(i=0;i<1000;i++){
        Send(tId, sendMsg, 256, receiveBuffer, 256);
    }
}

void Receiver256(){
    char buffer[256];
    int tId;
    RegisterAs("Receiver");
    int i;
    for(i=0;i<1000;i++){
        Receive(&tId, buffer, 256);
        Reply(tId, buffer, 256);
    }
}

void SendReceive4(){
    Create(1, Receiver4);
    Create(1, Sender4);
}

void ReceiveSend4(){
    Create(1, Sender4);
    Create(1, Receiver4);
}

void SendReceive64(){
    Create(1, Receiver64);
    Create(1, Sender64);
}

void ReceiveSend64(){
    Create(1, Sender64);
    Create(1, Receiver64);
}

void SendReceive256(){
    Create(1, Receiver256);
    Create(1, Sender256);
}

void ReceiveSend256(){
    Create(1, Sender256);
    Create(1, Receiver256);
}

void ssr_test_main() {

    Create(10, nameServer);
    Create(1, SendReceive4);

}
