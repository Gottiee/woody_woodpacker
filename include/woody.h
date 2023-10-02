#ifndef WOODY_H
#define WOODY_H

#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <elf.h>
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

typedef struct s_exploit_data
{
	Elf64_Ehdr *header;
	Elf64_Phdr *seg;
	Elf64_Addr init_entry_point;
	Elf64_Off new_entry_point;
	Elf64_Addr first_seg;
	Elf64_Addr start_payload;
	Elf64_Addr end_payload;
	Elf64_Off text_size;

} t_exploit_data;

typedef struct s_payload
{
	char *data;
	size_t len;
} t_payload;

/* woody.c */
void print_error(char *err, char *file_name, t_data *data);
void print_perror(char *string, char *cmd, t_data *data);

/* pack.c */
void pack(Elf64_Ehdr *header, t_data *data);

/* insert.c */
void insert_code(Elf64_Ehdr *header, t_data *data);

/* payload.c */
void update_payload(t_exploit_data *exploit, t_data *data, t_payload *payload);

#endif