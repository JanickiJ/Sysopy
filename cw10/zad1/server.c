#include <signal.h>
#include <netdb.h>
#include "header.h"
#define MAX_NUM_OF_CLIENTS 5

int port;
char* socket_path;

int unix_socket;
int inet_socket;

pthread_mutex_t clients_data_mutex = PTHREAD_MUTEX_INITIALIZER;

client *clients[MAX_LOGIN_LEN];

void ping();
void start_local_server();
void start_network_server();
void disconnect_client(int i);
void run_server();
void stop_server(int sig_no);
void no_ping_response(int i);

int main(int argc, char** argv) {
    if(argc != 3){
        fprintf(stderr, "Incorrect input\n");
        exit(EXIT_FAILURE);
    }
    signal(SIGTSTP, stop_server);
    signal(SIGINT, stop_server);
    port = atoi(argv[1]);
    socket_path = argv[2];

    start_local_server();
    start_network_server();

    pthread_t ping_thread;
    if(pthread_create(&ping_thread, NULL, (void *(*)(void *)) ping, NULL ) != 0){ print_error("pthread_create",errno);}

    run_server();

    return 0;
}


void start_local_server(){
    struct sockaddr local_socket_addr;
    strcpy(local_socket_addr.sa_data, socket_path);
    local_socket_addr.sa_family = AF_UNIX;

    if((unix_socket = socket(AF_UNIX, SOCK_STREAM, 0)) == -1){ print_error("unix_socket",errno);}
    if(bind(unix_socket, &local_socket_addr, sizeof (local_socket_addr)) != 0){ print_error("bind",errno);}
    if(listen(unix_socket, MAX_NUM_OF_CLIENTS) != 0){ print_error("unix_socket",errno);}
    printf("Unix socket ready fd: %d\n", unix_socket);
}

void start_network_server(){/*
    struct addrinfo inet_socket_addr;
    struct addrinfo* inet_socket_addr_res;
    inet_socket_addr.ai_flags = AI_PASSIVE;
    inet_socket_addr.ai_socktype = SOCK_STREAM;
    inet_socket_addr.ai_flags = AF_INET;
    inet_socket_addr.ai_family = AF_INET;
    if(getaddrinfo(NULL,port,&inet_socket_addr,&inet_socket_addr_res) != 0){ print_error("getaddrinfo",errno);}*/

    struct sockaddr_in inet_socket_addr;
    struct hostent* local_hostent = gethostbyname("localhost");
    struct in_addr address = *((struct in_addr*)local_hostent->h_addr);
    inet_socket_addr.sin_family = AF_INET;
    inet_socket_addr.sin_port = htons(port);
    inet_socket_addr.sin_addr.s_addr = address.s_addr;

    if((inet_socket = socket(AF_INET, SOCK_STREAM,0)) == -1){ print_error("inet_scoket",errno);}
    if((bind(inet_socket, (struct sockaddr *) &inet_socket_addr, sizeof(inet_socket_addr))) != 0){ print_error("bind",errno);}
    if(listen(inet_socket,MAX_NUM_OF_CLIENTS) != 0){ print_error("listen",errno);}
    printf("Inet socket ready on: %s:%d\n", inet_ntoa(address), port);
}

void ping(){
    while (1){
        sleep(10);
        pthread_mutex_lock(&clients_data_mutex);
        for (int i = 0; i < MAX_NUM_OF_CLIENTS; i++) {
            if(clients[i] && clients[i]->is_responding == 0){
                no_ping_response(i);
            }
        }
        for (int i = 0; i < MAX_NUM_OF_CLIENTS; i++) {
            if(clients[i] != NULL && clients[i]->opponent_id!=-1){
                char to_send[MAX_MESSAGE_LEN];
                sprintf(to_send,"%d##", PING);
                if(send(clients[i]->fd, to_send, MAX_MESSAGE_LEN,0) == -1){ print_error("send",errno);}
                clients[i]->is_responding = 0;
            }
        }
        pthread_mutex_unlock(&clients_data_mutex);
    }
}


