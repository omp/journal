#define _XOPEN_SOURCE
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

static void load_entries(char *dir, size_t *max_fn_size)
{
	DIR *d = opendir(dir);
	if (!d) exit(1);

	struct dirent *de;
	while ((de = readdir(d))) {
		if (de->d_name[0] == '.') continue;
		if (num_entries == MAX_ENTRIES) exit(1);

		size_t fn_size = strlen(de->d_name) + 1;
		if (fn_size > *max_fn_size)
			*max_fn_size = fn_size;
		char *entry = malloc(fn_size);
		memcpy(entry, de->d_name, fn_size);
		entries[num_entries++] = entry;
	}

	closedir(d);
}

static void print_entry(char *path, char *ts)
{
	FILE *f = fopen(path, "r");
	if (!f) return;

	struct tm tm;
	strptime(ts, "%s", &tm);
	char date[11];
	strftime(date, sizeof(date), "%Y/%m/%d", &tm);
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
	puts("</p></div>");
}

static int entry_cmp(const void *a, const void *b)
{
	return atoi(*(const char **)b) - atoi(*(const char **)a);
}

int main()
{
	size_t max_fn_size = 0;
	load_entries(ENTRY_DIR, &max_fn_size);
	qsort(entries, num_entries, sizeof(char *), entry_cmp);

	puts(
		"<!DOCTYPE html>\n"
		"<html lang='en'>\n"
		"<head>\n"
		"<meta charset='utf-8'>\n"
		"<meta name='viewport' content='width=device-width, initial-scale=1'>\n"
		"<link href='style.css' rel='stylesheet'>\n"
		"<title>"TITLE"</title>\n"
		"</head>\n"
		"<body>\n"
		"<h1>"TITLE"</h1>");

	char *path = malloc(sizeof(ENTRY_DIR) + max_fn_size);
	memcpy(path, ENTRY_DIR"/", sizeof(ENTRY_DIR));
	for (int i = 0; i < num_entries; ++i) {
		strcpy(path + sizeof(ENTRY_DIR), entries[i]);
		print_entry(path, entries[i]);
		free(entries[i]);
	}

	free(path);
	puts(
		"</body>\n"
		"</html>");

	return 0;
}
