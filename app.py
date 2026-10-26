# ==============================================
# File    : app.py
# Deskripsi: Web Server Flask untuk visualisasi hasil konvolusi
# Kelompok: Projek UTS             
# ==============================================
from flask import Flask, render_template_string, request
import os
import time
import base64
from PIL import Image, ImageFile

ImageFile.LOAD_TRUNCATED_IMAGES = True
app = Flask(__name__)

HTML = """
<!DOCTYPE html>
<html lang="id">
<head>
    <meta charset="UTF-8">
    <title>Hasil UTS: Image Convolution</title>
    <style>
        @import url('https://fonts.googleapis.com/css2?family=Playfair+Display:ital,wght@0,600;1,600&family=Quicksand:wght@400;600&display=swap');
        * { box-sizing: border-box; margin: 0; padding: 0; }
        body { background-color: #fdf1f5; background-image: radial-gradient(#f8d4e1 1px, transparent 1px); background-size: 20px 20px; color: #5a3a45; font-family: 'Quicksand', sans-serif; padding: 40px 20px; }
        .container { max-width: 1100px; margin: 0 auto; }
        header { text-align: center; margin-bottom: 40px; padding: 30px; background: #ffffff; border: 2px dashed #f4a7c1; border-radius: 20px; box-shadow: 0 10px 20px rgba(244, 167, 193, 0.15); position: relative; }
        header::before { content: ; font-size: 2rem; position: absolute; top: -20px; left: 50%; transform: translateX(-50%); background: #fff; padding: 0 10px; }
        header h1 { font-family: 'Playfair Display', serif; color: #d14b7c; font-size: 2.5rem; margin-bottom: 10px; }
        header p { font-size: 1.1rem; color: #9e6b7b; font-style: italic; }
        .upload-section { margin-top: 25px; padding: 20px; background-color: #fff0f5; border-radius: 15px; display: inline-block; border: 2px solid #f8d4e1; }
        .upload-section button { background-color: #d14b7c; color: white; border: none; padding: 12px 24px; border-radius: 25px; cursor: pointer; font-weight: 600; font-size: 1rem; margin-top: 10px; }
        
        /* CSS UNTUK 4 GAMBAR (Supaya muat berdampingan) */
        .image-section { display: flex; flex-wrap: wrap; gap: 15px; margin-bottom: 40px; justify-content: center; }
        .card { flex: 1 1 200px; background: #ffffff; padding: 15px; border-radius: 20px; box-shadow: 0 8px 16px rgba(209, 75, 124, 0.1); text-align: center; border: 1px solid #f8d4e1; }
        .card h2 { font-family: 'Playfair Display', serif; font-size: 1.1rem; margin-bottom: 10px; color: #d14b7c; }
        
        /* CSS UNTUK ZOOM GAMBAR */
        .card img { width: 100%; height: auto; border-radius: 12px; border: 4px solid #fdf1f5; transition: transform 0.3s ease; cursor: zoom-in; }
        .card img:hover { transform: scale(2.5); position: relative; z-index: 10; box-shadow: 0 10px 20px rgba(0,0,0,0.3); }
        
        .stats-section { background: #ffffff; padding: 30px; border-radius: 20px; border: 2px dashed #f4a7c1; }
        .stats-section h2 { font-family: 'Playfair Display', serif; text-align: center; color: #d14b7c; margin-bottom: 20px; }
        table { width: 100%; border-collapse: collapse; background-color: #fffafc; border-radius: 10px; overflow: hidden; }
        th, td { padding: 15px; text-align: center; border-bottom: 1px solid #f8d4e1; font-size: 1.05rem; }
        th { background-color: #f4a7c1; color: white; }
        .speedup { font-weight: bold; color: #d14b7c; }
    </style>
</head>
<body>
    <div class="container">
        <header>
            <h1>Gaussian Blur 5x5 - Image Convolution</h1>
            <p>Perbandingan Hasil dan Performa (Serial vs Pthreads vs OpenMP)</p>
            <div class="upload-section">
                <form action="/" method="POST" enctype="multipart/form-data">
                    <input type="file" name="foto" accept="image/*" required><br>
                    <button type="submit">Upload Foto </button>
                </form>
            </div>
        </header>

        {% if has_result %}
        <div class="image-section">
            <div class="card"><h2>Gambar Asli</h2><img src="data:image/png;base64,{{ img_input }}"></div>
            <div class="card"><h2>Hasil Serial</h2><img src="data:image/png;base64,{{ img_serial }}"></div>
            <div class="card"><h2>Hasil Pthreads</h2><img src="data:image/png;base64,{{ img_pthreads }}"></div>
            <div class="card"><h2>Hasil OpenMP</h2><img src="data:image/png;base64,{{ img_openmp }}"></div>
        </div>
        
        <div class="stats-section">
            <h2>Perbandingan Waktu Eksekusi</h2>
            <table>
                <thead><tr><th>Metode</th><th>Waktu (Detik)</th><th>Speedup</th></tr></thead>
                <tbody>
                    <tr><td>Serial (1 Thread)</td><td>{{ w_serial }}</td><td>1.00x</td></tr>
                    <tr><td>Pthreads (Manual)</td><td>{{ w_pthreads }}</td><td class="speedup">{{ speedup_p }}x lebih cepat</td></tr>
                    <tr><td>OpenMP (Paralel)</td><td>{{ w_openmp }}</td><td class="speedup">{{ speedup_o }}x lebih cepat</td></tr>
                </tbody>
            </table>
        </div>
        {% else %}
        <div style="text-align: center; padding: 50px; color: #888;">
            <h3>Belum ada gambar yang diproses.</h3>
            <p>Silakan upload foto di atas.</p>
        </div>
        {% endif %}
    </div>
</body>
</html>
"""

