#!/usr/bin/env python3
# CGI de test : emet un header Status custom pour valider le parsing CGI.
import sys

sys.stdout.write("Status: 418 I am a teapot\r\n")
sys.stdout.write("Content-Type: text/plain\r\n")
sys.stdout.write("\r\n")
sys.stdout.write("teapot")
sys.stdout.flush()
