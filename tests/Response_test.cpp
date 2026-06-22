// =============================================================================
// Response_test.cpp
// Feature : construction des reponses HTTP (Response)
//
// Couvre :
//   - GET d'un fichier existant / absent / methode interdite / path traversal
//   - DELETE d'une ressource
//   - POST (upload) d'une ressource
//   - redirection (return)
//   - helpers statiques (getStatusMessage, getContentType)
//   - parsing de sortie CGI (buildFromCgiOutput)
//   - serialisation toString()
// =============================================================================

#include "doctest.h"

#include "http/Request.hpp"
#include "http/Response.hpp"
#include "config/Server.hpp"
#include "config/Location.hpp"

#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <string>
#include <unistd.h>
#include <sys/stat.h>

// ---------------------------------------------------------------------------
// Fixture : un repertoire temporaire unique, nettoye a la destruction.
// ---------------------------------------------------------------------------
struct TmpDir
{
	std::string	path;

	TmpDir()
	{
		char tmpl[] = "/tmp/webserv_resp_XXXXXX";
		char* p = mkdtemp(tmpl);
		if (p)
			path = p;
	}
	~TmpDir()
	{
		// Best-effort : on supprime via rm -rf (fixtures de test uniquement).
		if (!path.empty())
			std::system(("rm -rf " + path).c_str());
	}

	std::string write_file(std::string const& name, std::string const& content) const
	{
		std::string full = path + "/" + name;
		std::ofstream f(full.c_str(), std::ios::binary);
		f << content;
		f.close();
		return (full);
	}
};

// Construit un Request pret a l'emploi (sans passer par le reseau).
static void fill_request(Request& req, Server& server,
	std::string const& method, std::string const& path)
{
	req.setServer(&server);
	req.setVersion("HTTP/1.1");
	req.setMethod(method);
	req.setPath(path);
}

// =============================================================================
TEST_SUITE("Response — GET")
{
	TEST_CASE("GET d'un fichier existant renvoie 200 et son contenu")
	{
		TmpDir	tmp;
		tmp.write_file("hello.txt", "bonjour");

		Server		server;
		Location	location(server);
		location.setRoot(tmp.path);
		location.setMethods("GET");

		Request	request;
		fill_request(request, server, "GET", "/hello.txt");

		Response	response(request, &location);

		CHECK(response.getStatusCode() == 200);
		std::vector<char> const& body = response.getBody();
		CHECK(std::string(body.begin(), body.end()) == "bonjour");
	}

	TEST_CASE("GET d'un fichier absent renvoie 404")
	{
		TmpDir	tmp;

		Server		server;
		Location	location(server);
		location.setRoot(tmp.path);
		location.setMethods("GET");

		Request	request;
		fill_request(request, server, "GET", "/missing.txt");

		Response	response(request, &location);
		CHECK(response.getStatusCode() == 404);
	}

	TEST_CASE("GET sur une location sans GET renvoie 405")
	{
		TmpDir	tmp;
		tmp.write_file("hello.txt", "bonjour");

		Server		server;
		Location	location(server);
		location.setRoot(tmp.path);
		location.setMethods("POST");

		Request	request;
		fill_request(request, server, "GET", "/hello.txt");

		Response	response(request, &location);
		CHECK(response.getStatusCode() == 405);
	}

	TEST_CASE("GET avec path traversal '..' renvoie 400")
	{
		Server		server;
		Location	location(server);
		location.setRoot("/tmp");
		location.setMethods("GET");

		Request	request;
		fill_request(request, server, "GET", "/../etc/passwd");

		Response	response(request, &location);
		CHECK(response.getStatusCode() == 400);
	}

	TEST_CASE("Location NULL renvoie 404")
	{
		Server	server;
		Request	request;
		fill_request(request, server, "GET", "/whatever");

		Response	response(request, NULL);
		CHECK(response.getStatusCode() == 404);
	}
}

// =============================================================================
TEST_SUITE("Response — DELETE")
{
	TEST_CASE("DELETE d'un fichier existant renvoie 204 et le supprime")
	{
		TmpDir	tmp;
		std::string file = tmp.write_file("todelete.txt", "x");

		Server		server;
		Location	location(server);
		location.setMethods("DELETE");
		location.setUploadStore(tmp.path);

		Request	request;
		fill_request(request, server, "DELETE", "/todelete.txt");

		Response	response(request, &location);
		CHECK(response.getStatusCode() == 204);
		struct stat st;
		CHECK(stat(file.c_str(), &st) != 0);
	}

	TEST_CASE("DELETE d'un fichier absent renvoie 404")
	{
		TmpDir	tmp;

		Server		server;
		Location	location(server);
		location.setMethods("DELETE");
		location.setUploadStore(tmp.path);

		Request	request;
		fill_request(request, server, "DELETE", "/missing.txt");

		Response	response(request, &location);
		CHECK(response.getStatusCode() == 404);
	}

	TEST_CASE("DELETE sans autorisation renvoie 405")
	{
		TmpDir	tmp;

		Server		server;
		Location	location(server);
		location.setMethods("GET");
		location.setUploadStore(tmp.path);

		Request	request;
		fill_request(request, server, "DELETE", "/x.txt");

		Response	response(request, &location);
		CHECK(response.getStatusCode() == 405);
	}
}