def get_base64(filepath):
    if os.path.exists(filepath):
        with open(filepath, "rb") as f:
            return base64.b64encode(f.read()).decode('utf-8')
    return ""

@app.route('/', methods=['GET', 'POST'])
def index():
    w_serial = "0.000"
    w_pthreads = "0.000"
    w_openmp = "0.000"
    
    if request.method == 'POST':
        file = request.files['foto']
        if file and file.filename != '':
            if not os.path.exists('temp'): os.makedirs('temp')
            temp_path = 'temp/upload.png'
            file.save(temp_path)
            
            img = Image.open(temp_path).convert('L')
            img = img.resize((2000, 2000)) 
            img.save('input/gambar.pgm')
            
            # --- HITUNG WAKTU 3 PROGRAM C ---
            start_s = time.time()
            os.system("serial.exe > nul")
            w_serial = str(round(time.time() - start_s, 3))

            start_p = time.time()
            os.system("pthreads.exe > nul")
            w_pthreads = str(round(time.time() - start_p, 3))

            start_o = time.time()
            os.system("openmp.exe > nul")
            w_openmp = str(round(time.time() - start_o, 3))

            # Konversi ke PNG
            os.system("python to_png.py > nul")

    # Hitung speedup otomatis
    speedup_p = "0.00"
    speedup_o = "0.00"
    try:
        if float(w_pthreads) > 0: speedup_p = f"{float(w_serial) / float(w_pthreads):.2f}"
        if float(w_openmp) > 0: speedup_o = f"{float(w_serial) / float(w_openmp):.2f}"
    except:
        pass

    # Tampilkan Hasil
    has_result = os.path.exists('static/serial.png') and os.path.exists('static/openmp.png')
    
    return render_template_string(HTML, 
                                  has_result=has_result,
                                  img_input=get_base64('static/input.png'),
                                  img_serial=get_base64('static/serial.png'),
                                  img_pthreads=get_base64('static/pthreads.png'),
                                  img_openmp=get_base64('static/openmp.png'),
                                  w_serial=w_serial,
                                  w_pthreads=w_pthreads,
                                  w_openmp=w_openmp,
                                  speedup_p=speedup_p,
                                  speedup_o=speedup_o)

if __name__ == '__main__':
    os.makedirs('temp', exist_ok=True)
    
    # === KODE PEMBERSIH: Hapus gambar lama saat server baru nyala ===
    files_to_clear = ['static/input.png', 'static/serial.png', 'static/pthreads.png', 'static/openmp.png']
    for f in files_to_clear:
        if os.path.exists(f):
            os.remove(f)
    print("Cache gambar lama berhasil dibersihkan.")
    # ================================================================

    print("=== SERVER BERJALAN ===")
    print("Buka http://127.0.0.1:5000")
    app.run(debug=True)