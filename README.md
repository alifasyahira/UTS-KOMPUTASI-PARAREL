# Tugas UTS Pemrosesan Paralel - Image Convolution (Opsi B)

**Kelompok: [Nomor Kelompok]**

**Anggota:**
1. Indira Azalia - 2322105009
2. Nandita Auraria Bilqis - 2322105012
3. Reva Mutiara Putri - 2322105017
4. Alifa Syahira - 2322105023 (Ketua)
5. Sinta Mala - 2322105046
6. Sulis Nurain - 2322105047

## Deskripsi
Implementasi filter Gaussian Blur 5x5 pada citra grayscale 2000x2000 pixel dengan tiga pendekatan:
1. **Serial** (Baseline, 1 Thread)
2. **Pthreads** (Manual Threading)
3. **OpenMP** (Compiler Directive)

Hasil pengujian divisualisasikan melalui **Web View** interaktif berbasis Flask (Python).

## Cara Compile
Pastikan GCC dan OpenMP sudah terinstall. Jalankan:
```bash
make
Cara Menjalankan (via Terminal)
1.Siapkan gambar input:
python input/buat_gambar.py

Jalankan program C:
./serial.exe
./pthreads.exe
./openmp.exe

Cara Menjalankan (via Web View)
Install dependensi Python:
pip install flask pillow
Jalankan server:
python app.py
Buka browser ke http://127.0.0.1:5000.
