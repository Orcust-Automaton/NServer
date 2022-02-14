#include <sys/socket.h>
#include <sys/un.h>
#include <arpa/inet.h>
#include<sys/sendfile.h>
#include <unistd.h>
#include <string.h>
#include <strings.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

#define PORT 8080
#define SERVER "Server: NiyahServer"
#define TARGE "%s\r\n%s\r\n%s\r\n\r\n"

#define OK "HTTP/1.0 200 OK"
#define NO_FOUND "HTTP/1.0 404 NOT FOUND"
#define NOT_IMPLEMENTED "HTTP/1.0 501 NOT IMPLEMENTED"

#define ICO "Content-Type: image/webp"
#define PNG "Content-Type: image/png"
#define JPG "Content-Type: image/jpg"
#define HTML "Content-Type: text/html;charset=utf-8"
#define CSS "Content-Type: text/css;charset=utf-8"

struct File_name{
    char *name;
    char *suffix;
};

void send404 (int client_socket)
{
    char *response = malloc(0x500);
    char *body = "<html><head><title>Niyah</title></head><body><h2>寄</h2><p>404 not found</p></body></html>";

    snprintf(response , 0x500 , TARGE , NO_FOUND , SERVER, HTML);
    send( client_socket, response ,strlen(response),0 );
    send( client_socket, body ,strlen(body),0 );
}

void send501 (int client_socket)
{
    char *response = malloc(0x500);
    char *body = "<html><head><title>Niyah</title></head><body><h2>Hacker?</h2><p>go out now!</p></body></html>";

    snprintf(response , 0x500 , TARGE , NO_FOUND , SERVER, HTML);
    send( client_socket, response ,strlen(response),0 );
    send( client_socket, body ,strlen(body),0 );
}

void send200 (int client_socket , char *type ,char *file_name)
{   
    int fd = open(file_name , O_RDONLY);
    if(fd == -1)
    {
        send404(client_socket);
        return;
    }
    char *response = malloc(0x500 );
    snprintf(response , 0x500 , TARGE , OK , SERVER , type );

    send( client_socket , response , strlen(response),0);
    sendfile(client_socket,fd,NULL,0xffffffffffffff);
}

struct File_name *getfile(char *file_name)
{
    if(!strchr(file_name , '.'))
    {
        return NULL;
    }
    char *suffix = strchr(file_name , '.') + 1;

    struct File_name *file = (struct File_name *) malloc(0X18);

    file->name = malloc(strlen(file_name));
    file->suffix = malloc(strlen(suffix));

    strcpy(file->name , file_name + 1);
    strcpy(file->suffix , suffix);

    return file;
}

void analysis( int client_socket ,  char *buf)
{
    char *file_name = malloc(0x500);
    char *tmp ;
    char *data ;
    char *type ;
    memset(file_name , 0 ,0x500);
    if(!strncasecmp( buf , "GET" , 3 ))
    {
        tmp = buf + 4;
    }else if (!strncasecmp( buf , "POST" , 4 ))
    {
        tmp = buf + 5;
    }else{
        send501( client_socket );
        return;
    }

    data = strchr(tmp , ' ');
    memcpy(file_name , tmp , data - tmp);

    if(!strcmp(file_name , "/"))
    {
        send200( client_socket , HTML ,"index.html" );
        return ;
    }

    struct File_name *file = getfile(file_name);
    if(! file)
    {
        send404( client_socket );
        return;
    }
    if(!strcmp(file->suffix , "html"))
    {
        type = HTML;
    }else if(!strcmp(file->suffix , "png"))
    {
        type = PNG;
    }else if(!strcmp(file->suffix , "jpg"))
    {
        type = JPG;
    }else if(!strcmp(file->suffix , "ico"))
    {
        type = ICO;
    }else if(!strcmp(file->suffix , "css"))
    {
        type = CSS;
    }else{
        send404( client_socket );
        return;
    }

    puts(file->name);
    puts(file->suffix);
    send200( client_socket , type ,file->name );
}

int main(){
    
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    int bind_res = 0;
    int listen_res = 0;

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(PORT);

    do{
        sleep(1);
        bind_res = bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr));
    }while(bind_res == -1);
    puts("bind");

    do{
        sleep(1);
        listen_res = listen(server_socket, 5);
    }while(listen_res == -1);
    puts("listen");

    while(1)
    {
        int client_socket = accept(server_socket, NULL, NULL);

        char buf[1024];
        read(client_socket, buf, 1024);
        analysis(client_socket , buf);
        close( client_socket );
    }
    close( server_socket );

    return 0;
}
