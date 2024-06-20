#include "main.h"

#define PORT 5678


int main() {
    initialize_shared_memory();

    // Socket erstellen
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        error("Fehler beim Erstellen des Sockets");
    }

    // Serveradresse konfigurieren
    struct sockaddr_in serverAddress;
    memset(&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(PORT);

    // Socket an die Serveradresse binden
    if (bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1) {
        error("Fehler beim Binden des Sockets");
    }

    // Socket für eingehende Verbindungen abhören
    if (listen(serverSocket, 10) == -1) { // Erhöht die Länge der Verbindungswarteschlange
        error("Fehler beim Lauschen auf dem Socket");
    }

    printf("Server gestartet. Port ist %d \n",PORT);
    printf("Warte auf eingehende Verbindungen...\n");

    // Signalhandler für beendete Kindprozesse einrichten
    struct sigaction sa;
    sa.sa_handler = sigchld_handler; // Reaper für beendete Kindprozesse
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        error("Fehler beim Einrichten des Signalhandlers");
    }

    // Verbindungen akzeptieren
    while (1) {
        int clientSocket = accept(serverSocket, NULL, NULL);
        if (clientSocket == -1) {
            error("Fehler beim Akzeptieren der Verbindung");
        }

        if (!fork()) {
            // Kindprozess
            close(serverSocket); // Der Kindprozess benötigt keinen Zugriff auf den Serversocket
            handle_client(clientSocket);
            close(clientSocket);
            detach_shared_memory();
            exit(0);
        }

        // Elternprozess
        close(clientSocket); // Der Elternprozess benötigt keinen Zugriff auf den Clientsocket
    }

    // Sockets schließen
    close(serverSocket);
    detach_shared_memory();
    cleanup_shared_memory();

    return 0;
}
