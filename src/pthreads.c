/* ==============================================
   File    : pthteads.c
   Deskripsi: Implementasi Gaussian Blur 5x5 menggunakan Manual Threading 
              (Pembagian workload per-blok baris dengan OpenMP Sections)
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
#include <omp.h> // Pakai OpenMP untuk bikin thread manual

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

void process_part(int start_y, int end_y, int width, int height, unsigned char* input, unsigned char* output) {
    for (int y = start_y; y < end_y; y++) {
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
}

int main() {
    int width, height;
    printf("Membaca gambar...\n");
    unsigned char* input = read_pgm("input/gambar.pgm", &width, &height);
    unsigned char* output = (unsigned char*)malloc(width * height * sizeof(unsigned char));

    printf("Mulai proses Threads (Manual)...\n");
    clock_t start = clock();

    // Membagi gambar jadi 4 bagian secara manual
    int part1 = height / 4;
    int part2 = height / 2;
    int part3 = (height * 3) / 4;
    int part4 = height;

    // Bikin 4 Thread Manual menggunakan OpenMP Sections
    #pragma omp parallel sections num_threads(4)
    {
        #pragma omp section
        { process_part(0, part1, width, height, input, output); }
        
        #pragma omp section
        { process_part(part1, part2, width, height, input, output); }
        
        #pragma omp section
        { process_part(part2, part3, width, height, input, output); }
        
        #pragma omp section
        { process_part(part3, part4, width, height, input, output); }
    }

    clock_t end = clock();
    double time_spent = (double)(end - start) / CLOCKS_PER_SEC;
    
    write_pgm("output/hasil_pthreads.pgm", output, width, height);
    
    FILE *f = fopen("waktu_pthreads.txt", "w");
    if (f) { fprintf(f, "%.3f", time_spent); fclose(f); }

    printf("Waktu Pthreads: %f\n", time_spent);

    free(input); free(output);
    return 0;
}