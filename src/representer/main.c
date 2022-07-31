#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>

#include <unistd.h>

/* max number of files specifiable through CLI */
#define FILE_N_CLI_LIMIT 1024

/* remove unnecessary whitespace */
void
rmwhsp(char** file_content)
{
	
}

/* return the source file's IR code as a string in heap */
char*
compile(FILE* f)
{
	rmwhsp
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
		char* ofile_name = argv[i];
		char* ir_code = compile(files[i]);

		
		if 
		FILE* out_file = fopen(argv[i], "w");
		fclose(files[i]);
	}
}