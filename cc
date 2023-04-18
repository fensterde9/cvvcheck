import re
from tkinter import ttk
from tkinter import messagebox
import requests
import json
from threading import Event, Thread
from tkinter import scrolledtext, Menu
from tkinter import scrolledtext
from tkinter.scrolledtext import ScrolledText
from selenium.webdriver.chrome.options import Options
import time
import threading
import tkinter as tk
paused = False
from selenium import webdriver
from selenium.common.exceptions import NoSuchElementException, TimeoutException
from selenium.webdriver.common.by import By
from selenium.webdriver.support import expected_conditions as EC
from selenium.webdriver.support.ui import WebDriverWait

def process_input():
    if not validate_input():
        return

has_auto_paused = False

pause_event = Event()
pause_event.set()  # 设置 pause_event 的初始状态为 True
ok_cards = []
no_cards = []
  # 添加一个标记，表示是否已经自动暂停过

def display_checking_status(card_number, dots_count):
    result_textbox.configure(state="normal")
    result_textbox.delete(f"{card_number}.0", f"{card_number}.end")
    result_textbox.insert(f"{card_number}.0", f"正在检测 {card_number} {'.' * dots_count}\n")
    result_textbox.configure(state="disabled")
    dots_count = (dots_count + 1) % 6
    root.after(500, display_checking_status, card_number, dots_count)

def input_textbox_modified(event):
    current_input = input_textbox.get("1.0", "end-1c")
    if len(current_input) > 0:
        submit_button.config(state="normal")
    else:
        submit_button.config(state="disabled")

def on_pause_resume():
    global pause_event
    if pause_event.is_set():
        pause_event.clear()
        pause_resume_button.config(text="暂停")
    else:
        pause_event.set()
        pause_resume_button.config(text="继续")

def check_credit_cards(cards):
    global ok_cards, no_cards
    for card in cards:
        pause_event.wait()
        card_status = check_card(card)
        if card_status:
            ok_cards.append(card)
        else:
            no_cards.append(card['card_number'])

def show_popup(event, textbox):
    popup_menu = Menu(root, tearoff=0)
    popup_menu.add_command(label="复制", command=lambda: textbox.event_generate("<<Copy>>"))
    popup_menu.add_command(label="粘贴", command=lambda: textbox.event_generate("<<Paste>>"))
    popup_menu.add_command(label="剪切", command=lambda: textbox.event_generate("<<Cut>>"))
    popup_menu.add_command(label="删除", command=lambda: textbox.delete("sel.first", "sel.last"))
    popup_menu.add_separator()
    popup_menu.add_command(label="全选", command=lambda: textbox.tag_add("sel", "1.0", "end-1c"))

    try:
        popup_menu.tk.call("tk_popup", popup_menu, event.x_root, event.y_root)
    finally:
        popup_menu.grab_release()
        
#以下是识别并提取卡号安全码 到期日
