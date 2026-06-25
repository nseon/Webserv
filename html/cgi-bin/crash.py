#!/usr/bin/env python3
# CGI demo (erreur) : ce script contient VOLONTAIREMENT une erreur de syntaxe.
# L'interpreteur python3 echoue au parsing (SyntaxError) et n'ecrit jamais
# d'en-tetes CGI valides sur stdout. Webserv ne peut donc pas construire de
# reponse a partir de la sortie CGI et renvoie une erreur 502 Bad Gateway.
import os

print("Content-Type: text/html; charset=utf-8\r")
print("\r")

# Erreur de syntaxe deliberee : parenthese jamais fermee + def invalide.
def broken(:
    return "cette ligne ne sera jamais executee"

print("<h1>Vous ne verrez jamais ceci</h1>"
