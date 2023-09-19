#ifndef WOODY_H
#define WOODY_H

#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
//#include <elf.h>
#include <unistd.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <stdbool.h>

typedef struct s_data
{
	int fd;
	off_t size_mmap;
	char *file_data;
	char *file_name;
} t_data;

typedef struct s_64
{
	//Elf64_Shdr	*symbol_section_header;
	//Elf64_Shdr	*string_section_header;
	//Elf64_Sym	*symbols;
	//Elf64_Ehdr	*header;
} t_64;

#endif