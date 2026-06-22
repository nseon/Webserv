#!/usr/bin/env python3
# CGI de test - timeout : dort longtemps pour declencher le timeout CGI.
import time
import sys

time.sleep(30)

sys.stdout.write("Content-Type: text/plain\r\n")
sys.stdout.write("\r\n")
sys.stdout.write("slow")
sys.stdout.flush()
