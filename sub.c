#include "sub.h"

#define BUFFER_SIZE 1024

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
