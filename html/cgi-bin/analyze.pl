#!/usr/bin/perl
# CGI demo (POST) en Perl : analyse un texte envoye en POST
# (application/x-www-form-urlencoded) et renvoie des statistiques.
use strict;
use warnings;
use utf8;

# IMPORTANT : Webserv ne ferme pas son cote ecriture du socket, il faut donc
# lire EXACTEMENT CONTENT_LENGTH octets sur STDIN (jamais une boucle jusqu'a EOF).
my $len = $ENV{'CONTENT_LENGTH'} || 0;
$len += 0;
my $body = '';
if ($len > 0) {
    my $got = 0;
    while ($got < $len) {
        my $n = read(STDIN, my $buf, $len - $got);
        last if !defined $n || $n == 0;
        $body .= $buf;
        $got += $n;
    }
}

# Decodage minimal du form-urlencoded.
sub url_decode {
    my $s = shift;
    $s =~ tr/+/ /;
    $s =~ s/%([0-9A-Fa-f]{2})/chr(hex($1))/ge;
    return $s;
}

my %params;
for my $pair (split /&/, $body) {
    my ($k, $v) = split /=/, $pair, 2;
    next unless defined $k;
    $v = '' unless defined $v;
    $params{ url_decode($k) } = url_decode($v);
}

my $text = $params{'text'} // '';

# Statistiques.
my $chars = length $text;
my $no_spaces = length($text =~ tr/ \t\n\r//dr);
my @words = grep { length } split /\s+/, $text;
my $word_count = scalar @words;
my $line_count = ($text =~ tr/\n//) + ($chars ? 1 : 0);

# Echappement HTML pour reafficher le texte sans danger.
sub esc {
    my $s = shift // '';
    $s =~ s/&/&amp;/g;
    $s =~ s/</&lt;/g;
    $s =~ s/>/&gt;/g;
    return $s;
}
my $preview = esc($text);
$preview = '<em>(texte vide)</em>' if $chars == 0;

my $rows = "";
for my $r (["Caractères (total)", $chars],
           ["Caractères (hors espaces)", $no_spaces],
           ["Mots", $word_count],
           ["Lignes", $line_count]) {
    $rows .= "<tr><td class='k'>$r->[0]</td><td>$r->[1]</td></tr>";
}

my $page = <<"HTML";
<!DOCTYPE html>
<html lang="fr"><head><meta charset="UTF-8">
<title>CGI Perl - Analyse de texte (POST)</title>
<style>
body{font-family:'Segoe UI',sans-serif;background:linear-gradient(135deg,#667eea,#764ba2);color:#fff;margin:0;padding:40px 20px;display:flex;flex-direction:column;align-items:center}
.card{background:rgba(255,255,255,.1);padding:40px;border-radius:16px;border:1px solid rgba(255,255,255,.2);box-shadow:0 8px 32px rgba(0,0,0,.3);max-width:560px;width:100%;box-sizing:border-box}
table{width:100%;border-collapse:collapse;margin-bottom:20px}
td{padding:10px 12px;border-bottom:1px solid rgba(255,255,255,.15)}
.k{font-weight:bold;color:#ffe0a3;width:60%}
.preview{background:rgba(0,0,0,.2);padding:12px;border-radius:8px;white-space:pre-wrap;word-break:break-word;font-size:.9rem}
a{color:#fff;display:inline-block;margin-top:20px}
.lang{display:inline-block;font-size:.75rem;font-weight:bold;padding:3px 10px;border-radius:20px;background:#7a52cc;margin-left:8px;vertical-align:middle}
</style></head><body>
<div class="card">
<h1>Analyse de texte <span class="lang">Perl</span></h1>
<p>Texte reçu en POST et analysé côté serveur par le script Perl <code>analyze.pl</code>.</p>
<table>$rows</table>
<div class="preview">$preview</div>
<a href="/pages/cgi.html">← Retour aux démos CGI</a>
</div></body></html>
HTML

print "Content-Type: text/html; charset=utf-8\r\n";
print "\r\n";
print $page;
