#include <stdio.h>
#include <stdlib.h>
#include <udpsocketlib.h>
#include <string.h>

#define PORT 25000

int main(int argc, char *argv[])
{
    printf("Avvio server asta telematica.\n");
    int sockid = create_udp_server("0.0.0.0", PORT);
    
    if (sockid > -1) {
        printf("Creato un server UDP con socket identifier: %d.\n", sockid);
    } else {
        printf("Errore durante la creazione del server.\n");
    }
}

void error_handler (const char *message) {
    printf("ERRORE: %s\n", message);
}
