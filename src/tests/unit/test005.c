#include "test005.h"

#include <stdio.h>

#include "../../lib/Arena.h"
#include "../../lib/Base.h"
#include "../../lib/Base64.h"
#include "../../lib/Sha1.h"

static const char* WS_HANDSHAKE_MAGIC = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";

void Test005__Test() {
  LOG_DEBUGF("Test005 Base64");

  char* key = "piuY/sopTtheQIVA3jM4vQ==";

  // Concatenate the client's Sec-WebSocket-Key with the magic string
  char a[256];
  sprintf(a, "%s%s", key, WS_HANDSHAKE_MAGIC);

  // Compute the SHA-1 hash of this concatenated string
  u8 hash[20];
  sha1(hash, (const u8*)a, strlen(a));

  Arena* arena;
  Arena__Alloc(&arena, 1024);

  // Base64 encode the result of the hash
  char* encoded = base64_encode(arena, (const unsigned char*)hash, 20);

  printf(  // TODO: use mprintf()
      "HTTP/1.1 101 Switching Protocols\r\n"
      "Upgrade: websocket\r\n"
      "Connection: Upgrade\r\n"
      "Sec-WebSocket-Accept: %s\r\n"
      "Sec-WebSocket-Extensions: permessage-deflate\r\n\r\n",
      encoded);
}
