#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>

#include <unistd.h>

/* max number of files specifiable through CLI */
#define FILE_N_CLI_LIMIT 1024

/* length of the buffer while reading a file from disk */
#define READ_BUFF_SIZE 4096

struct preproc {
	char* name;
	char*
};

/* store string in heap from its beginning until one of a set of characters occur.
   if the string ends before one of the characters are found, the entire string is stored.
   if the first character in the string matches one of the characters, the return value is null. */
char*
storesubstr(char* str, const char* ch)
{
	int i, str_len, ch_len, substr_len = 0;
	char* substr;

	str_len = strlen(str);
	ch_len = strlen(ch);

	/* count substring length */
	for (i = 0; i < str_len; i++) {
		int j, found = 0;

		for (j = 0; j < ch_len; j++) {
			if (str[i] == ch[j]) {
				found = 1;
				break;
			}
		}

		if (found) {
			break;
		}
	}
	substr_len = i;

	if (substr_len == 0) {
		return NULL;
	}

	substr = calloc(substr_len + 1, 1);
	strncpy(substr, str, substr_len);
	return substr;
}

void
execpreproc()
{
}

/* parse preprocessor */
char*
parsepreproc(char* str)
{
}

/* parse and execute preprocessor directives */
void
preproc(char** file_cont)
{
	/* j is used to fill dir_name */
	int i, file_len, is_preproc = 0, j = 0;

	/* directive name */
	char* dir_name;

	char* dir_value;

	file_len = strlen(*file_cont);
	memset(dir_name, 0, 8);

	/* find lines that start with '#' and parse them */
	for (i = 0; i < file_len; i++) {
		if (file_cont[i] == '#' && (i == 0 || file_cont[i-1] == '\n')) {
			/* it's the beginning of a preprocessor directive */
			dir_name = storesubstr(&file_cont[i+1], " <\"");

			/* skip name and jump to next word */
			i += strlen(dir_name) + 2;

			if (strcmp(dir_name, "include") == 0) {
				if (file_cont[i-1] == '\"') {

			}

		}
	}
}

/* remove unnecessary whitespace from file content and convert tabs to spaces */
void
rmwhsp(char** file_cont)
{
	/* dlt_start = delete start, dlt_end = delete end, is_ws = is whitespace? */
	int i, file_len, dlt_start = -1, dlt_end = -1, is_ws = 0, removed_len = 0;

	file_len = strlen(*file_cont);

	for (i = 0; i < file_len; i++) {
		switch (*file_cont[i]) {
		case '\t':
			file_cont[i] = ' ';
		case ' ':
			dlt_start = i;
			is_ws = 1;
			break;
		default:
			if (is_ws) {
				dlt_end = i;
				is_ws = 0;

				/* check if the character before the first whitespace and the character after the last whitespace are
				   alphanumeric or underscores */
				if ((isalnum(*file_cont[dlt_start - 1]) || *file_cont[dlt_start - 1] == '_') &&
					(isalnum(*file_cont[dlt_end + 1]) || *file_cont[dlt_end + 1] == '_')) {
					/* leave one space */
					dlt_end--;
				}

				memmove(*file_cont + dlt_start, *file_cont + dlt_end, file_len - dlt_end);
				removed_len += dlt_end - dlt_start;
				i -= removed_len;
			}
			break;
		}
	}

	/* shrink the file content array by removing unused space */
	*file_cont[file_len] = 0;
	*file_cont = realloc(file_cont, file_len - removed_len);
}

/* read the nth unicode character (encoded in UTF-8) of str and return it as an integer.
   optionally also return its byte position in byte_pos if it's not null.
   this function assumes that str is a correctly encoded UTF-8 string.
   for reference: https://en.wikipedia.org/wiki/UTF-8#Encoding */
int
nthch(char* str, long ch_pos, long* byte_pos)
{
	int i, j, k, ch = 0;

	/* go to nth character */
	for (i = 0; i < ch_pos; i++) {
		/* check most significant bits; by the end of this loop, the value of j is the length of the UTF-8 byte sequence */
		for (j = 1; str[i] & (0x03 << 7-j) == (0x03 << 7-j); j++);

		/* skip bytes */
		i += j-1;
		/* ch_pos is also increased since skipping bytes does not mean skipping characters */
		ch_pos += j-1;

	}

	/* check most significant bits; by the end of this loop, the value of j is the length of the UTF-8 byte sequence */
	for (j = 1; str[i] & (0x03 << 7-j) == (0x03 << 7-j); j++);

	switch (j) {
	case 1:
		ch = ch | (str[i] & 0x7F);
		break;
	case 2:
		ch = ch | ((str[i] & 0x1F) << 6);
		ch = ch | (str[i+1] & 0x3F);
		break;
	case 3:
		ch = ch | ((str[i] & 0x0F) << 12);
		ch = ch | ((str[i+1] & 0x3F) << 6);
		ch = ch | (str[i+2] & 0x3F);
		break;
	case 4:
		ch = ch | ((str[i] & 0x07) << 18);
		ch = ch | ((str[i+1] & 0x3F) << 12);
		ch = ch | ((str[i+2] & 0x3F) << 6);
		ch = ch | (str[i+3] & 0x3F);
		break;
	}

	return ch;
}

/* apply line reconstruction */
void
lnrecon(char** file_cont)
{
	/* remove excessive whitespace */
	rmwhsp(&file_cont);
}

/* read the entire file, allocate it in heap and return it. return NULL if an error occurred. */
char*
readf(FILE* f)
{
	char* total;
	char buffer[READ_BUFF_SIZE];
	int bytes_read, total_len, total_i = 0;

	/* get the file size */
	fseek(f, 0, SEEK_END);
	total_len = ftell(f);
	fseek(f, 0, SEEK_SET);

	total = malloc(total_len + 1);

	/* read bytes */
	while ((bytes_read = fread(buffer, 1, READ_BUFF_SIZE, f)) > 0) {
		/* copy the buffer */
		memcpy(total + total_i, buffer, bytes_read);
		total_i += bytes_read;
	}

	/* the loop may have ended due to an error */
	if (ferror(f)) {
		return NULL;
	}

	/* set string terminator */
	total[total_len] = 0;

	return total;
}

/* return the source file's IR code as a string in heap or NULL if an error occurred */
char*
compile(FILE* f)
{
	/* read all the file */
	char* file_cont = readf(f);

	if (!file_cont) {
		return NULL;
	}

	lnrecon(file_cont);

	free(file_cont);
}

/* attempt to change file name (".c" -> ".2ir") in place, name_len is the length of the name array */
void
chgfname(char** name, int name_len)
{
	int i;

	/* name_len - 1 is the last character (always null)
	   name_len - 2 is the last usable character
	   the name array must have at least 3 null characters at the end: two for the new name (whose
	   length differs of 2 characters) and one as string terminator */
	if (name[name_len - 3] != 0) {
		/* not enough space for new name, do nothing */
		return;
	}

	/* find the dot character and do the change */
	for (i = 0; i < name_len; i++) {
		if (*name[i] == '.') {
			strcpy(*name + i, ".2ir");
		}
	}
}

int
main(int argc, char** argv)
{
	int i, null_files;
	FILE* files[FILE_N_CLI_LIMIT];

	/* init files array */
	memset(files, 0, sizeof(FILE*) * FILE_N_CLI_LIMIT);

	/* read arguments */
	/* will be necessary if there will be arguments */
	/*
	char arg;
	while ((arg = getopt(argc, argv, "") != -1) {
		switch (arg) {
		case '':
			break;
		default:
			break;
	}
	*/

	/* assumes all the CLI arguments are files to be compiled */
	if (argc > FILE_N_CLI_LIMIT) {
		errno = 7;
		perror(argv[0]);
	}

	/* open all files */
	for (i = 1; i < argc; i++) {
		files[i] = fopen(argv[i], "r");

		/* check if file opened correctly */
		if (files[i] == NULL) {
			perror(argv[0]);

			/* one or more files were NULL */
			null_files = 1;
		}
	}

	if (null_files) {
		return 1;
	}

	/* compile all files and then close them */
	for (i = 0; files[i] != NULL; i++) {
		FILE* ofile; /* output file */
		char* ofile_name;
		char* ir_code;
		int ifile_len, ofile_len;

		/* change name */
		ifile_len = strlen(argv[i]);
		ofile_len = ifile_len + 2;
		ofile_name = calloc(ofile_len + 1, 1);
		chgfname(ofile_name, ofile_len + 1);

		ir_code = compile(files[i]);

		FILE* out_file = fopen(ofile_name, "w");
		fputs(ir_code, out_file);
		fclose(out_file);

		fclose(files[i]);
	}
}
