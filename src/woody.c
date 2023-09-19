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
	if (data->size_mmap > 0 && data->file_data)
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

void check_arg(int argc, char **argv, t_data *data)
{
	if (argc == 1)
	{
		data->file_name = "a.out";
		if ((data->fd = open_file("a.out")) == -1)
			print_perror(NULL, "woody", data);
		printf("\'a.out\' loaded\n");
	}
	else
	{
		data->file_name = argv[1];
		if ((data->fd = open_file(argv[1])) == -1)
			print_perror(NULL, "woody", data);
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
	//data.file_data = mmap_file(&data);
	//choose_bits(&data);
	//close(data.fd);
	//munmap(data.file_data, data.size_mmap);
}