#!/usr/bin/env python3
# CGI de test - GET : renvoie la methode et la query string recue via l'env.
import os
import sys

method = os.environ.get("REQUEST_METHOD", "")
query = os.environ.get("QUERY_STRING", "")
path_info = os.environ.get("PATH_INFO", "")

body = "method=%s;query=%s;path_info=%s" % (method, query, path_info)

sys.stdout.write("Content-Type: text/plain\r\n")
sys.stdout.write("X-Cgi-Test: hello\r\n")
sys.stdout.write("\r\n")
sys.stdout.write(body)
sys.stdout.flush()
