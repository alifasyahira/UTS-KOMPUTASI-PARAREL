from PIL import Image
import os

def convert():
    print("Memulai konversi...")
    # Input diambil dari folder input, disimpan ke static
    if os.path.exists("input/gambar.pgm"):
        Image.open("input/gambar.pgm").save("static/input.png")
        print("- input.png berhasil dibuat")
        
    if os.path.exists("output/hasil_serial.pgm"):
        Image.open("output/hasil_serial.pgm").save("static/serial.png")
        print("- serial.png berhasil dibuat")
        
    if os.path.exists("output/hasil_Openmp.pgm"):
        Image.open("output/hasil_Openmp.pgm").save("static/openmp.png")
        print("- openmp.png berhasil dibuat")

    if os.path.exists("output/hasil_pthreads.pgm"):
        Image.open("output/hasil_pthreads.pgm").save("static/pthreads.png")
        print("- pthreads.png berhasil dibuat")    
    print("Konversi gambar ke static/ selesai!")

if __name__ == "__main__":
    convert()