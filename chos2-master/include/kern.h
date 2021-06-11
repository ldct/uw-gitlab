#ifndef KERN_H
#define KERN_H


//Exiting kernel and jump
void exitKernel(void* usrsp);

//assumes that we are resuming execution
//assumes svc mode
void enterKernel();

#endif
