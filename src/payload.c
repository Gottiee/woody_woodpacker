#include "../include/woody.h"

Elf64_Addr get_strtab(Elf64_Ehdr *header, char *file_data)
{
	Elf64_Shdr *seg = (void *)file_data + header->e_shoff;
	return seg[header->e_shstrndx].sh_offset;
}

void find_text_section(t_data *data, t_exploit_data *exploit)
{
	Elf64_Shdr *seg = (void *)data->file_data + exploit->header->e_shoff;
	Elf64_Addr strtab;
	char *name;

	strtab = get_strtab(exploit->header, data->file_data);
	for (int i = 0; i < exploit->header->e_shnum; i++)
	{
		name = (void *)exploit->header + strtab + seg[i].sh_name;
		if (!strcmp(".text", name))
		{
			exploit->text_size = seg[i].sh_size;
			return;
		}
	}
	print_error("%s: no text section found", data->file_name, data);
}

void inject_address(t_payload *payload, long long value)
{
	char *code = "\xff\xff\xff\xff";

	for (unsigned int i = 0; i < payload->len - strlen(code); i++)
	{

		if (!strncmp(&payload->data[i], code, strlen(code)))
		{
			memcpy(&payload->data[i], (char *)&value, strlen(code));
			printf("value 0x%llx as well been inserted\n", value);
			return;
		}
	}
}

void inject_return_address(t_payload *payload, t_exploit_data *exploit)
{
	Elf64_Word jmp = exploit->init_entry_point + exploit->first_seg - (exploit->new_entry_point + payload->len);
	*(Elf64_Word *)(payload->data + payload->len - 4) = jmp;
	printf("\t\tModified return address: 0x%d\n", jmp);
}

void update_payload(t_exploit_data *exploit, t_data *data, t_payload *payload)
{
	printf("\tUpdate Payload\n");
	find_text_section(data, exploit);
	// inject_address(payload, exploit->init_entry_point);
	inject_return_address(payload, exploit);

	// printf("Entry point = %lx first seg = 0x%lx\nJmp = %x", exploit->init_entry_point, jmp);
}
