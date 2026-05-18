#include "doctest.h"

#include <string>
#include <map>
#include "cgi/CGIHandler.hpp"

// ─────────────────────────────────────────────
//  Helper : construit une map d'extensions
// ─────────────────────────────────────────────
static std::map<std::string, std::string> make_cgi_map()
{
    std::map<std::string, std::string> m;
    m[".py"]  = "/usr/bin/python3";
    m[".php"] = "/usr/bin/php";
    m[".pl"]  = "/usr/bin/perl";
    m[".rb"]  = "/usr/bin/ruby";
    return m;
}

// ─────────────────────────────────────────────
//  TESTS
// ─────────────────────────────────────────────

// ── 1. Extension exacte en fin d'URI ─────────

TEST_CASE("extension CGI en fin d'URI")
{
    std::map<std::string, std::string> cfg = make_cgi_map();

    SUBCASE(".py")
        CHECK(isCgi("/cgi-bin/script.py", cfg) == true);

    SUBCASE(".php")
        CHECK(isCgi("/var/www/page.php", cfg) == true);

    SUBCASE(".pl")
        CHECK(isCgi("/scripts/run.pl", cfg) == true);

    SUBCASE(".rb")
        CHECK(isCgi("/app/handler.rb", cfg) == true);

    SUBCASE("extension seule, sans chemin")
        CHECK(isCgi(".py", cfg) == true);
}

// ── 2. PATH_INFO : '/' apres l'extension ─────

TEST_CASE("PATH_INFO present apres l'extension")
{
    std::map<std::string, std::string> cfg = make_cgi_map();

    SUBCASE(".py suivi d'un path simple")
        CHECK(isCgi("/cgi-bin/script.py/extra/path", cfg) == true);

    SUBCASE(".php suivi d'un seul segment")
        CHECK(isCgi("/page.php/info", cfg) == true);

    SUBCASE(".pl suivi de plusieurs segments")
        CHECK(isCgi("/run.pl/data/42", cfg) == true);
}

// ── 3. Extensions non CGI ────────────────────

TEST_CASE("extension absente de la map CGI")
{
    std::map<std::string, std::string> cfg = make_cgi_map();

    SUBCASE(".html")  CHECK(isCgi("/index.html",    cfg) == false);
    SUBCASE(".css")   CHECK(isCgi("/style.css",     cfg) == false);
    SUBCASE(".js")    CHECK(isCgi("/app.js",        cfg) == false);
    SUBCASE(".png")   CHECK(isCgi("/img/logo.png",  cfg) == false);
    SUBCASE("aucune extension") CHECK(isCgi("/api/v1/users", cfg) == false);
}

// ── 4. Extension presente dans le chemin mais mal positionnee ──

TEST_CASE("extension CGI dans le chemin mais pas en position terminale valide")
{
    std::map<std::string, std::string> cfg = make_cgi_map();

    SUBCASE("extension suivie d'un autre caractere que '/' ou fin")
        // rfind trouve .py mais le caractere suivant est '-', pas '/'
        CHECK(isCgi("/cgi-bin.py-archive/script", cfg) == false);

    SUBCASE("extension au milieu du nom de fichier")
        // .php trouve, suivi de '.', ni fin ni '/'
        CHECK(isCgi("/cgi/script.php.old", cfg) == false);

    SUBCASE("extension dans le repertoire, nom de fichier different")
        CHECK(isCgi("/dir.php.bak/file", cfg) == false);
}

// ── 5. Cas limites ───────────────────────────

TEST_CASE("cas limites")
{
    SUBCASE("URI vide, map pleine")
    {
        std::map<std::string, std::string> cfg = make_cgi_map();
        CHECK(isCgi("", cfg) == false);
    }

    SUBCASE("URI non vide, map vide")
    {
        std::map<std::string, std::string> cfg;
        CHECK(isCgi("/script.py", cfg) == false);
    }

    SUBCASE("plusieurs extensions dans la map, seule la seconde correspond")
    {
        std::map<std::string, std::string> cfg;
        cfg[".sh"]  = "/bin/sh";
        cfg[".php"] = "/usr/bin/php";
        CHECK(isCgi("/app/run.php", cfg) == true);
        CHECK(isCgi("/app/run.sh",  cfg) == true);
        CHECK(isCgi("/app/run.py",  cfg) == false);
    }
}
