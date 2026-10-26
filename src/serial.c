/* ==============================================
   File    : serial.c
   Deskripsi: Implementasi Gaussian Blur 5x5 secara Serial (Baseline)
   Kelompok: Projek UTS
   Anggota :  
                Indira Azalia (2322105009)
                Nandita Auraria Bilqis (2322105012)
                Reva Mutiara Putri (2322105017)
                Alifa Syahira (2322105023)
                Sinta Mala (2322105046)
                Sulis Nurain (2322105047)
   ============================================== */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

const float KERNEL[5][5] = {
    {1/273.0,  4/273.0,  7/273.0,  4/273.0, 1/273.0},
    {4/273.0, 16/273.0, 26/273.0, 16/273.0, 4/273.0},
    {7/273.0, 26/273.0, 41/273.0, 26/273.0, 7/273.0},
    {4/273.0, 16/273.0, 26/273.0, 16/273.0, 4/273.0},
    {1/273.0,  4/273.0,  7/273.0,  4/273.0, 1/273.0}
};

unsigned char* read_pgm(const char* filename, int* width, int* height) {
    FILE* file = fopen(filename, "rb");
    if (!file) { printf("Gagal buka file input\n"); exit(1); }
    char magic[3]; int maxval;
    fscanf(file, "%2s", magic);
    fscanf(file, "%d %d", width, height);
    fscanf(file, "%d", &maxval);
    fgetc(file);
    unsigned char* data = (unsigned char*)malloc((*width) * (*height) * sizeof(unsigned char));
    fread(data, sizeof(unsigned char), (*width) * (*height), file);
    fclose(file);
    return data;
}

void write_pgm(const char* filename, unsigned char* data, int width, int height) {
    FILE* file = fopen(filename, "wb");
    fprintf(file, "P5\n%d %d\n255\n", width, height);
    fwrite(data, sizeof(unsigned char), width * height, file);
    fclose(file);
}

int main() {
    int width, height;
    printf("Membaca gambar...\n");
    unsigned char* input = read_pgm("input/gambar.pgm", &width, &height);
    unsigned char* output = (unsigned char*)malloc(width * height * sizeof(unsigned char));

    printf("Mulai proses Serial (Single Thread)...\n");
        clock_t start = clock();

    // --- TAMBAHKAN LOOPING UNTUK BLUR 3 KALI ---
    for (int pass = 0; pass < 3; pass++) {
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                float sum = 0.0;
                for (int ky = -2; ky <= 2; ky++) {
                    for (int kx = -2; kx <= 2; kx++) {
                        int ny = y + ky; int nx = x + kx;
                        if (ny < 0) ny = 0; if (ny >= height) ny = height - 1;
                        if (nx < 0) nx = 0; if (nx >= width) nx = width - 1;
                        sum += input[ny * width + nx] * KERNEL[ky + 2][kx + 2];
                    }
                }
                if (sum > 255) sum = 255; if (sum < 0) sum = 0;
                output[y * width + x] = (unsigned char)sum;
            }
        }
        // Copy hasil ke input biar bisa di-blur lagi di pass berikutnya
        for (int i = 0; i < width * height; i++) {
            input[i] = output[i];
        }
    }
    // -------------------------------------------

    clock_t end = clock();
    double time_spent = (double)(end - start) / CLOCKS_PER_SEC;
    printf("Waktu Serial: %f detik\n", time_spent);

    printf("Menyimpan gambar hasil...\n");
    write_pgm("output/hasil_serial.pgm", output, width, height);
        // Simpan waktu ke file teks
    FILE *f_waktu = fopen("waktu_serial.txt", "w");
    fprintf(f_waktu, "%.3f", time_spent);
    fclose(f_waktu);
    free(input); free(output);
    return 0;
}