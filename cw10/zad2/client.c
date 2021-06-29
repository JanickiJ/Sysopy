//
// Created by ja on 5/30/21.
//

#include "header.h"

void disconnect_client(int sig_no);
void stop_client();
void connect_to_server(char* mode, char* server_ip);
void client_run();
void ping();

char mark;
int server_socket;
char* login;
char board[9];

int main(int argc, char** argv){
    if(argc != 4){
        fprintf(stderr,"%s", "Incorrect arguments");
        exit(EXIT_FAILURE);
    }
    signal(SIGTSTP, disconnect_client);
    signal(SIGINT, disconnect_client);
    login = argv[1];
    char* mode = argv[2];
    char* server_ip = argv[3];
    for (int i = 0; i < 9; ++i) sprintf(&board[i],"%d",i+1);
    pthread_t ping_thread;
    connect_to_server(mode,server_ip);
    if(pthread_create(&ping_thread, NULL, (void *(*)(void *)) client_run, NULL ) != 0){ print_error("pthread_create",errno);}
    client_run();
}

void stop_client(){
    printf("TURNING OFF\n");
    sleep(2);
    unlink(login);
    close(server_socket);
    exit(EXIT_SUCCESS);
}

void disconnect_client(int sig_no){
    char mess[MAX_MESSAGE_LEN];
    sprintf(mess,"%d# #%s", DISCONNECT, login);
    if(send(server_socket,mess,MAX_MESSAGE_LEN,0) == -1){ print_error("send",errno);}
    stop_client();
}

void connect_to_server(char* mode, char*server_ip){
    if(!strcmp(mode,"local")){
        struct sockaddr_un sock_addr;
        strcpy(sock_addr.sun_path, server_ip);
        sock_addr.sun_family = AF_UNIX;
        if((server_socket = socket(AF_UNIX, SOCK_DGRAM, 0)) == -1){ print_error("socket",errno);}

        struct sockaddr_un client_socket;
        client_socket.sun_family = AF_UNIX;
        strcpy(client_socket.sun_path, login);
        if(bind(server_socket, (struct sockaddr*) &client_socket, sizeof(client_socket)) <0){ print_error("bind", errno);}

        if(connect(server_socket, (const struct sockaddr *) &sock_addr, sizeof(sock_addr)) == -1){ print_error("connect",errno);}
        printf("Client connected local\n");
    }
    else if(!strcmp(mode,"network")){
        char *colon_pointer = strchr(server_ip, ':');
        int port = atoi(colon_pointer + 1);
        *colon_pointer = '\0';
        struct sockaddr_in addr;
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);
        addr.sin_addr.s_addr = inet_addr(server_ip);
        server_socket = socket(AF_INET, SOCK_DGRAM, 0);

        struct sockaddr_in client_socket;
        client_socket.sin_family = AF_INET;
        client_socket.sin_port = 0;
        client_socket.sin_addr.s_addr = inet_addr(server_ip);
        if(bind(server_socket, (struct sockaddr*) &client_socket, sizeof(client_socket)) < 0){ print_error("bind", errno);}

        if(connect(server_socket, (struct sockaddr*) &addr, sizeof(addr)) < 0){ print_error("send",errno);}
        puts("Client connected network\n");
    }
    else{
        fprintf(stderr,"Incorrect mode\n");
        exit(EXIT_FAILURE);
    }
    char message[MAX_MESSAGE_LEN];
    sprintf(message, "%d# #%s", CONNECT, login);
    if(send(server_socket,message,MAX_MESSAGE_LEN,0) == -1){ print_error("send",errno);}
    printf("Waiting for opponent\n");
    if(recv(server_socket,&message,MAX_MESSAGE_LEN,0) == -1){ print_error("recv",errno);}
    msg msq_type = atoi(strtok (message,"#"));
    char* args = strtok (NULL,"#");
    char* name = strtok (NULL,"#");

    if(msq_type == SERVER_FULL){
        printf("Server is full\n");
        stop_client();
    }
    else if(msq_type == NAME_TAKEN){
        printf("Name is taken\n");
        stop_client();
    }
    else if(msq_type == SAVE_RESPONSE){
        printf("Connected successfully as %s with %s\n", args,name);
        mark = args[0];
    }
}

