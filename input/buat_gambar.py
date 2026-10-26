import random

def create_pgm(filename, width=2000, height=2000):
    print(f"Membuat gambar {width}x{height}...")
    with open(filename, 'wb') as f:
        header = f"P5\n{width} {height}\n255\n"
        f.write(header.encode('ascii'))
        pixels = bytearray([random.randint(0, 255) for _ in range(width * height)])
        f.write(pixels)
    print(f"Selesai: {filename}")

if __name__ == "__main__":
    create_pgm("gambar.pgm")