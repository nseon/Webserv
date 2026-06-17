#!/usr/bin/env python3
# CGI demo (GET) : lit la QUERY_STRING et renvoie une page personnalisee.
# Exemple d'URL : /cgi-bin/greet.py?name=Leo&color=tomato
import os
import html
from urllib.parse import parse_qs

query = os.environ.get("QUERY_STRING", "")
params = parse_qs(query)

name = params.get("name", ["visiteur"])[0]
color = params.get("color", ["#ffd166"])[0]

# On nettoie pour eviter toute injection HTML/CSS.
name = html.escape(name)[:40]
safe_color = "".join(c for c in color if c.isalnum() or c in "#(),.% ")[:30] or "#ffd166"

body = """<!DOCTYPE html>
<html lang="fr"><head><meta charset="UTF-8">
<title>CGI - Salutation</title>
<style>
body{{font-family:'Segoe UI',sans-serif;background:linear-gradient(135deg,#667eea,#764ba2);color:#fff;margin:0;height:100vh;display:flex;align-items:center;justify-content:center;text-align:center}}
.card{{background:rgba(255,255,255,.1);padding:50px;border-radius:16px;border:1px solid rgba(255,255,255,.2);box-shadow:0 8px 32px rgba(0,0,0,.3);max-width:480px}}
h1{{font-size:2.4rem;color:{color}}}
a{{color:#fff;display:inline-block;margin-top:25px}}
.q{{opacity:.7;font-size:.85rem;word-break:break-all}}
</style></head><body>
<div class="card">
<h1>Bonjour, {name} ! 👋</h1>
<p>Cette page a été générée dynamiquement par un script CGI Python à partir des paramètres GET.</p>
<p class="q">QUERY_STRING reçue : <code>{query}</code></p>
<a href="/pages/cgi.html">← Retour aux démos CGI</a>
</div></body></html>""".format(
    color=safe_color, name=name, query=html.escape(query) or "<em>aucune</em>"
)

print("Content-Type: text/html; charset=utf-8\r")
print("\r")
print(body)
