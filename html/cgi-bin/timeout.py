#!/usr/bin/env python3
# CGI demo (timeout) : ce script dort INDEFINIMENT sans jamais ecrire de reponse.
# Il sert a verifier que Webserv applique bien un timeout sur les CGI :
# le serveur doit tuer le processus et renvoyer une erreur (504/502) au lieu
# d'attendre eternellement.
import time

# On ne produit AUCUNE sortie : on dort, c'est tout.
while True:
    time.sleep(3600)
