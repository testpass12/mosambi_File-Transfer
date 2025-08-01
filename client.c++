#include "lib.h"

class Client
{
    void handle(int s);

public:
    Client()
    {
        sockaddr_in addr;
        addr.sin_addr.s_addr = inet_addr("127.0.0.1");
        addr.sin_family = AF_INET;
        addr.sin_port = htons(8080);
        int s = socket(AF_INET, SOCK_STREAM, 0);
        if (s < 0)
        {
            perror("SOCKET ERROR");
            exit(1);
        }
        std::cout << "SOCKET SUCCESS" << std::endl;
        if (connect(s, (sockaddr *)&addr, sizeof(addr)) < 0)
        {
            perror("CONNECT ERROR");
            exit(1);
        }
        std::cout << "CONNECT SUCCESS" << std::endl;
        handle(s);
    }
};

int main()
{
    Client *s = new Client();
    delete (s);
    return 0;
}

void Client::handle(int s)
{
    req r;
    r.code = 2;
    strcpy(r.msg,"client");
    send(s, &r, sizeof(r), 0);
    if (r.code == 0)
    {
        directory dir;
        while (recv(s, &dir, sizeof(dir), 0) != 0)
        {
            std::cout << dir.filename << "\t\t" << dir.filesize << std::endl;
            bzero(dir.filename, sizeof(dir.filename));
        }
    }
    else if(r.code == 1){
        recv(s,&r,sizeof(r),0);
        FILE *fp = fopen("Temp","wb");
        unsigned char buf[1] = {'\0'};
        while (recv(s, buf, sizeof(buf), 0) != 0)
        {
            fwrite(buf,sizeof(unsigned char),1,fp);
        }
        fclose(fp);
        std::cout << r.size << std::endl;
    }
    else if(r.code == 2){
        FILE *fp = fopen(r.msg,"rb");
        if(fp == NULL){
            close(s);
            return;
        }
        unsigned char buf[1] = {'\0'};
        while(fread(buf,sizeof(unsigned char),1,fp)){
            send(s,buf,sizeof(buf),0);
        }
        fclose(fp);
    }
    close(s);
}