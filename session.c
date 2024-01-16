#include<stdio.h>
#include<stdlib.h>
#include <unistd.h>

void read_file(char *filename, char *buffer)
{
	FILE *f = fopen(filename, "r");
	if (f == NULL)
		return;
	while ((*buffer++ = getc(f)) != EOF) ;
}

int main()
{
	char *session_state_filepath = "/tmp/.session_state";
	char *session_start_filepath = "/tmp/.session_start";
	int seconds = 0;
	char buffer[100] = { 0 };

	for (;;) {
		if (access(session_start_filepath, F_OK) == 0) {
			read_file(session_start_filepath, buffer);
			remove(session_start_filepath);
			seconds = atoi(buffer);
		}

		if (seconds > 0) {
			FILE *f = fopen(session_state_filepath, "w+");
			fprintf(f, "%d %s", seconds, "🔴");
			fclose(f);
			seconds--;
		} else {
			FILE *f = fopen(session_state_filepath, "w+");
			fprintf(f, "✅");
			fclose(f);
		}

		sleep(1);
	}
}