// =============================================================================
TEST_SUITE("Response — POST")
{
	TEST_CASE("POST ecrit le corps dans upload_store et renvoie 201")
	{
		TmpDir	tmp;

		Server		server;
		Location	location(server);
		location.setMethods("POST");
		location.setUploadStore(tmp.path);

		// On parse une vraie requete pour disposer d'un corps.
		Request	request;
		request.setServer(&server);
		bool done = request.parseRequest(
			"POST /uploaded.txt HTTP/1.1\r\ncontent-length: 5\r\n\r\nhello");
		REQUIRE(done == true);

		Response	response(request, &location);
		CHECK(response.getStatusCode() == 201);

		std::ifstream f((tmp.path + "/uploaded.txt").c_str(), std::ios::binary);
		REQUIRE(f.good());
		std::string content((std::istreambuf_iterator<char>(f)),
			std::istreambuf_iterator<char>());
		CHECK(content == "hello");
	}

	TEST_CASE("POST sans upload_store renvoie 403")
	{
		Server		server;
		Location	location(server);
		location.setMethods("POST");

		Request	request;
		fill_request(request, server, "POST", "/x.txt");

		Response	response(request, &location);
		CHECK(response.getStatusCode() == 403);
	}
}

// =============================================================================
TEST_SUITE("Response — redirection")
{
	TEST_CASE("Une location avec 'return' produit une redirection")
	{
		Server		server;
		Location	location(server);
		location.setMethods("GET");
		location.setReturn("301 /new-location");

		Request	request;
		fill_request(request, server, "GET", "/old");

		Response	response(request, &location);
		CHECK(response.getStatusCode() == 301);

		std::map<std::string, std::string> const& h = response.getHeaders();
		REQUIRE(h.find("Location") != h.end());
		CHECK(h.find("Location")->second == "/new-location");
	}
}

// =============================================================================
TEST_SUITE("Response — helpers statiques")
{
	TEST_CASE("getStatusMessage couvre les codes usuels")
	{
		CHECK(Response::getStatusMessage(200) == "OK");
		CHECK(Response::getStatusMessage(404) == "Not Found");
		CHECK(Response::getStatusMessage(405) == "Method Not Allowed");
		CHECK(Response::getStatusMessage(500) == "Internal Server Error");
		CHECK(Response::getStatusMessage(999) == "Unknown Status");
	}

	TEST_CASE("getContentType deduit le type depuis l'extension")
	{
		CHECK(Response::getContentType("page.html") == "text/html");
		CHECK(Response::getContentType("style.css") == "text/css");
		CHECK(Response::getContentType("logo.png") == "image/png");
		CHECK(Response::getContentType("data.bin") == "text/plain");
		CHECK(Response::getContentType("noext") == "text/html");
	}
}

// =============================================================================
TEST_SUITE("Response — sortie CGI")
{
	TEST_CASE("buildFromCgiOutput parse headers et corps (CRLF)")
	{
		Server		server;
		Location	location(server);
		Response	response(location, "HTTP/1.1");

		std::string raw = "Content-Type: text/html\r\n\r\n<h1>hi</h1>";
		std::vector<char>	v(raw.begin(), raw.end());

		CHECK(response.buildFromCgiOutput(v) == true);
		CHECK(response.getStatusCode() == 200);
		std::vector<char> const& body = response.getBody();
		CHECK(std::string(body.begin(), body.end()) == "<h1>hi</h1>");

		std::map<std::string, std::string> const& h = response.getHeaders();
		REQUIRE(h.find("Content-Type") != h.end());
		CHECK(h.find("Content-Type")->second == "text/html");
	}

	TEST_CASE("buildFromCgiOutput honore le header Status")
	{
		Server		server;
		Location	location(server);
		Response	response(location, "HTTP/1.1");

		std::string raw = "Status: 404 Not Found\r\nContent-Type: text/plain\r\n\r\nnope";
		std::vector<char>	v(raw.begin(), raw.end());

		CHECK(response.buildFromCgiOutput(v) == true);
		CHECK(response.getStatusCode() == 404);
	}

	TEST_CASE("buildFromCgiOutput echoue sur une sortie vide")
	{
		Server		server;
		Location	location(server);
		Response	response(location, "HTTP/1.1");

		std::vector<char>	empty;
		CHECK(response.buildFromCgiOutput(empty) == false);
	}
}

// =============================================================================
TEST_SUITE("Response — serialisation")
{
	TEST_CASE("toString() commence par la status line HTTP")
	{
		Server		server;
		Response	response(404, server);

		std::string out = response.toString();
		CHECK(out.find("HTTP/1.1 404 Not Found\r\n") == 0);
		CHECK(out.find("\r\n\r\n") != std::string::npos);
	}
}
