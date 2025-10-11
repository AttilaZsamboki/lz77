import subprocess
import tkinter as tk
from tkinter import filedialog, messagebox

def encode_file():
    infile = filedialog.askopenfilename(title="Select file to encode")
    if not infile: return
    outfile = filedialog.asksaveasfilename(defaultextension=".lz77", title="Save encoded file as")
    if not outfile: return
    try:
        subprocess.run(["encoder.exe", infile, outfile], check=True)
        messagebox.showinfo("Success", f"Encoded file saved to:\n{outfile}")
    except subprocess.CalledProcessError:
        messagebox.showerror("Error", "Encoding failed!")

def decode_file():
    infile = filedialog.askopenfilename(filetypes=[("LZ77 files", "*.lz77")], title="Select .lz77 file")
    if not infile: return
    outfile = filedialog.asksaveasfilename(defaultextension=".txt", title="Save decoded file as")
    if not outfile: return
    try:
        subprocess.run(["decoder.exe", infile, outfile], check=True)
        messagebox.showinfo("Success", f"Decoded file saved to:\n{outfile}")
    except subprocess.CalledProcessError:
        messagebox.showerror("Error", "Decoding failed!")

root = tk.Tk()
root.title("LZ77 Encoder/Decoder")

tk.Button(root, text="Encode File", width=25, command=encode_file).pack(padx=10, pady=10)
tk.Button(root, text="Decode File", width=25, command=decode_file).pack(padx=10, pady=10)
tk.Button(root, text="Exit", width=25, command=root.quit).pack(padx=10, pady=10)

root.mainloop()
