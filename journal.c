#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define TITLE "david's journal"
#define ENTRY_DIR "entries"
#define MAX_ENTRIES 1024

static char *entries[MAX_ENTRIES];
static int num_entries;

static void load_entries()
{
	DIR *d = opendir(ENTRY_DIR);
	if (!d) exit(1);

	struct dirent *de;
	while ((de = readdir(d))) {
		if (de->d_name[0] == '.') continue;
		if (num_entries == MAX_ENTRIES) exit(1);

		size_t entry_size = strlen(de->d_name) + 1;
		char *entry = malloc(entry_size);
		memcpy(entry, de->d_name, entry_size);
		entries[num_entries++] = entry;
	}

	closedir(d);
}

static void print_entry(char *entry)
{
	size_t len = strlen(entry) + 1;
	char *path = malloc(sizeof(ENTRY_DIR) + len);
	memcpy(path, ENTRY_DIR"/", sizeof(ENTRY_DIR));
	memcpy(path + sizeof(ENTRY_DIR), entry, len);
	FILE *f = fopen(path, "r");
	free(path);
	if (!f) return;

	char date[11];
	long time = atol(entry);
	strftime(date, sizeof(date), "%Y/%m/%d", localtime(&time));
	printf("<div class='entry'><p><span class='date'>%s</span>&emsp;", date);

	for (int c = getc(f), nl = 0; c != EOF; c = getc(f)) {
		if (c == '\n') {
			++nl;
			continue;
		}

		if (nl > 0) {
			printf(nl == 1 ? "<br>" : "</p><p>");
			nl = 0;
		}

		putchar(c);
	}

	fclose(f);

	printf("</p></div>\n");
}

static int entry_cmp(const void *a, const void *b)
{
	return atoi(*(const char **)b) - atoi(*(const char **)a);
}

int main()
{
	load_entries();
	qsort(entries, num_entries, sizeof(char *), entry_cmp);

	printf(
		"<!DOCTYPE html>\n"
		"<html lang='en'>\n"
		"<head>\n"
		"<meta charset='utf-8'>\n"
		"<meta name='viewport' content='width=device-width, initial-scale=1'>\n"
		"<link href='style.css' rel='stylesheet'>\n"
		"<title>"TITLE"</title>\n"
		"</head>\n"
		"<body>\n"
		"<h1>"TITLE"</h1>\n");

	for (int i = 0; i < num_entries; ++i) {
		print_entry(entries[i]);
		free(entries[i]);
	}

	printf(
		"</body>\n"
		"</html>\n");

	return 0;
}
