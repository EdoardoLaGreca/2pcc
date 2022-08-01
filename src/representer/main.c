#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>

#include <unistd.h>

/* max number of files specifiable through CLI */
#define FILE_N_CLI_LIMIT 1024

/* length of the buffer while reading a file from disk */
#define READ_BUFF_SIZE 4096

/* remove unnecessary whitespace */
void
rmwhsp(char** file_content)
{
	/* dlt_start = delete start, dlt_end = delete end, is_ws = is whitespace? */
	int i, file_len, dlt_start = -1, dlt_end = -1, is_ws = 0, removed_len = 0;

	file_len = strlen(*file_content);

	for (i = 0; i < file_len; i++) {
		switch (*file_content[i]) {
		case ' ':
		case '\t':
		case '\n':
			dlt_start = i
			is_ws = 1;
			break;
		default:
			if (is_ws) {
				dlt_end = i-1;
				is_ws = 0;
				memmove(*file_content + dlt_start, *file_content + dlt_end, file_len - dlt_end);
				removed_len += dlt_end - dlt_start;
			}
			break;
		}
	}

	/* shrink the file content array by removing unused space */
	*file_content = realloc(file_content, file_len - removed_len);
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

	total = malloc(total_len + 1)

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

/* return the source file's IR code as a string in heap */
char*
compile(FILE* f)
{
	/* read all the file */
	char* file_content = readf(f);

	rmwhsp(&file_content);

	free(file_content);
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
		int ifile_len, ofile_len;

		/* change name */
		ifile_len = strlen(argv[i]);
		ofile_len = ifile_len + 2
		ofile_name = calloc(ofile_len + 1, 1);
		chgfname(ofile_name, ofile_len + 1);

		ir_code = compile(files[i]);

		FILE* out_file = fopen(ofile_name, "w");
		fputs(ir_code, out_file);
		fclose(out_file);

		fclose(files[i]);
	}
}
