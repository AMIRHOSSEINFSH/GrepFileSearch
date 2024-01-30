import os
import sys
import tkinter as tk
from tkinter import scrolledtext
from tkinter import filedialog
from pubsub import pub

sys.path.append(os.path.abspath(os.path.join(os.path.dirname(__file__), '..')))
from ui.connection import CLIENT_MESSAGE_TOPIC, startSearching

HEIGHT = 500
WIDTH = 600

socket: any

def on_Message_Event(data: str):
    current_text = scrollable_label.get("1.0", tk.END)
    updated_text = f"{current_text}\n{data}"
    scrollable_label.delete("1.0", tk.END)
    scrollable_label.insert(tk.END, updated_text)

    # Scroll to the top after inserting new text
    scrollable_label.yview(tk.MOVETO, 1.0)

    if data == 'exit' or data.startswith("Unable Connect to"):
        button.configure(state=tk.NORMAL)

def open_folder_picker():
    folder_path = filedialog.askdirectory()
    if folder_path:
        entry.config(state=tk.NORMAL)
        entry.delete(0, tk.END)
        entry.insert(0, folder_path)
        entry.config(state='readonly')
        startSearching(folder_path,new_entry.get())

root = tk.Tk()
pub.subscribe(on_Message_Event, CLIENT_MESSAGE_TOPIC)
canvas = tk.Canvas(root, height=HEIGHT, width=WIDTH)
canvas.pack()

background_image = tk.PhotoImage(file='landscape.png')
background_label = tk.Label(root, image=background_image)
background_label.place(relwidth=1, relheight=1)

frame = tk.Frame(root, bg='#ff1100', bd=5)
frame.place(relx=0.5, rely=0.1, relwidth=0.75, relheight=0.1, anchor='n')

entry = tk.Entry(frame, font=40, state='readonly')
entry.place(relx=0, relwidth=0.3, relheight=1)  # Adjusted relwidth

# New Entry Widget next to the existing entry
new_entry = tk.Entry(frame, font=40)
new_entry.place(relx=0.35, relwidth=0.3, relheight=1)  # Adjusted relx and relwidth

button = tk.Button(frame, text="Choose a Folder", font=40, command=lambda: open_folder_picker())
button.place(relx=0.7, relheight=1, relwidth=0.3)

lower_frame = tk.Frame(root, bg='#80c1ff', bd=10)
lower_frame.place(relx=0.5, rely=0.25, relwidth=0.75, relheight=0.6, anchor='n')

# Use ScrolledText instead of Label for scrollable text
scrollable_label = scrolledtext.ScrolledText(lower_frame, wrap=tk.WORD, font=("Courier", 10))
scrollable_label.place(relwidth=1, relheight=1)

root.mainloop()
