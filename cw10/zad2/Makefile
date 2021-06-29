CC = gcc -std="c11" -O2 -D_XOPEN_SOURCE=700 -D_GNU_SOURCE

SERVER_FILE = server.c
SERVER_OUT = server

CLIENT_FILE = client.c
CLIENT_OUT = client

TEST_SOCKET_PATH =__socket
TEST_SERVER_IP =127.0.0.1:6661
all: main test
main: clean
	$(CC) -D_NDEBUG $(SERVER_FILE)  -o $(SERVER_OUT) -lpthread
	$(CC) -D_NDEBUG $(CLIENT_FILE)  -o $(CLIENT_OUT) -lpthread

debug:
	$(CC) -fsanitize=address -Wall -Wextra -Wshadow -pedantic $(SERVER_FILE) -o $(SERVER_OUT) -lpthread
	$(CC) -fsanitize=address -Wall -Wextra -Wshadow -pedantic $(CLIENT_FILE) -o $(CLIENT_OUT) -lpthread

debug_fast:
	$(CC) -Wall -Wextra -Wshadow -pedantic $(SERVER_FILE) -o $(SERVER_OUT) -lpthread
	$(CC) -Wall -Wextra -Wshadow -pedantic $(CLIENT_FILE) -o $(CLIENT_OUT) -lpthread

test: main
	./$(SERVER_OUT) 6661 $(TEST_SOCKET_PATH)

local_client1:
	./$(CLIENT_OUT) name1 local $(TEST_SOCKET_PATH)

local_client2:
	./$(CLIENT_OUT) name2 local $(TEST_SOCKET_PATH)

local_client3:
	./$(CLIENT_OUT) name3 local $(TEST_SOCKET_PATH)

server_client1:
	./$(CLIENT_OUT) name3 network $(TEST_SERVER_IP)

server_client2:
	./$(CLIENT_OUT) name4 network $(TEST_SERVER_IP)

clean:
	rm -f $(SERVER_OUT) $(CLIENT_OUT) $(TEST_SOCKET_PATH) *.o *.out name*