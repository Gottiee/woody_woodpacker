#include "../include/woody.h"

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
	printf("\t\tModified return address: jmp to 0x%1$x(%1$d)\n", jmp);
}

void update_payload(t_exploit_data *exploit, t_payload *payload)
{
	printf("\tUpdate Payload\n");
	// inject_address(payload, exploit->init_entry_point);
	inject_return_address(payload, exploit);
}
