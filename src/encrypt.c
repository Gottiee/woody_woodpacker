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
            exploit->text_off = seg[i].sh_offset;
            exploit->text_size = seg[i].sh_size;
            return;
        }
    }
    print_error("%s: no text section found", data->file_name, data);
}

void key_gen(char key[KEY_SIZE])
{
    srand(time(NULL));
    const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#$%^&*()-_=+{}|;:'\",.<>?/\\`~ ";
    const int charsetSize = sizeof(charset) - 1;
    for (int i = 0; i < KEY_SIZE - 1; ++i)
    {
        int randomIndex = rand() % charsetSize;
        key[i] = charset[randomIndex];
    }
    key[KEY_SIZE - 1] = '\0';
}

void XORCipher(char *data, char *key, unsigned int dataLen, int keyLen)
{
    for (unsigned int i = 0; i < dataLen; ++i)
    {
        printf("0x%02hhx ", data[i]);
        data[i] = data[i] ^ key[i % keyLen];
        printf("0x%02hhx ", data[i]);

    }
    printf("\n");
}

void encrypt(t_exploit_data *exploit, t_data *data, t_payload *payload)
{
    payload->key_size = KEY_SIZE;
    printf("\tEncrypt Text section\n");
    find_text_section(data, exploit);
    key_gen(payload->key);
    printf("\t\tKey generated: [%.20s...] %d bytes\n", payload->key, KEY_SIZE);
    XORCipher(data->file_data + exploit->text_off, payload->key, exploit->text_size, KEY_SIZE);
    printf("\t\t.text section has been encrypted\n");
}