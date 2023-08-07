#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>

#define MAXQUOTES 10000
#define MAXLEN 1000

// Read motivational quotes from a file, select one randomly, and send it to clients.

char *quotes[MAXQUOTES];
int numQuotes = 0;

int main() {
	char line[MAXLEN];

	FILE *fp = fopen("quotes.txt", "r");
	if (!fp) {
		// Display error message and exit if the quotes file cannot be opened
		puts("quotes.txt cannot be opened for reading.");
		exit(1);
	}
	int i = 0;

	// Read quotes from the file and store them in an array
	while (fgets(line, MAXLEN, fp)) {
		quotes[i] = (char *)malloc(strlen(line) + 1);
		strcpy(quotes[i], line);
		i++;
	}
	numQuotes = i;
	printf("%d quotes were read.\n", numQuotes);

	// Seed the random number generator
	srand(getpid() + time(NULL) + getuid());
	
	// Create a named pipe to receive client requests
	char filename[MAXLEN];
	sprintf(filename, "/tmp/%s-%d", getenv("USER"), getpid());
	mkfifo(filename, 0600);
	chmod(filename, 0622);
	printf("Send your requests to %s\n", filename);

	while (1) {
		FILE *fp = fopen(filename, "r");
		if (!fp) {
			// Display error message and exit if the named pipe cannot be opened for reading
			printf("FIFO %s cannot be opened for reading.\n", filename);
			exit(2);
		}
		printf("Opened %s to read...\n", filename);
		while (fgets(line, MAXLEN, fp)) {
			char *cptr = strchr(line, '\n');
			if (cptr) 
				*cptr = '\0';

			// Open a client-specific named pipe for writing
			FILE *clientfp = fopen(line, "w");
			printf("%s: ", line);
			if (clientfp) {
				// Select a random quote and send it to the client
				int quoteNumber = rand() % numQuotes;
				printf("%s", quotes[quoteNumber]);
				fputs(quotes[quoteNumber], clientfp);
				fclose(clientfp);
			}
			putchar('\n');
		}
		fclose(fp);
	}
}
