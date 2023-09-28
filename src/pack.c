#include "../include/woody.h"

void duplicated_binary(t_data *data)
{
	int woodyfd;
	int ret;

	printf("\nCreation of woody file:\n");
	woodyfd = open("woody", O_CREAT | O_TRUNC | O_RDWR, S_IRWXU);
	if (woodyfd < 0)
	{
		data->file_name = "woody";
		print_perror(NULL, "open", data);
	}
	if ((ret = write(woodyfd, data->file_data, data->size_mmap)) < data->size_mmap)
		print_error("%s: data can't be copied", "woody", data);
	close(data->fd);
	munmap(data->file_data, data->size_mmap);
	data->fd = woodyfd;
	data->size_mmap = ret;
	data->file_name = "woody";
	data->file_data = mmap(NULL, data->size_mmap, PROT_WRITE | PROT_EXEC | PROT_READ, MAP_SHARED, data->fd, 0);
	if (data->file_data == MAP_FAILED)
		print_perror(NULL, "mmap", data);
	printf("\twoody created\n\n");
}

void pack(Elf64_Ehdr *header, t_data *data)
{
	duplicated_binary(data);
	insert_code(header, data);
}