int accept_connection(int fd){
    if(fd == unix_socket || fd == inet_socket){
        if((fd = accept(fd,NULL,NULL)) == -1){ print_error("accept",errno);}
    }
    return fd;
}


int poll_fd_to_service(){
    pthread_mutex_lock(&clients_data_mutex);
    int connected =0;
    for (int i = 0; i < MAX_NUM_OF_CLIENTS; i++) {
        if(clients[i]!=NULL){
            struct pollfd client_pollfd;
            client_pollfd.fd = clients[i]->fd;
            client_pollfd.events = POLLIN;
            clients[i]->client_pollfd = client_pollfd;
            connected++;
        }
    }
    struct pollfd pollfd_inet;
    struct pollfd pollfd_unix;
    pollfd_inet.fd = inet_socket;
    pollfd_unix.fd = unix_socket;
    pollfd_inet.events = POLLIN;
    pollfd_unix.events = POLLIN;

    struct pollfd pollfd_arr[connected+2];
    pollfd_arr[0] = pollfd_inet;
    pollfd_arr[1] = pollfd_unix;

    int j=2;
    for (int i = 0; i < MAX_NUM_OF_CLIENTS; ++i) {
        if(clients[i]){
            pollfd_arr[j] = clients[i]->client_pollfd;
            j++;
        }
    }
    pthread_mutex_unlock(&clients_data_mutex);
    if(poll(pollfd_arr, 2+connected, -1) == -1){ print_error("poll",errno);}

    for (int i = 0; i < 2+connected; i++) {
        if(pollfd_arr[i].revents & POLLIN){
            return pollfd_arr[i].fd;
        }
    }

    return -1;
}

void disconnect_client(int client_id){
    if(!clients[client_id]){return;}
    printf("Disconnecting %s\n", clients[client_id]->login);
    client* client_to_disconnect = clients[client_id];
    close(client_to_disconnect->fd);
    clients[client_id] = NULL;
}

void disconnect_pair(int client_id){
    if(!clients[client_id]){return;}

    client* client_to_disconnect = clients[client_id];
    char mess[MAX_MESSAGE_LEN];
    sprintf(mess,"%d##",DISCONNECT);
    if(client_to_disconnect->has_opponent){
        if(send(client_to_disconnect->opponent_fd,mess,MAX_MESSAGE_LEN,0) == -1){print_error("send",errno);}
        disconnect_client(client_to_disconnect->opponent_id);
    }
    disconnect_client(client_id);
}

void no_ping_response(int i){
    if(!clients[i]){return;}
    client* client_to_disconnect = clients[i];
    char mess[MAX_MESSAGE_LEN];
    sprintf(mess,"%d##",DISCONNECT);

    if(send(client_to_disconnect->fd,mess,MAX_MESSAGE_LEN,0) == -1){print_error("send",errno);}
    disconnect_pair(i);
}

int get_id(char* name){
    int client_id = -1;
    for (int i = 0; i < MAX_NUM_OF_CLIENTS; i++) {
        if(clients[i]!=NULL && !strcmp(clients[i]->login, name)){
            client_id = i;
        }
    }
    return client_id;

}