void print_board(){
    printf(" ===== ===== ===== \n");
    for(int i = 0; i < 9; i++){
        printf("|  %c  ", board[i]);
        if((i + 1) % 3 == 0) {
            printf("\n");
        }
    }
    printf(" ===== ===== ===== \n");
}


void ping(){
    printf("ping\n");
    char mess[MAX_MESSAGE_LEN];
    sprintf(mess,"%d# #%s", PING, login);
    if(send(server_socket,mess,MAX_MESSAGE_LEN,0) == -1){ print_error("send",errno);}
}

void check_win(int choose){
    int marks = 0;
    int counter =0;
    char winner = ' ';

    for(int i = 0; i < 9; i++){
        if(board[i] =='X') counter++;
        else if(board[i] =='O') counter--;
        if(counter == 3) winner = 'X';
        if(counter == -3) winner = 'O';
        if(i % 3 == 2) counter = 0;
        if(board[i-1] != i +'0') marks++;
    }
    for(int i = 0; i < 3; i++){
        for(int j = 0; j < 3; j++){
            if(board[i + j*3] == 'X') counter++;
            else if(board[i + j*3] =='O') counter--;
            if(counter == 3) winner = 'X';
            if(counter == -3) winner = 'O';
        }
        counter = 0;
    }
    if(board[0] == board[4] && board[4] == board[8] && board[4] != '5') winner = board[4];
    if(board[2] == board[4] && board[4] == board[6] && board[4] != '5') winner =  board[4];

    if(winner == mark) {
        printf("You win\nEnd of game\n");
        char mess[MAX_MESSAGE_LEN];
        sprintf(mess,"%d#%d#%s", WIN,choose, login);
        if(send(server_socket,mess,MAX_MESSAGE_LEN,0) == -1){ print_error("send",errno);}
        stop_client();
    }
    if(marks == 9){
        printf("Draw\nEnd of game\n");
        char mess[MAX_MESSAGE_LEN];
        sprintf(mess,"%d#%d#%s", DRAW,choose, login);
        if(send(server_socket,mess,MAX_MESSAGE_LEN,0) == -1){ print_error("send",errno);}
        stop_client();
    }
}

void move(){
    int correct_move = 0;
    int choose;
    while (!correct_move){
        printf("Choose field\n");
        if(scanf("%d",&choose) == EOF){ print_error("scanf",errno);};
        if(board[choose-1] != choose + '0' || choose >9) printf("Incorrect input\n");
        else correct_move++;
    }
    board[choose-1] = mark;
    print_board();
    check_win(choose);
    char mess[MAX_MESSAGE_LEN];
    sprintf(mess,"%d#%d#%s", MAKE_MARK,choose, login);
    if(send(server_socket,mess,MAX_MESSAGE_LEN,0) == -1){ print_error("send",errno);}
}

void end_game(msg type){
    if(type == DRAW){printf("Its draw\nEnd of game\n");}
    if(type == LOOSE){ printf("You loose\nEnd of game\n");}
    stop_client();
}

void client_run(){
    char message[MAX_MESSAGE_LEN];
    while (1) {
        if (recv(server_socket, &message, MAX_MESSAGE_LEN, 0) == -1) { print_error("recv",errno); }
        msg msq_type = atoi(strtok (message,"#"));
        char* args = strtok (NULL,"#");
        char* name = strtok (NULL,"#");
        switch (msq_type) {
            case DISCONNECT:
                stop_client();
                break;
            case PING:
                ping();
                break;
            case MOVE:
                if(strcmp(args," ")){
                    board[atoi(args)-1] = mark == 'O' ? 'X' : 'O';
                }
                print_board();
                move();
                break;
            case LOOSE:
                if(strcmp(args," ")){
                    board[atoi(args)-1] = mark == 'O' ? 'X' : 'O';
                }
                print_board();
                end_game(msq_type);
                break;
            case DRAW:
                if(strcmp(args," ")){
                    board[atoi(args)-1] = mark == 'O' ? 'X' : 'O';
                }
                print_board();
                end_game(msq_type);
                break;
        }
    }
}

