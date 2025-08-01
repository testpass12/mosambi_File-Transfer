#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<bits/stdc++.h>
#include<dirent.h>
struct directory{
    char filename[256];
    int filesize;
};
struct req{
    int code;
    char msg[256];
    int size;
};