void save_client(int fd, char* name){
    int new_id = get_id(name);
    char mess[MAX_MESSAGE_LEN];

    if(new_id != -1){
        printf("Name taken: %s\n", name);
        sprintf(mess,"%d##",NAME_TAKEN);
        if(send(fd,mess,MAX_MESSAGE_LEN,0) == -1){ print_error("send",errno);}
        close(fd);
        return;
    }
    for (int i = 0; i < MAX_NUM_OF_CLIENTS; i++) {
        if(clients[i] == NULL){
            new_id = i;
            break;
        }
    }
    if(new_id == -1){
        printf("Server full: %s\n", name);
        sprintf(mess,"%d##",SERVER_FULL);
        if(send(fd,mess,MAX_MESSAGE_LEN,0) == -1){ print_error("send",errno);}
        return;
    }
    printf("Trying to connect: %s\n", name);
    client* new_client = malloc(sizeof(client));
    sprintf(new_client->login, "%s",name);
    new_client->fd = fd;
    new_client->is_responding = 1;
    new_client->opponent_fd = -1;
    new_client->opponent_id = -1;
    new_client->has_opponent = 0;
    for (int i = 0; i < MAX_NUM_OF_CLIENTS; i++) {
        if(i!=new_id && clients[i] && clients[i]->opponent_id == -1){
            clients[i]->opponent_id = new_id;
            clients[i]->opponent_fd = fd;
            clients[i]->has_opponent = 1;
            new_client->opponent_fd = clients[i]->fd;
            new_client->opponent_id = i;
            new_client->has_opponent= 1;
            break;
        }
    }
    clients[new_id] = new_client;

    if(new_client->opponent_id != -1){
        printf("Sending opponent: %s\n", name);
        sprintf(mess,"%d#X#%s", SAVE_RESPONSE, clients[new_client->opponent_id]->login);
        if(send(fd, mess,MAX_MESSAGE_LEN,0) == -1){ print_error("send",errno);}
        sprintf(mess,"%d#O#%s", SAVE_RESPONSE,new_client->login);
        if(send(new_client->opponent_fd, mess,MAX_MESSAGE_LEN,0) == -1){ print_error("send",errno);}
        sprintf(mess,"%d# #", MOVE);
        if(send(new_client->opponent_fd, mess,MAX_MESSAGE_LEN,0) == -1){ print_error("send",errno);}
    }
    printf("End of save: %s\n", name);
}

void accept_ping(int client_id){
    if(client_id != -1){
        clients[client_id]->is_responding = 1;
    }
}

void make_mark(int id, char* args){
    int field_num = atoi(args);
    char mess[MAX_MESSAGE_LEN];
    sprintf(mess,"%d#%d#", MOVE, field_num);
    if(send(clients[id]->opponent_fd,mess,MAX_MESSAGE_LEN,0) == -1){ print_error("send",errno);}
}

void end_game(int id,msg msg_type, int field){
    char mess[MAX_MESSAGE_LEN];
    if(msg_type == WIN){
        sprintf(mess,"%d#%d#", LOOSE,field);
    } else{
        sprintf(mess,"%d#%d#", DRAW,field);
    }
    if(send(clients[id]->opponent_fd,mess,MAX_MESSAGE_LEN,0) == -1){ print_error("send",errno);}
    disconnect_client(clients[id]->opponent_id);
    disconnect_client(id);
}

void run_server(){
    while (1){
        int fd_to_service = poll_fd_to_service();
        fd_to_service = accept_connection(fd_to_service);
        char message[MAX_MESSAGE_LEN];
        if(recv(fd_to_service,message,MAX_MESSAGE_LEN,0) == -1) { print_error("recv",errno);}
        msg msq_type = atoi(strtok (message,"#"));
        char* args = strtok (NULL,"#");
        char* name = strtok (NULL,"#");

        pthread_mutex_lock(&clients_data_mutex);
        int client_id = get_id(name);
        switch (msq_type) {
            case CONNECT:
                save_client(fd_to_service,name);
                break;
            case PING:
                accept_ping(client_id);
                break;
            case DISCONNECT:
                disconnect_pair(client_id);
                break;
            case MAKE_MARK:
                make_mark(client_id,args);
                break;
            case WIN:
                end_game(client_id,WIN, atoi(args));
                break;
            case DRAW:
                end_game(client_id,DRAW, atoi(args));
                break;
            default:
                break;
        }
    pthread_mutex_unlock(&clients_data_mutex);

    }
}

void stop_server(int sig_no){
    pthread_mutex_lock(&clients_data_mutex);
    for (int i = 0; i < MAX_NUM_OF_CLIENTS; i++) {
        if(clients[i] != NULL){
            disconnect_client(i);
        }
    }
    pthread_mutex_unlock(&clients_data_mutex);
    shutdown(unix_socket, SHUT_RDWR);
    unlink(socket_path);
    shutdown(inet_socket, SHUT_RDWR);
    close(inet_socket);
    close(inet_socket);
    close(unix_socket);
    exit(EXIT_SUCCESS);
}