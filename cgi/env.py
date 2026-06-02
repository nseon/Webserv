#!/usr/bin/env python3
import os

print("Content-Type: text/plain")
print()
print("---- CGI environment ----")
for key in sorted(os.environ):
    print(key + "=" + os.environ[key])
