#include "../include/woody.h"

int open_file(char *file)
{
	int fd = open(file, O_RDONLY);
	return fd;
}

void print_perror(char *string, char *cmd, t_data *data)
{
	int malloc_size = 500;

	if (cmd)
		malloc_size += strlen(cmd) + strlen(data->file_name);
	if (string)
		malloc_size += strlen(string);
	char *err_mess = malloc(malloc_size);
	if (!err_mess)
		perror("Woody:");
	else
	{
		if (!string)
			sprintf(err_mess, "%s: '%s'", cmd, data->file_name);
		else
			sprintf(err_mess, string, data->file_name);
		perror(err_mess);
		free(err_mess);
	}
	if (data->fd > 0)
		close(data->fd);
	if (data->size_mmap > 0 && data->file_data != MAP_FAILED)
		munmap(data->file_data, data->size_mmap);
	exit(1);
}

void print_error(char *err, char *file_name, t_data *data)
{
	if (file_name)
		fprintf(stderr, err, file_name);
	else
		fprintf(stderr, "%s", err);
	if (data->fd > 0)
		close(data->fd);
	if (data->size_mmap > 0 && data->file_data)
		munmap(data->file_data, data->size_mmap);
	exit(1);
}

void check_format(t_data *data)
{
	Elf64_Ehdr *header = (Elf64_Ehdr *)data->file_data;

	if (header->e_ident[EI_CLASS] == ELFCLASS32)
		print_error("woody_woodpacker: %s: File format not supported: \'elf32 bit\'", data->file_name, data);
	else if (header->e_ident[EI_CLASS] == ELFCLASS64)
		pack(header, data);
	else
		print_error("woody_woodpacker: %s: File format not recognized\n", data->file_name, data);
}

char *mmap_file(t_data *data)
{
	struct stat file_stat;
	char *file_data;

	if (fstat(data->fd, &file_stat) == -1)
		print_perror(NULL, "fstat", data);
	if (S_ISDIR(file_stat.st_mode))
		print_error("woody_woodpacker: Warning: '%s' is a directory\n", data->file_name, data);
	file_data = mmap(NULL, file_stat.st_size, PROT_READ, MAP_SHARED, data->fd, 0);
	if (file_data == MAP_FAILED)
		print_perror(NULL, "mmap", data);
	data->size_mmap = file_stat.st_size;
	return file_data;
}

void check_arg(int argc, char **argv, t_data *data)
{
	printf("WOODY PACKING\n\n");
	printf("Looking for a file...\n");
	if (argc == 1)
	{
		data->file_name = "a.out";
		if ((data->fd = open_file("a.out")) == -1)
			print_perror(NULL, "woody_woodpacker", data);
		printf("\tFILE: \'a.out\'\n");
	}
	else
	{
		data->file_name = argv[1];
		if (!strncmp("woody", data->file_name, strlen("woody")))
			print_error("%s: argv[1] can't be 'woody'\n", "woody_woodpacker", data);
		if ((data->fd = open_file(argv[1])) == -1)
			print_perror(NULL, "woody_woodpacker", data);
		printf("\tFILE: \'%s\'\n", data->file_name);
	}
}

void init_struct(t_data *data)
{
	data->fd = 0;
	data->size_mmap = 0;
	data->file_data = NULL;
	data->file_name = NULL;
}

int main(int argc, char **argv)
{
	t_data data;

	init_struct(&data);
	check_arg(argc, argv, &data);
	data.file_data = mmap_file(&data);
	check_format(&data);
	close(data.fd);
	munmap(data.file_data, data.size_mmap);
}