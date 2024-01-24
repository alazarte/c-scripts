#include<stdio.h>
#include<stdbool.h>
#include<stdlib.h>
#include <curl/curl.h>

// assumes to reach a '/' backwards
// http://example.com/file.txt, the filename is file.txt
void copy_filename_from_url(char *to, char *from_url)
{
	while (*from_url++ != '\0') ;
	while (*from_url-- != '/') ;
	from_url += 2;
	while (*from_url != '\0') {
		*to++ = *from_url++;
	}
}

// Callback function to write data to a file
size_t write_data(void *ptr, size_t size, size_t nmemb, FILE *stream)
{
	return fwrite(ptr, size, nmemb, stream);
}

char try_download_file(char *url, char *output_file)
{
	CURL *curl;
	FILE *fp;
	CURLcode res;

	curl_global_init(CURL_GLOBAL_DEFAULT);
	curl = curl_easy_init();

	if (!curl) {
		fprintf(stderr, "Failed to initialize libcurl\n");
		return 0;
	}
	// Set the URL to download
	curl_easy_setopt(curl, CURLOPT_URL, url);

	// Set the callback function to write data to a file
	fp = fopen(output_file, "wb");
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);

	// Perform the HTTP request
	res = curl_easy_perform(curl);

	// Clean up
	fclose(fp);
	curl_easy_cleanup(curl);

	if (res != CURLE_OK) {
		fprintf(stderr, "curl_easy_perform() failed: %s\n",
			curl_easy_strerror(res));
		return 0;
	}

	// Clean up libcurl
	curl_global_cleanup();

	printf("File downloaded: %s\n", output_file);

	return 1;
}

char prefix_match(char *str, char *prefix)
{
	while (*str != '\0' && *prefix != '\0' && *str++ == *prefix++) ;
	return *prefix == '\0';
}

void add_prefix(char *into, char *to, char *prefix)
{
	while (*prefix != '\0')
		*into++ = *prefix++;

	while (*to != '\0')
		*into++ = *to++;
}

void print_if_prefix_match(char *str, char *prefix)
{
	if (!prefix_match(str, prefix))
		return;

	char full_url[1000] = { 0 };
	add_prefix(full_url, str, "https:");

	char output_filename[100] = { 0 };
	copy_filename_from_url(output_filename, full_url);
	try_download_file(full_url, output_filename);
}

int main()
{
	// TODO get the index.html instead of downloading first
	FILE *f = fopen("index.html", "r");
	bool in_str = false;
	char ch;

	char buffer[1000] = { 0 };
	char *b = buffer;

	for (;;) {
		ch = getc(f);
		if (ch == EOF)
			break;
		if (ch == '"') {
			if (in_str) {
				*b = '\0';
				print_if_prefix_match(buffer, "//i.4cdn.org/");
				b = buffer;
			}
			in_str = !in_str;
			continue;
		}

		if (in_str) {
			*b++ = ch;
		}
	}
}
