#include "keyValStore.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

#define PORT 5678

void error(const char *msg) {
    perror(msg);
    exit(1);
}

void handle_client(int clientSocket) {
    setbuf(stdout, NULL);

    // Eingabe und Ausgabe in einer Schleife verarbeiten
    while (1) {
        char buffer[BUFFER_SIZE];
        memset(buffer, 0, sizeof(buffer));

        // Daten vom Client empfangen
        ssize_t totalBytesRead = 0;
        ssize_t bytesRead;

        // Solange Daten verfügbar sind, kontinuierlich lesen
        while ((bytesRead = recv(clientSocket, buffer + totalBytesRead, sizeof(buffer) - totalBytesRead - 1, 0)) > 0) {
            totalBytesRead += bytesRead;

            // Prüfen, ob die vollständige Eingabezeile empfangen wurde
            if (buffer[totalBytesRead - 1] == '\n') {
                break;
            }
        }

        if (bytesRead == -1) {
            error("Fehler beim Empfangen der Daten");
        } else if (bytesRead == 0) {
            printf("Verbindung getrennt.\n");
            break;
        }

        // Eingabe vom Client verarbeiten
        printf("Eingabe erhalten: %s", buffer);

        // Prüfen, ob der Befehl "QUIT" ist
        if (strncmp(buffer, "QUIT", 4) == 0) {
            printf("Verbindung beendet.\n");
            break;
        }

        // Befehl, Schlüssel und Wert aus der Eingabe extrahieren
        char command[5];
        char key[256];
        char value[256];
        sscanf(buffer, "%s %s %s", command, key, value);

        // Befehl verarbeiten
        char response[BUFFER_SIZE];
        memset(response, 0, sizeof(response));

        if (strcmp(command, "GET") == 0) {
            int result = get(key, response);
            if (result == 0) {
                strcat(response, "\n");
                printf(">Ausgabe:%s", response);
            } else {
                strcpy(response, ">GET:");
                strcat(response, key);
                strcat(response, ":key_nonexistent\n");
                printf(">Ausgabe:>GET name:%s key_nonexistent\n",key);
            }
        } else if (strcmp(command, "PUT") == 0) {
            int result = put(key, value);
            if (result == 0) {
                strcpy(response, ">PUT: ");
                strcat(response, "name:");
                strcat(response, key);
                strcat(response, " ");
                strcat(response, "value:");
                strcat(response, value);
                strcat(response, "\n");
                printf(">Ausgabe:>PUT name:%s value:%s\n", key, value);
            } else {
                strcpy(response, ">PUT:");
                strcat(response, key);
                strcat(response, ":error\n");
                printf(">Ausgabe:>PUT name:%s error!\n", key);
            }
        } else if (strcmp(command, "DEL") == 0) {
            int result = del(key);
            if (result == 0) {
                strcpy(response, ">DEL: ");
                strcat(response, key);
                strcat(response, ":key_deleted\n");
                printf(">Ausgabe:>DEL: name:%s key_deleted\n", key);
            } else {
                strcpy(response, ">DEL:");
                strcat(response, key);
                strcat(response, ":key_nonexistent\n");
                printf(">Ausgabe:>DEL name:%s key_nonexistent\n", key);
            }
        }

        // Antwort an den Client senden
        ssize_t bytesSent = send(clientSocket, response, strlen(response), 0);
        if (bytesSent == -1) {
            error("Fehler beim Senden der Daten");
        }
    }

    // Socket schließen
    close(clientSocket);
}

void sigchld_handler(int s) {
    while(waitpid(-1, NULL, WNOHANG) > 0);
}

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