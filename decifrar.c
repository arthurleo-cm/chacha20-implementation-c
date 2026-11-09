
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>

static uint32_t rotl32(uint32_t value, unsigned int shift)
{
    return (value << shift) | (value >> (32 - shift));
}

void quarter_round(uint32_t *a, uint32_t *b, uint32_t *c, uint32_t *d)
{
    *a = *a + *b;
    *d = *d ^ *a;
    *d = rotl32(*d, 16);

    *c = *c + *d;
    *b = *b ^ *c;
    *b = rotl32(*b, 12);

    *a = *a + *b;
    *d = *d ^ *a;
    *d = rotl32(*d, 8);

    *c = *c + *d;
    *b = *b ^ *c;
    *b = rotl32(*b, 7);
}

uint32_t load32(const uint8_t *p)
{
    return (uint32_t)p[0] |
           (uint32_t)p[1] << 8 |
           (uint32_t)p[2] << 16 |
           (uint32_t)p[3] << 24;
}

void chacha20(uint8_t *dados, size_t tamanho,
              const uint8_t chave[32], const uint8_t nonce[12])
{
    uint32_t estado[16] = {
        0x61707865, 0x3320646e,
        0x79622d32, 0x6b206574
    };

    for (int i = 0; i < 8; i++)
        estado[4 + i] = load32(chave + i * 4);

    estado[12] = 1;

    for (int i = 0; i < 3; i++)
        estado[13 + i] = load32(nonce + i * 4);

    for (size_t pos = 0; pos < tamanho; pos += 64)
    {
        uint32_t x[16];

        for (int i = 0; i < 16; i++)
            x[i] = estado[i];

        for (int i = 0; i < 10; i++)
        {
            quarter_round(&x[0], &x[4], &x[8], &x[12]);
            quarter_round(&x[1], &x[5], &x[9], &x[13]);
            quarter_round(&x[2], &x[6], &x[10], &x[14]);
            quarter_round(&x[3], &x[7], &x[11], &x[15]);

            quarter_round(&x[0], &x[5], &x[10], &x[15]);
            quarter_round(&x[1], &x[6], &x[11], &x[12]);
            quarter_round(&x[2], &x[7], &x[8], &x[13]);
            quarter_round(&x[3], &x[4], &x[9], &x[14]);
        }

        for (int i = 0; i < 16; i++)
        {
            uint32_t resultado = x[i] + estado[i];

            for (int j = 0; j < 4; j++)
            {
                size_t indice = pos + i * 4 + j;

                if (indice < tamanho)
                    dados[indice] ^= (uint8_t)(resultado >> (j * 8));
            }
        }

        estado[12]++;
    }
}

int main()
{
    uint8_t chave[32];
    uint8_t nonce[12];
    uint8_t mensagem[256];

    // Mesma chave utilizada na cifragem
    for (int i = 0; i < 32; i++)
        chave[i] = i;

    // Abre o arquivo cifrado
    FILE *arquivo = fopen("mensagem.bin", "rb");

    if (arquivo == NULL)
    {
        printf("Arquivo mensagem.bin nao encontrado.\n");
        return 1;
    }

    // Le o nonce que foi salvo no arquivo
    if (fread(nonce, 1, 12, arquivo) != 12)
    {
        printf("Arquivo invalido.\n");
        fclose(arquivo);
        return 1;
    }

    // Le a mensagem cifrada
    size_t tamanho = fread(mensagem, 1, 255, arquivo);

    if (fgetc(arquivo) != EOF || ferror(arquivo))
    {
        printf("Arquivo muito grande ou invalido.\n");
        fclose(arquivo);
        return 1;
    }

    fclose(arquivo);

    // Decifra a mensagem
    chacha20(mensagem, tamanho, chave, nonce);

    mensagem[tamanho] = '\0';

    printf("Mensagem decifrada: %s\n", mensagem);

    return 0;
}
