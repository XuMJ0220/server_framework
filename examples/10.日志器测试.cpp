#include "log.h"

#include <unistd.h>
#include <sys/syscall.h>
#include <iostream>

int main(int argc,char** argv){

    LOG_DEBUG("root","HAHA");
    LOG_INFO("xumj","HEHE");

    return 0;
}