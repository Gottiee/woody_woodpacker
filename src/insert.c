#include "../include/woody.h"

void get_base_addr(void *file_data, t_exploit_data *exploit)
{
	Elf64_Phdr *seg = NULL;

	seg = (Elf64_Phdr *)(file_data + exploit->header->e_phoff);
	for (unsigned long i = 0; i < exploit->header->e_phnum; i++)
	{
		if (seg[i].p_type == PT_LOAD)
		{
			printf("\t\tFirst seg base address found at 0x%ld\n", seg[i].p_vaddr);
			exploit->first_seg = seg[i].p_vaddr;
			break;
		}
	}
}

int check_corruption(t_exploit_data *exploit)
{
	int i = 0;

	for (i = 0; i < exploit->header->e_phnum; i++)
	{
		if (exploit->seg[i].p_offset >= exploit->start_payload && exploit->seg[i].p_offset < exploit->end_payload && exploit->seg[i].p_filesz > 0)
			break;
	}
	return i;
}

void load_payload(t_payload *payload, t_data *data)
{
	printf("\tLoading Payload:\n");
	int fd = open("asm/opcode", O_RDONLY);
	size_t size;

	if (fd == -1)
		print_perror("./asm/payload: open", NULL, data);
	size = lseek(fd, 0, SEEK_END);
	payload->data = malloc(sizeof(char) * (size + 1));
	if (!payload->data)
	{
		close(fd);
		print_perror("payload malloc", NULL, data);
	}
	payload->data[size] = '\0';
	lseek(fd, 0, SEEK_SET);
	int ret = read(fd, payload->data, size);
	payload->len = size;
	(void)ret;
	printf("\t\tPayload size: %ld (%.20s[...])\n", payload->len, payload->data);
	close(fd);
}

void find_cave(t_exploit_data *exploit, t_data *data, t_payload *payload)
{
	Elf64_Phdr *seg;
	Elf64_Ehdr *header = exploit->header;
	unsigned long i = 0;

	printf("\tFind a cave:\n");
	exploit->start_payload = 0;
	exploit->end_payload = 0;
	exploit->first_seg = 0;
	seg = (Elf64_Phdr *)(data->file_data + header->e_phoff);
	exploit->seg = seg;
	get_base_addr(data->file_data, exploit);
	for (i = 0; i < header->e_phnum; i++)
	{
		if (seg[i].p_filesz > 0 && seg[i].p_filesz == seg[i].p_memsz && (seg[i].p_flags & (PF_X)))
		{
			exploit->start_payload = seg[i].p_offset + seg[i].p_filesz;
			exploit->end_payload = exploit->start_payload + payload->len;
			int ret = check_corruption(exploit);
			if (ret == header->e_phnum)
			{
				exploit->new_entry_point = seg[i].p_vaddr + seg[i].p_filesz;
				if (exploit->new_entry_point + data->file_data + payload->len > data->file_data + data->size_mmap)
					continue;
				printf("\t\tFound cave at offset -> 0x%lx.\n", exploit->start_payload);
				printf("\t\t0x%1$lx (%1$ld) bytes available\n", seg[i + 1].p_offset - exploit->start_payload);
				seg->p_filesz += payload->len;
				seg->p_memsz += payload->len;
				return;
			}
		}
	}
	print_error("%s: No cave Found\n", "woody_woodpacker", data);
}

void insert_payload(t_exploit_data *exploit, t_data *data, t_payload *payload)
{
	update_payload(exploit, data, payload);
	if (!memcpy((void *)data->file_data + exploit->new_entry_point, payload->data, payload->len))
		print_perror("Copy payload to woody: memmove", NULL, data);
	exploit->header->e_entry = exploit->new_entry_point;
	printf("\t\tPayload injected: new entry point at offset 0x%lx\n", exploit->new_entry_point);
}

void insert_code(Elf64_Ehdr *header, t_data *data)
{
	t_exploit_data exploit;
	t_payload payload;

	printf("Insert Code:\n");
	exploit.header = header;
	exploit.init_entry_point = header->e_entry;
	printf("\tInitial entry point address 0x%lx\n", exploit.init_entry_point);
	load_payload(&payload, data);
	find_cave(&exploit, data, &payload);
	insert_payload(&exploit, data, &payload);
	free(payload.data);
}