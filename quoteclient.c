// Usage: quoteclient <server-fifo-filename>
// This program establishes a client-side connection to a server using named pipes (FIFOs).
// It creates its own named pipe, sends its pipe name to the server, and reads and displays the received quote.

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>

#define MAXLEN 1000

int main(int argc, char *argv[]) {	
	if (argc != 2) {
		// Display correct usage if the command-line arguments are not provided
		puts("Usage: quoteclient <server-fifo-name>");
		exit(1);
	}

	// Construct the client's named pipe (FIFO) name using process and user information
    char clientfifo[MAXLEN];
	sprintf(clientfifo, "/tmp/%s-%d", getenv("USER"), getpid());

	// Create the client's named pipe with specified permissions
	mkfifo(clientfifo, 0600);
	chmod(clientfifo, 0622);
	
	// Open the server's named pipe for writing and send the client's named pipe name
	FILE *fp = fopen(argv[1], "w");
	fputs(clientfifo, fp);
	fclose(fp);

	// Open the client's named pipe for reading and read the received quote, then print it on the screen
	FILE *quotefp = fopen(clientfifo, "r");
	// Read the quote from the named pipe

	// Remove the client's named pipe after communication is done
	unlink(clientfifo);
}
