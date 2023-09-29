#include "../include/woody.h"

Elf64_Addr *get_strtab(Elf64_Ehdr *header, char *file_data)
{

}

void find_text_section(t_data *data, t_exploit_data *exploit)
{
	Elf64_Shdr *seg = (void *)data->file_data + exploit->header->e_shoff;
	Elf64_Addr strtab;

	strtab = get_strtab(exploit->header, data->file_data);
}

void update_payload(t_exploit_data *exploit, t_data *data, t_payload *payload)
{
	find_text_section(data, exploit);
	(void)payload;
}
