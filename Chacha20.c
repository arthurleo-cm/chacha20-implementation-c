#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
// Esses valores são de referencia após o quarter_round
    //  a = 0xea2a92f4
    //  b = 0xcb1cf8ce
    //  c = 0x4581472e
    //  d = 0x5881c4bb
#define EXPECTED_A 0xea2a92f4
#define EXPECTED_B 0xcb1cf8ce
#define EXPECTED_C 0x4581472e
#define EXPECTED_D 0x5881c4bb
static uint32_t rotl32(uint32_t value, unsigned int shift)
{
    return (value << shift) | (value >>(32 - shift));
}
static void print_hex(const uint8_t *data, size_t length)
{
    for (size_t i = 0; i < length; i++)
    {
        printf("%02x",data[i]);
    }
    printf("\n");
}
void quarter_round(uint32_t *a,uint32_t *b,uint32_t *c,uint32_t *d)
{
 *a = *a + *b;
 *d = *d ^ *a;
 *d = rotl32(*d,16);
 
 *c = *c + *d;
 *b = *b ^ *c;
 *b =rotl32(*b,12);

 *a = *a + *b;
 *d = *d ^ *a;
 *d = rotl32(*d,8);

 *c = *c + *d;
 *b = *b ^ *c;
 *b =rotl32(*b,7);
}

int main()
{
    uint32_t value = 0x12345678;
    uint32_t rotated = rotl32(value,16);

    printf("Original: %08x\n",value);
    printf("Rotacionado: %08x\n",rotated);

    uint8_t example[] = {0x00,0x01,0xab,0xff};
    print_hex(example,sizeof(example));
    uint32_t a = 0x11111111;
    uint32_t b = 0x01020304;
    uint32_t c = 0x9b8d6f43;
    uint32_t d = 0x01234567;
    quarter_round(&a,&b,&c,&d);
    printf("a = %08x\n",a);
    printf("b = %08x\n",b);
    printf("c = %08x\n",c);
    printf("d = %08x\n",d);

    if (a == EXPECTED_A && b == EXPECTED_B && c == EXPECTED_C && d == EXPECTED_D)
    {
        printf("Passou no teste de Validação\n");
    }else
    {
        printf("Teste falhou\n");
    }
    
    return 0;
}
