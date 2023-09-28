#include "../include/woody.h"

void get_base_addr(void *file_data, t_exploit_data *exploit)
{
	Elf64_Phdr *seg = NULL;

	seg = (Elf64_Phdr *)(file_data + exploit->header->e_phoff);
	for (unsigned long i = 0; i < exploit->header->e_phnum; i++)
	{
		if (seg[i].p_type == PT_LOAD)
		{
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

void find_cave(t_exploit_data *exploit, t_data *data)
{
	Elf64_Phdr *seg;
	Elf64_Ehdr *header = exploit->header;
	unsigned long i = 0;
	int len = 200;

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
			exploit->end_payload = exploit->start_payload + len;
			int ret = check_corruption(exploit);
			if (ret == header->e_phnum)
			{
				exploit->new_entry_point = seg[i].p_vaddr + seg[i].p_filesz;
				printf("\tFound cave at offset -> 0x%lx.\n", exploit->start_payload + i);
				return ;
			}
		}
	}
	print_error("");
}

void insert_code(Elf64_Ehdr *header, t_data *data)
{
	t_exploit_data exploit;

	printf("Insert Code:\n");
	exploit.header = header;
	exploit.init_entry_point = header->e_entry;
	printf("\tEntry point address 0x%lx\n", exploit.init_entry_point);
	find_cave(&exploit, data);
}