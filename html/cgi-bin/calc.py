#!/usr/bin/env python3
# CGI demo (POST) : calculatrice. Lit un corps application/x-www-form-urlencoded
# sur stdin (exactement CONTENT_LENGTH octets), calcule et renvoie le resultat.
import os
import sys
import html
from urllib.parse import parse_qs


def read_body():
    # IMPORTANT : Webserv ne ferme pas son cote ecriture du socket,
    # il faut donc lire EXACTEMENT CONTENT_LENGTH octets, jamais read() seul.
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


body_in = read_body()
params = parse_qs(body_in)

a_raw = params.get("a", ["0"])[0]
b_raw = params.get("b", ["0"])[0]
op = params.get("op", ["+"])[0]

result = ""
error = ""
try:
    a = float(a_raw)
    b = float(b_raw)
    if op == "+":
        result = a + b
    elif op == "-":
        result = a - b
    elif op == "*":
        result = a * b
    elif op == "/":
        result = "Division par zéro" if b == 0 else a / b
    else:
        error = "Opérateur inconnu : {}".format(html.escape(op))
    if isinstance(result, float) and result.is_integer():
        result = int(result)
except ValueError:
    error = "Entrées invalides (nombres attendus)."

display = error if error else "{} {} {} = <strong>{}</strong>".format(
    html.escape(a_raw), html.escape(op), html.escape(b_raw), result
)

page = """<!DOCTYPE html>
<html lang="fr"><head><meta charset="UTF-8">
<title>CGI - Calculatrice (POST)</title>
<style>
body{{font-family:'Segoe UI',sans-serif;background:linear-gradient(135deg,#667eea,#764ba2);color:#fff;margin:0;height:100vh;display:flex;align-items:center;justify-content:center;text-align:center}}
.card{{background:rgba(255,255,255,.1);padding:50px;border-radius:16px;border:1px solid rgba(255,255,255,.2);box-shadow:0 8px 32px rgba(0,0,0,.3);max-width:480px}}
.res{{font-size:1.8rem;margin:20px 0;color:#a3ffd1}}
a{{color:#fff;display:inline-block;margin-top:15px}}
</style></head><body>
<div class="card">
<h1>Calculatrice CGI 🧮</h1>
<p>Calcul effectué côté serveur par <code>calc.py</code> à partir du corps POST.</p>
<div class="res">{}</div>
<a href="/pages/cgi.html">← Retour aux démos CGI</a>
</div></body></html>""".format(display)

print("Content-Type: text/html; charset=utf-8\r")
print("\r")
print(page)
