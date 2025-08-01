#include "lib.h"

void handleclient(int s);
class Server
{
public:
    Server()
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
        if (bind(s, (sockaddr *)&addr, sizeof(addr)) < 0)
        {
            perror("BIND ERROR");
            exit(1);
        }
        std::cout << "BIND SUCCESS" << std::endl;
        if (listen(s, 5) < 0)
        {
            perror("LISTEN ERROR");
            exit(1);
        }
        std::cout << "LISTEN SUCCESS" << std::endl;
        int c = accept(s, NULL, NULL);
        std::thread(handleclient, c).join();
    }
};

int main()
{
    Server *s = new Server();
    delete (s);
    return 0;
}

void handleclient(int s)
{
    req r;
    recv(s,&r,sizeof(r),0);
    if (r.code == 0){
        directory dir;
        DIR *ep = opendir(".");
        dirent *rp = readdir(ep);
        while (rp != NULL)
        {
            strcpy(dir.filename, rp->d_name);
            FILE *fp = fopen(rp->d_name, "r");
            fseek(fp, 0, SEEK_END);
            dir.filesize = ftell(fp);
            fclose(fp);
            send(s, &dir, sizeof(dir), 0);
            rp = readdir(ep);
        }
        closedir(ep);
        delete(rp);
    }
    else if(r.code == 1){
        FILE *fp = fopen(r.msg,"rb");
        if(fp == NULL){
            close(s);
            return;
        }
        fseek(fp,0,SEEK_END);
        r.size = ftell(fp);
        rewind(fp);
        send(s,&r,sizeof(r),0);
        unsigned char buf[1] = {'\0'};
        while(fread(buf,sizeof(unsigned char),1,fp)){
            send(s,buf,sizeof(buf),0);
        }
        fclose(fp);
    }
    else if(r.code == 2){
        FILE *fp = fopen("TEMP","wb");
        unsigned char buf[1] = {'\0'};
        while(recv(s,buf,sizeof(buf),0) != 0){
            fwrite(buf,sizeof(unsigned char),1,fp);
        }
        fclose(fp);
    }
    close(s);
}