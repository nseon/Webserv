#!/usr/bin/env python3
# CGI demo (GET) : affiche les variables d'environnement CGI/1.1
# transmises par Webserv au script, plus la query string brute.
import os
import html

# Variables CGI standard que Webserv renseigne (voir cgi.cpp)
cgi_vars = [
    "REQUEST_METHOD", "QUERY_STRING", "CONTENT_TYPE", "CONTENT_LENGTH",
    "GATEWAY_INTERFACE", "SERVER_PROTOCOL", "SERVER_SOFTWARE",
    "SERVER_NAME", "SERVER_PORT", "SCRIPT_NAME", "PATH_INFO",
    "REMOTE_ADDR", "AUTH_TYPE",
]

rows = ""
for name in cgi_vars:
    value = os.environ.get(name, "")
    rows += "<tr><td class='k'>{}</td><td class='v'>{}</td></tr>".format(
        html.escape(name), html.escape(value) if value else "<em>(vide)</em>"
    )

body = """<!DOCTYPE html>
<html lang="fr"><head><meta charset="UTF-8">
<title>CGI - Variables d'environnement</title>
<style>
body{{font-family:'Segoe UI',sans-serif;background:linear-gradient(135deg,#667eea,#764ba2);color:#fff;margin:0;padding:40px 20px;display:flex;flex-direction:column;align-items:center}}
.card{{background:rgba(255,255,255,.1);padding:40px;border-radius:16px;border:1px solid rgba(255,255,255,.2);box-shadow:0 8px 32px rgba(0,0,0,.3);max-width:640px;width:100%;box-sizing:border-box}}
h1{{margin-top:0}}
table{{width:100%;border-collapse:collapse}}
td{{padding:8px 12px;border-bottom:1px solid rgba(255,255,255,.15);font-size:.95rem;word-break:break-all}}
.k{{font-weight:bold;color:#ffe0a3;width:200px}}
a{{color:#fff;display:inline-block;margin-top:20px}}
</style></head><body>
<div class="card">
<h1>Variables d'environnement CGI 🌱</h1>
<p>Ce script Python (<code>env.py</code>) a été exécuté par Webserv via <strong>{}</strong>. Voici l'environnement qu'il a reçu :</p>
<table>{}</table>
<a href="/html/pages/cgi.html">← Retour aux démos CGI</a>
</div></body></html>""".format(
    html.escape(os.environ.get("REQUEST_METHOD", "?")), rows
)

print("Content-Type: text/html; charset=utf-8\r")
print("\r")
print(body)
