#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <omp.h>

#define KERNEL_SIZE 5

/* Kernel Gaussian 5x5 dengan bobot integer.
   Jumlah seluruh bobot adalah 273. */
const int kernel[KERNEL_SIZE][KERNEL_SIZE] = {
    {1,  4,  7,  4, 1},
    {4, 16, 26, 16, 4},
    {7, 26, 41, 26, 7},
    {4, 16, 26, 16, 4},
    {1,  4,  7,  4, 1}
};

#define KERNEL_SUM 273

/* Membaca token PGM sambil melewati komentar. */
int read_token(FILE *fp, char *token, int size) {
    int c;

    do {
        c = fgetc(fp);
        if (c == '#') {
            while (c != '\n' && c != EOF)
                c = fgetc(fp);
        }
    } while (c != EOF && c <= ' ');

    if (c == EOF) return 0;

    int i = 0;
    do {
        if (i < size - 1)
            token[i++] = (char)c;
        c = fgetc(fp);
    } while (c != EOF && c > ' ' && c != '#');

    if (c == '#') {
        while (c != '\n' && c != EOF)
            c = fgetc(fp);
    }

    token[i] = '\0';
    return 1;
}

/* Membaca file PGM format P2 atau P5. */
int read_pgm(const char *filename, int *w, int *h,
             int *maxval, unsigned char **pixels) {
    FILE *fp = fopen(filename, "rb");
    if (!fp) {
        perror("Gagal membuka input PGM");
        return 0;
    }

    char token[64];
    if (!read_token(fp, token, sizeof(token)) ||
        (strcmp(token, "P2") != 0 &&
         strcmp(token, "P5") != 0)) {
        fprintf(stderr, "Format PGM harus P2 atau P5.\n");
        fclose(fp);
        return 0;
    }

    int binary = strcmp(token, "P5") == 0;

    if (!read_token(fp, token, sizeof(token))) {
        fclose(fp);
        return 0;
    }
    *w = atoi(token);

    if (!read_token(fp, token, sizeof(token))) {
        fclose(fp);
        return 0;
    }
    *h = atoi(token);

    if (!read_token(fp, token, sizeof(token))) {
        fclose(fp);
        return 0;
    }
    *maxval = atoi(token);

    if (*w <= 0 || *h <= 0 || *maxval <= 0 ||
        *maxval > 255) {
        fprintf(stderr, "Dimensi atau maxval PGM tidak valid.\n");
        fclose(fp);
        return 0;
    }

    size_t count = (size_t)(*w) * (size_t)(*h);
    *pixels = malloc(count);
    if (!*pixels) {
        fprintf(stderr, "Memori tidak cukup.\n");
        fclose(fp);
        return 0;
    }

    if (binary) {
        if (fread(*pixels, 1, count, fp) != count) {
            fprintf(stderr, "Data piksel PGM tidak lengkap.\n");
            free(*pixels);
            fclose(fp);
            return 0;
        }
    } else {
        for (size_t i = 0; i < count; i++) {
            if (!read_token(fp, token, sizeof(token))) {
                fprintf(stderr, "Data piksel PGM tidak lengkap.\n");
                free(*pixels);
                fclose(fp);
                return 0;
            }
            int value = atoi(token);
            if (value < 0 || value > *maxval) {
                fprintf(stderr, "Nilai piksel tidak valid.\n");
                free(*pixels);
                fclose(fp);
                return 0;
            }
            (*pixels)[i] = (unsigned char)value;
        }
    }

    fclose(fp);
    return 1;
}

/* Menyimpan gambar grayscale sebagai PGM P5. */
int write_pgm(const char *filename, int w, int h,
              const unsigned char *pixels) {
    FILE *fp = fopen(filename, "wb");
    if (!fp) {
        perror("Gagal menyimpan output PGM");
        return 0;
    }

    fprintf(fp, "P5\n%d %d\n255\n", w, h);
    size_t count = (size_t)w * (size_t)h;
    int ok = fwrite(pixels, 1, count, fp) == count;
    fclose(fp);

    return ok;
}

/* Menghitung blur dengan replicate padding pada border. */
void gaussian_blur_openmp (const unsigned char *input,
                          unsigned char *output,
                          int w, int h) {
    #pragma omp parallel for schedule(static)
for (int y = 0; y < h; y++) {
    for (int x = 0; x < w; x++) {
            int sum = 0;

            for (int ky = -2; ky <= 2; ky++) {
                for (int kx = -2; kx <= 2; kx++) {
                    int yy = y + ky;
                    int xx = x + kx;

                    if (yy < 0) yy = 0;
                    if (yy >= h) yy = h - 1;
                    if (xx < 0) xx = 0;
                    if (xx >= w) xx = w - 1;

                    sum += kernel[ky + 2][kx + 2] *
                           input[(size_t)yy * w + xx];
                }
            }

            int value = (sum + KERNEL_SUM / 2) / KERNEL_SUM;
            if (value > 255) value = 255;

            output[(size_t)y * w + x] = (unsigned char)value;
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Penggunaan: %s input.pgm output.pgm\n", argv[0]);
        return 1;
    }

    int w, h, maxval;
    unsigned char *input = NULL;
    unsigned char *output = NULL;

    if (!read_pgm(argv[1], &w, &h, &maxval, &input))
        return 1;

    size_t count = (size_t)w * (size_t)h;
    output = malloc(count);
    if (!output) {
        fprintf(stderr, "Memori tidak cukup untuk output.\n");
        free(input);
        return 1;
    }

    double start = omp_get_wtime();
gaussian_blur_openmp(input, output, w, h);
double end = omp_get_wtime();

    if (!write_pgm(argv[2], w, h, output)) {
        free(input);
        free(output);
        return 1;
    }

    printf("=== GAUSSIAN BLUR 5x5 - OPENMP ===\n");
    printf("Resolusi : %d x %d\n", w, h);
    printf("Border   : Replicate\n");
    printf("Waktu    : %.6f detik\n",
           end - start);
    printf("Output   : %s\n", argv[2]);

    free(input);
    free(output);
    return 0;
}
