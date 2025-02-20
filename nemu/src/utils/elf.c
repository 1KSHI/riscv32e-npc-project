#include <common.h>
#include <elf.h>

#define SYMTAB_SIZE 4096
#define STRTAB_SIZE 4096
#define SHTAB_SIZE 4096
#define SHSTRTAB_SIZE 4096

FILE *elf_fp = NULL;

Elf32_Ehdr elf_head;

Elf32_Sym symtab[SYMTAB_SIZE];
int sym_num = 0;
char strtab[STRTAB_SIZE];

void read_elfHead();
void read_shTable();
void read_sections();

void init_elf(const char *elf_file) {
    if (elf_file != NULL) {
        FILE *fp = fopen(elf_file, "rb");//binary file "rb"
        Assert(fp, "Can not open '%s'", elf_file);
        elf_fp = fp;
        Log("The ELF file is opened: %s", elf_file);

        /* read the elf head */
        read_elfHead();

        /* read some sections */
        read_sections();
        Log("Read the symtab and strtab successfully.");
    }
    else 
        Log("There is no ELF file");
}

/* void test_elfread() {*/
/*     Log("The number of the section head: %u", elf_head.e_shnum);*/
/* }*/
 
void read_elfHead() {
    if (elf_fp != NULL) {
        size_t n = fread(&elf_head, sizeof(elf_head), 1, elf_fp);
        Assert(n == 1, "Reading the ELF head failed.");
        Log("The ELF head is readed successfully"); 
        /* test_elfread();*/
    }
}

void read_sections() {
    size_t size_sh_table = elf_head.e_shnum * elf_head.e_shentsize;

    Elf32_Shdr sh_table[SHTAB_SIZE];
    char shstrtab[SHSTRTAB_SIZE];

    //read the section head table
    int n = fseek(elf_fp, (long)elf_head.e_shoff, SEEK_SET);
    assert(n == 0);

    assert(SHTAB_SIZE >= size_sh_table);
    n = fread(sh_table, size_sh_table, 1, elf_fp);
    assert(n == 1);

    //read the shstrtab
    n = fseek(elf_fp, (long)sh_table[elf_head.e_shstrndx].sh_offset, SEEK_SET);
    assert(n == 0);

    assert(SHSTRTAB_SIZE >= sh_table[elf_head.e_shstrndx].sh_size);
    n = fread(shstrtab, sh_table[elf_head.e_shstrndx].sh_size, 1, elf_fp);
    assert(n == 1);

    //read the .symtab and .strtab
    for (int i = 0; i < elf_head.e_shnum; i ++) {
        if (strcmp(".symtab", shstrtab + sh_table[i].sh_name) == 0) {
            n = fseek(elf_fp, (long)sh_table[i].sh_offset, SEEK_SET);
            assert(n == 0);

            assert(SYMTAB_SIZE >= sh_table[i].sh_size);
            n = fread(symtab, sh_table[i].sh_size, 1, elf_fp);
            assert(n == 1);
            sym_num = sh_table[i].sh_size / sh_table[i].sh_entsize;
        }
        if (strcmp(".strtab", shstrtab + sh_table[i].sh_name) == 0) {
            n = fseek(elf_fp, (long)sh_table[i].sh_offset, SEEK_SET);
            assert(n == 0);

            assert(STRTAB_SIZE >= sh_table[i].sh_size);
            n = fread(strtab, sh_table[i].sh_size, 1, elf_fp);
            assert(n == 1);
        }
    }
    // Print the symtab content in hexadecimal format
    // printf("symtab content in hexadecimal:\n");
    // for (int i = 0; i < 40; i++) {
        // printf("symtab[%d].st_value = 0x%x    ", i, symtab[i].st_value);
        // printf("symtab[%d].st_info = 0x%x    ", i, symtab[i].st_info);
        // printf("symtab[%d].st_name = 0x%x\n", i, symtab[i].st_name);
        // printf("%02x ", ((unsigned char*)symtab)[i]);
        // if ((i + 1) % 16 == 0) {
        //     printf("\n");
        // }
    // }
    // printf("\n");

    // // Print the strtab content in hexadecimal format
    // printf("strtab content in hexadecimal:\n");
    // for (size_t i = 0; i < STRTAB_SIZE; i++) {
    //     printf("%02x ", ((unsigned char*)strtab)[i]);
    //     if ((i + 1) % 16 == 0) {
    //         printf("\n");
    //     }
    // }
    // printf("\n");
}

