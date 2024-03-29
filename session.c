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

char *format_seconds(size_t seconds, char *buffer) {
	size_t hours = 0, minutes = 0;
	while(seconds >= 3600) {
		hours++;
		seconds -= 3600;
	}
	while(seconds >= 60) {
		minutes++;
		seconds -= 60;
	}

	size_t len = 0;
	if (hours > 0)
		len = sprintf(buffer + len, "%ldh ", hours);
	if (minutes > 0)
		len = sprintf(buffer + len, "%ldm ", minutes);
	if (seconds > 0)
		len = sprintf(buffer + len, "%lds ", seconds);

	return buffer;
}

size_t parse_to_seconds(char *str) {
	size_t current_number = 0;
	size_t total_seconds = 0;

	while(*str != '\0') {
		if (*str >= '0' && *str <= '9') {
			current_number = (current_number*10) + (*str - '0');
		} else {
			switch (*str) {
			case 'h':
				total_seconds = current_number * 3600;
				break;
			case 'm':
				total_seconds = current_number * 60;
				break;
			case 's':
				total_seconds = current_number;
				break;
			}
			current_number = 0;
		}
		str++;
	}
	return total_seconds;
}

int main(int argn, char **argv)
{
	char *session_state_filepath = "/tmp/.session_state";
	char *session_start_filepath = "/tmp/.session_start";
	size_t seconds = 0;
	char file_buffer[100] = { 0 };
	char output_buffer[100] = { 0 };

	if (argn == 2) {
		FILE *f = fopen(session_start_filepath, "w+");
		fprintf(f, "%ld", parse_to_seconds(argv[1]));
		fclose(f);
		return 0;
	}

	for (;;) {
		if (access(session_start_filepath, F_OK) == 0) {
			read_file(session_start_filepath, file_buffer);
			remove(session_start_filepath);
			seconds = atoi(file_buffer);
		}

		if (seconds > 0) {
			FILE *f = fopen(session_state_filepath, "w+");
			format_seconds(seconds, output_buffer);
			printf("output: %s\n", output_buffer);
			fprintf(f, "%s %s", output_buffer, "🔴");
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
