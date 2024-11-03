#include "test004.h"

#include "../../lib/Base.h"
#include "../../lib/Net.h"
#include "../../lib/Time.h"

static bool q1 = false, q2 = false;
#define MAX_CLIENTS (1)
static Socket* clients[MAX_CLIENTS];
static u32 client_count = 0;

static void onAccept(Socket* server, Socket* client) {
  LOG_DEBUGF("Server %s:%s accept %s:%s", server->addr, server->port, client->addr, client->port);

  // remember all incoming connections
  clients[client_count++] = client;

  // Send response to client
  char* data = "shello";
  u32 len = strlen(data);
  Net__write(client, len, data);
  LOG_DEBUGF("Server send. len: %u, data: %s", len, data);
}

static void onConnect(Socket* client) {
  LOG_DEBUGF("Client connected to %s:%s", client->addr, client->port);

  // Send response to server
  char* data = "chello";
  u32 len = strlen(data);
  Net__write(client, len, data);
  LOG_DEBUGF("Client send. len: %u, data: %s", len, data);
}

static void ServerPump(Socket* client) {
  // Read data from client
  Net__read(client);
  LOG_DEBUGF("Server recv. len: %u, data: %s", client->buf.len, client->buf.data);

  char* expected = "chello";
  if (strcmp(client->buf.data, expected) == 0) {
    LOG_DEBUGF("server satisfied");
    q2 = true;
  }
}

static void ClientPump(Socket* client) {
  // Read data from server
  Net__read(client);
  LOG_DEBUGF("Client recv. len: %u, data: %s", client->buf.len, client->buf.data);

  char* expected = "shello";
  if (strcmp(client->buf.data, expected) == 0) {
    LOG_DEBUGF("client satisfied");
    q1 = true;
  }
}

void Test004__Test() {
  LOG_DEBUGF("Test004 Network Client + Server");

  Net__init();
  char* port = "9000";

  // Server
  Socket* ssocket = Net__Socket__alloc();
  Net__Socket__init(ssocket, "0.0.0.0", port, SERVER_SOCKET);
  LOG_DEBUGF("Server listen on %s:%s", ssocket->addr, ssocket->port);
  Net__listen(ssocket);

  // Client
  Socket* csocket = Net__Socket__alloc();
  Net__Socket__init(csocket, "127.0.0.1", port, CLIENT_SOCKET);
  LOG_DEBUGF("Client connecting to %s:%s", csocket->addr, csocket->port);
  Net__connect(csocket, onConnect);

  while (!(q1 && q2)) {
    Net__accept(ssocket, onAccept);
    for (u32 i = 0; i < client_count; i++) {
      ServerPump(clients[i]);
    }
    ClientPump(csocket);

    SLEEP(10);
  }

  // Cleanup
  LOG_DEBUGF("close client 1a");
  Net__shutdown(csocket);  // client 1a
  Net__close(csocket);  // client 1a
  Net__free(csocket);

  LOG_DEBUGF("close accepted 1b");
  Net__shutdown(clients[0]);  // client 1b
  Net__close(clients[0]);  // client 1b
  // Net__free(clients[0]);

  LOG_DEBUGF("close server");
  Net__close(ssocket);  // server 1
  Net__free(ssocket);

  LOG_DEBUGF("shutdown");
  Net__destroy();
}
