#include "keyValStore.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#define PORT 5678

void error(const char *msg) {
    perror(msg);
    exit(1);
}

int main() {
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
    if (listen(serverSocket, 1) == -1) {
        error("Fehler beim Lauschen auf dem Socket");
    }

    printf("Server gestartet. Warte auf eingehende Verbindungen...\n");

    // Verbindung akzeptieren
    int clientSocket = accept(serverSocket, NULL, NULL);
    if (clientSocket == -1) {
        error("Fehler beim Akzeptieren der Verbindung");
    }

    printf("Verbindung hergestellt. Warte auf Eingaben...\n");

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
            } else {
                strcpy(response, ">GET:");
                strcat(response, key);
                strcat(response, ":key_nonexistent\n");
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
            } else {
                strcpy(response, ">PUT:");
                strcat(response, key);
                strcat(response, ":error\n");
            }
        } else if (strcmp(command, "DEL") == 0) {
            int result = del(key);
            if (result == 0) {
                strcpy(response, ">DEL: ");
                strcat(response, key);
                strcat(response, ":key_deleted\n");
            } else {
                strcpy(response, ">DEL:");
                strcat(response, key);
                strcat(response, ":key_nonexistent\n");
            }
        }

        // Antwort an den Client senden
        ssize_t bytesSent = send(clientSocket, response, strlen(response), 0);
        if (bytesSent == -1) {
            error("Fehler beim Senden der Daten");
        }
    }

    // Sockets schließen
    close(clientSocket);
    close(serverSocket);

    return 0;
}