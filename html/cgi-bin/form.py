#!/usr/bin/env python3
# CGI demo (POST) : formulaire de contact. Lit le corps POST
# (application/x-www-form-urlencoded) et renvoie un recapitulatif.
import os
import sys
import html
from urllib.parse import parse_qs


def read_body():
    # Lire exactement CONTENT_LENGTH octets (le socket ne recoit pas d'EOF
    # tant que le script ne se termine pas -> ne jamais boucler sur read()).
    try:
        length = int(os.environ.get("CONTENT_LENGTH", "0"))
    except ValueError:
        length = 0
    data = b""
    while len(data) < length:
        chunk = sys.stdin.buffer.read(length - len(data))
        if not chunk:
            break
        data += chunk
    return data.decode("utf-8", "replace")


params = parse_qs(read_body())
name = html.escape(params.get("name", [""])[0])[:60]
email = html.escape(params.get("email", [""])[0])[:80]
message = html.escape(params.get("message", [""])[0])[:500]

rows = ""
for label, value in (("Nom", name), ("Email", email), ("Message", message)):
    rows += "<tr><td class='k'>{}</td><td>{}</td></tr>".format(
        label, value if value else "<em>(vide)</em>"
    )

page = """<!DOCTYPE html>
<html lang="fr"><head><meta charset="UTF-8">
<title>CGI - Formulaire reçu (POST)</title>
<style>
body{{font-family:'Segoe UI',sans-serif;background:linear-gradient(135deg,#667eea,#764ba2);color:#fff;margin:0;padding:40px 20px;display:flex;flex-direction:column;align-items:center}}
.card{{background:rgba(255,255,255,.1);padding:40px;border-radius:16px;border:1px solid rgba(255,255,255,.2);box-shadow:0 8px 32px rgba(0,0,0,.3);max-width:560px;width:100%;box-sizing:border-box}}
table{{width:100%;border-collapse:collapse}}
td{{padding:10px 12px;border-bottom:1px solid rgba(255,255,255,.15);vertical-align:top;word-break:break-word}}
.k{{font-weight:bold;color:#ffe0a3;width:120px}}
a{{color:#fff;display:inline-block;margin-top:20px}}
</style></head><body>
<div class="card">
<h1>Message bien reçu ! ✅</h1>
<p>Le script <code>form.py</code> a traité votre envoi POST côté serveur :</p>
<table>{}</table>
<a href="/pages/cgi.html">← Retour aux démos CGI</a>
</div></body></html>""".format(rows)

print("Content-Type: text/html; charset=utf-8\r")
print("\r")
print(page)
