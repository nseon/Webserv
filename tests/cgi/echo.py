#!/usr/bin/env python3
# CGI de test - POST : lit CONTENT_LENGTH octets sur stdin et les renvoie.
import os
import sys

try:
    length = int(os.environ.get("CONTENT_LENGTH", "") or "0")
except ValueError:
    length = 0

if length > 0:
    data = sys.stdin.read(length)
else:
    data = sys.stdin.read()

sys.stdout.write("Content-Type: text/plain\r\n")
sys.stdout.write("\r\n")
sys.stdout.write(data)
sys.stdout.flush()
