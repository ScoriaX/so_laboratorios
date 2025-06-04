/**
 * programa_bidireccional.c
 *
 * Este programa demuestra la comunicación bidireccional entre un proceso padre
 * y su hijo mediante el uso de dos pipes.
 * Piero Poblete
 * Katherine Saico
 * Facundo Sanz
 * Nicolle Lozano
 * El padre envía un mensaje al hijo, el hijo lo convierte a mayúsculas y
 * lo devuelve al padre. El padre muestra el mensaje recibido.
 */


#include <sys/types.h>
#include <wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

int main() {
    pid_t pid;                 // Identificador del proceso hijo
    int pipe_padre_hijo[2];    // Pipe para comunicación padre -> hijo
    int pipe_hijo_padre[2];    // Pipe para comunicación hijo -> padre
    char buffer[100];          // Buffer para almacenar mensajes
    int estado;                // Estado de salida del proceso hijo
   
    // Crear los dos pipes
    if (pipe(pipe_padre_hijo) == -1 || pipe(pipe_hijo_padre) == -1) {
        perror("Error al crear los pipes");
        exit(EXIT_FAILURE);
    }
   
    // Crear proceso hijo
    pid = fork();
   
    if (pid == -1) {
        perror("Error en fork");
        exit(EXIT_FAILURE);
    }
   
    if (pid == 0) {  // Proceso hijo
        // Cerrar extremos no utilizados
        close(pipe_padre_hijo[1]);  // Extremo de escritura del padre al hijo
        close(pipe_hijo_padre[0]);  // Extremo de lectura del hijo al padre
       
        // Leer mensaje del padre
        read(pipe_padre_hijo[0], buffer, sizeof(buffer));
        printf("Hijo: Recibido del padre: \"%s\"\n", buffer);
       
        // Convertir mensaje a mayúsculas
        for (int i = 0; buffer[i]; i++) {
            buffer[i] = toupper(buffer[i]);
        }
       
        printf("Hijo: Enviando al padre: \"%s\"\n", buffer);
       
        // Enviar mensaje modificado al padre
        write(pipe_hijo_padre[1], buffer, strlen(buffer) + 1);
       
        // Cerrar extremos restantes
        close(pipe_padre_hijo[0]);
        close(pipe_hijo_padre[1]);
       
        exit(EXIT_SUCCESS);
    } else {  // Proceso padre
        // Cerrar extremos no utilizados
        close(pipe_padre_hijo[0]);  // Extremo de lectura del padre al hijo
        close(pipe_hijo_padre[1]);  // Extremo de escritura del hijo al padre
       
        // Preparar y enviar mensaje al hijo
        strcpy(buffer, "Hola, este es un mensaje del padre al hijo");
        printf("Padre: Enviando al hijo: \"%s\"\n", buffer);
        write(pipe_padre_hijo[1], buffer, strlen(buffer) + 1);
       
        // Esperar y recibir respuesta del hijo
        read(pipe_hijo_padre[0], buffer, sizeof(buffer));
        printf("Padre: Recibido del hijo: \"%s\"\n", buffer);
       
        // Cerrar extremos restantes
        close(pipe_padre_hijo[1]);
        close(pipe_hijo_padre[0]);
       
        // Esperar que el hijo termine
        wait(&estado);
       
        exit(EXIT_SUCCESS);
    }
   
    return 0;  // Nunca se alcanza, pero evita warnings del compilador
}
