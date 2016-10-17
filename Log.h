#include <stdio.h>

void wright_to_file(char* path, char* data)
{
	FILE* file = fopen((const char*) path, "a");	
	fputs((const char*) data, file);	
	fclose(file);
}

// returns content of file NOTE RETURN VALUE NEEDS TO DELETED VIA free()
char* read_file(char* path)
{
	FILE* file = fopen((const char*) path, "r");	
	// get len of file	
	fseek(file, 0L, SEEK_END);
	int len = (int) ftell(file);
	fseek(file, 0L, SEEK_SET);
	// read file
	char* buff = malloc(len);
	for (int i = 0; i < len; i++)
	{
		buff[i] = fgetc(file);
	}
	fclose(file);
	printf("%s\n", buff);
	return buff;
}
