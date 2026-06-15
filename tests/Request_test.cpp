// =============================================================================
// Request_test.cpp
// Feature : parsing HTTP incrementiel (Request)
//
// Couvre la machine a etats REQUEST -> HEADERS -> BODY -> TRAILERS -> DONE :
//   - ligne de requete (methode / path / version)
//   - headers (normalisation en minuscules, trim des valeurs)
//   - corps via Content-Length (complet et fragmente)
//   - corps via Transfer-Encoding: chunked
//   - arrivee fragmentee des donnees
//   - erreurs de parsing
// =============================================================================

#include "doctest.h"

#include "http/Request.hpp"
#include "config/Server.hpp"

#include <string>

// Un Request a besoin d'un Server pour connaitre client_max_body_size lors du
// parsing du corps. On en fournit un, configurable par test.
static Server make_server(std::string const& maxBody)
{
	Server	server;

	server.setClientMaxBodySize(maxBody);
	return (server);
}

// =============================================================================
TEST_SUITE("Request — ligne de requete")
{
	TEST_CASE("GET simple sans corps atteint l'etat DONE")
	{
		Server	server = make_server("1M");
		Request	request;
		request.setServer(&server);

		bool done = request.parseRequest(
			"GET /index.html HTTP/1.1\r\nHost: example.com\r\n\r\n");

		CHECK(done == true);
		CHECK(request.getParsingState() == DONE);
		CHECK(request.getMethod() == "GET");
		CHECK(request.getPath() == "/index.html");
		CHECK(request.getVersion() == "HTTP/1.1");
	}

	TEST_CASE("Une methode invalide leve une exception")
	{
		Server	server = make_server("1M");
		Request	request;
		request.setServer(&server);

		CHECK_THROWS(request.parseRequest("BREW / HTTP/1.1\r\n\r\n"));
	}

	TEST_CASE("Une ligne de requete incomplete leve une exception")
	{
		Server	server = make_server("1M");
		Request	request;
		request.setServer(&server);

		CHECK_THROWS(request.parseRequest("GET /only-two\r\n\r\n"));
	}
}

// =============================================================================
TEST_SUITE("Request — headers")
{
	TEST_CASE("Les noms de header sont normalises en minuscules")
	{
		Server	server = make_server("1M");
		Request	request;
		request.setServer(&server);

		request.parseRequest("GET / HTTP/1.1\r\nHost: a\r\nContent-Type: text/html\r\n\r\n");

		std::map<std::string, std::string> const& h = request.getHeaders();
		CHECK(h.find("host") != h.end());
		CHECK(h.find("content-type") != h.end());
		CHECK(h.find("Host") == h.end());
	}

	TEST_CASE("Les valeurs de header sont trimmees")
	{
		Server	server = make_server("1M");
		Request	request;
		request.setServer(&server);

		request.parseRequest("GET / HTTP/1.1\r\nHost:   example.com   \r\n\r\n");

		std::map<std::string, std::string> const& h = request.getHeaders();
		REQUIRE(h.find("host") != h.end());
		CHECK(h.find("host")->second == "example.com");
	}

	TEST_CASE("Un header sans ':' leve une exception")
	{
		Server	server = make_server("1M");
		Request	request;
		request.setServer(&server);

		CHECK_THROWS(request.parseRequest("GET / HTTP/1.1\r\nBadHeader\r\n\r\n"));
	}
}

// =============================================================================
TEST_SUITE("Request — corps Content-Length")
{
	TEST_CASE("Corps complet recu en une fois")
	{
		Server	server = make_server("1M");
		Request	request;
		request.setServer(&server);

		bool done = request.parseRequest(
			"POST /up HTTP/1.1\r\ncontent-length: 5\r\n\r\nhello");

		CHECK(done == true);
		CHECK(request.getParsingState() == DONE);
		std::vector<char> const& body = request.getBody();
		CHECK(std::string(body.begin(), body.end()) == "hello");
	}

	TEST_CASE("Corps fragmente sur plusieurs appels")
	{
		Server	server = make_server("1M");
		Request	request;
		request.setServer(&server);

		CHECK(request.parseRequest("POST /up HTTP/1.1\r\ncontent-length: 11\r\n\r\nhel") == false);
		CHECK(request.getParsingState() == BODY);
		CHECK(request.parseRequest("lo wor") == false);
		bool done = request.parseRequest("ld");

		CHECK(done == true);
		std::vector<char> const& body = request.getBody();
		CHECK(std::string(body.begin(), body.end()) == "hello world");
	}

	TEST_CASE("Sans Content-Length ni Transfer-Encoding le corps est vide et DONE")
	{
		Server	server = make_server("1M");
		Request	request;
		request.setServer(&server);

		bool done = request.parseRequest("POST /up HTTP/1.1\r\nHost: a\r\n\r\n");

		CHECK(done == true);
		CHECK(request.getBody().empty());
	}
}

// =============================================================================
TEST_SUITE("Request — corps chunked")
{
	TEST_CASE("Un corps chunked simple est reassemble")
	{
		Server	server = make_server("1M");
		Request	request;
		request.setServer(&server);

		bool done = request.parseRequest(
			"POST /up HTTP/1.1\r\ntransfer-encoding: chunked\r\n\r\n"
			"5\r\nhello\r\n6\r\n world\r\n0\r\n\r\n");

		CHECK(done == true);
		CHECK(request.getParsingState() == DONE);
		std::vector<char> const& body = request.getBody();
		CHECK(std::string(body.begin(), body.end()) == "hello world");
	}

	TEST_CASE("Un corps chunked fragmente est reassemble")
	{
		Server	server = make_server("1M");
		Request	request;
		request.setServer(&server);

		CHECK(request.parseRequest(
			"POST /up HTTP/1.1\r\ntransfer-encoding: chunked\r\n\r\n5\r\nhel") == false);
		bool done = request.parseRequest("lo\r\n0\r\n\r\n");

		CHECK(done == true);
		std::vector<char> const& body = request.getBody();
		CHECK(std::string(body.begin(), body.end()) == "hello");
	}

	TEST_CASE("Un transfer-encoding non supporte leve une exception")
	{
		Server	server = make_server("1M");
		Request	request;
		request.setServer(&server);

		CHECK_THROWS(request.parseRequest(
			"POST /up HTTP/1.1\r\ntransfer-encoding: gzip\r\n\r\n"));
	}
}

// =============================================================================
TEST_SUITE("Request — reset")
{
	TEST_CASE("reset() reinitialise l'etat pour une nouvelle requete")
	{
		Server	server = make_server("1M");
		Request	request;
		request.setServer(&server);

		request.parseRequest("GET /first HTTP/1.1\r\nHost: a\r\n\r\n");
		REQUIRE(request.getParsingState() == DONE);

		request.reset();
		CHECK(request.getParsingState() == REQUEST);
		CHECK(request.getMethod().empty());
		CHECK(request.getHeaders().empty());

		bool done = request.parseRequest("GET /second HTTP/1.1\r\nHost: b\r\n\r\n");
		CHECK(done == true);
		CHECK(request.getPath() == "/second");
	}
}
