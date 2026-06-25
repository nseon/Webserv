// =============================================================================
// Cgi_test.cpp
// Feature : CGI
//
// Cote serveur (unitaire) :
//   - parseCgiTarget : matching d'extension, PATH_INFO, QUERY_STRING
//   - checkExecutable : 404 / 403 / OK
//   - buildCgiEnvs   : variables d'environnement CGI
//
// Bout en bout (execution reelle de scripts CGI) :
//   - GET  : la query string passe par l'environnement
//   - POST : le corps passe par stdin (CONTENT_LENGTH)
//   - header Status custom interprete par Response::buildFromCgiOutput
//   - timeout CGI : un script lent est tue au bout du delai
//
// Les scripts de test vivent dans tests/cgi/.
// =============================================================================

#include "doctest.h"

#include "http/cgi.hpp"
#include "http/Request.hpp"
#include "http/Response.hpp"
#include "config/Server.hpp"
#include "config/Location.hpp"

#include <cstdlib>
#include <cstring>
#include <ctime>
#include <string>
#include <vector>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <poll.h>
#include <signal.h>
#include <unistd.h>

extern char**	environ;

// ---------------------------------------------------------------------------
// Localisation de l'interpreteur python3 et des scripts CGI.
// ---------------------------------------------------------------------------
static std::string find_python()
{
	const char* candidates[] = {
		"/usr/bin/python3", "/usr/local/bin/python3",
		"/opt/pyenv/shims/python3", 0
	};
	for (size_t i = 0; candidates[i]; ++i)
		if (access(candidates[i], X_OK) == 0)
			return (candidates[i]);
	return ("");
}

static std::string script_path(std::string const& name)
{
	std::string	rel = "tests/cgi/" + name;

	if (access(rel.c_str(), F_OK) == 0)
		return (rel);
	return ("cgi/" + name); // si lance depuis le dossier tests/
}

// ---------------------------------------------------------------------------
// Execute un CGI exactement comme le serveur (socketpair + fork + execve),
// envoie `body` sur stdin et collecte stdout. Renvoie false en cas de timeout
// (le process est alors tue), true sinon. La sortie brute est dans `out`.
// ---------------------------------------------------------------------------
static bool run_cgi(std::string const& interpreter, std::string const& script,
	std::string const& body, std::vector<char>& out, int timeoutMs)
{
	int	sv[2];
	if (socketpair(AF_UNIX, SOCK_STREAM, 0, sv) != 0)
		return (false);

	pid_t	pid = fork();
	if (pid < 0)
	{
		close(sv[0]);
		close(sv[1]);
		return (false);
	}
	if (pid == 0)
	{
		dup2(sv[1], STDIN_FILENO);
		dup2(sv[1], STDOUT_FILENO);
		close(sv[0]);
		close(sv[1]);

		char*	argv[3];
		argv[0] = const_cast<char*>(interpreter.c_str());
		argv[1] = const_cast<char*>(script.c_str());
		argv[2] = 0;
		execve(interpreter.c_str(), argv, environ);
		_exit(127);
	}

	close(sv[1]);
	if (!body.empty())
		write(sv[0], body.c_str(), body.size());
	shutdown(sv[0], SHUT_WR);

	bool	timedOut = false;
	time_t	deadline = std::time(NULL) + (timeoutMs / 1000) + 1;
	char	buffer[4096];

	while (true)
	{
		struct pollfd pfd;
		pfd.fd = sv[0];
		pfd.events = POLLIN;
		int ready = poll(&pfd, 1, 200);

		if (ready > 0)
		{
			ssize_t n = recv(sv[0], buffer, sizeof(buffer), 0);
			if (n > 0)
				out.insert(out.end(), buffer, buffer + n);
			else if (n == 0)
				break ;
		}
		if (std::time(NULL) >= deadline)
		{
			timedOut = true;
			break ;
		}
	}

	close(sv[0]);
	if (timedOut)
	{
		kill(pid, SIGKILL);
		waitpid(pid, NULL, 0);
		return (false);
	}
	waitpid(pid, NULL, 0);
	return (true);
}

// =============================================================================
TEST_SUITE("CGI — parseCgiTarget")
{
	static Location make_cgi_location()
	{
		Server		server;
		Location	location(server);
		location.addCgi(".py /usr/bin/python3");
		location.addCgi(".php /usr/bin/php-cgi");
		return (location);
	}

	TEST_CASE("Une extension CGI en fin d'URI est detectee")
	{
		Location	loc = make_cgi_location();
		CgiTarget	target;

		REQUIRE(parseCgiTarget(loc, "/cgi/script.py", target) == true);
		CHECK(target.scriptName == "/cgi/script.py");
		CHECK(target.pathInfo == "");
		CHECK(target.queryString == "");
		CHECK(target.interpreter == "/usr/bin/python3");
	}

	TEST_CASE("La query string est separee du chemin")
	{
		Location	loc = make_cgi_location();
		CgiTarget	target;

		REQUIRE(parseCgiTarget(loc, "/script.py?name=foo&x=1", target) == true);
		CHECK(target.scriptName == "/script.py");
		CHECK(target.queryString == "name=foo&x=1");
	}

	TEST_CASE("Le PATH_INFO apres l'extension est extrait")
	{
		Location	loc = make_cgi_location();
		CgiTarget	target;

		REQUIRE(parseCgiTarget(loc, "/script.py/extra/path", target) == true);
		CHECK(target.scriptName == "/script.py");
		CHECK(target.pathInfo == "/extra/path");
	}

	TEST_CASE("Une extension non CGI n'est pas detectee")
	{
		Location	loc = make_cgi_location();
		CgiTarget	target;

		CHECK(parseCgiTarget(loc, "/index.html", target) == false);
		CHECK(parseCgiTarget(loc, "/style.css", target) == false);
	}

	TEST_CASE("Une extension au milieu du nom n'est pas detectee")
	{
		Location	loc = make_cgi_location();
		CgiTarget	target;

		CHECK(parseCgiTarget(loc, "/script.php.old", target) == false);
	}
}

// =============================================================================
TEST_SUITE("CGI — checkExecutable")
{
	TEST_CASE("Un fichier inexistant renvoie 404")
	{
		CHECK(checkExecutable("/no/such/path/script.py") == 404);
	}

	TEST_CASE("Un fichier non executable renvoie 403")
	{
		char tmpl[] = "/tmp/webserv_cgi_XXXXXX";
		int fd = mkstemp(tmpl);
		REQUIRE(fd != -1);
		close(fd);
		chmod(tmpl, 0644);

		CHECK(checkExecutable(tmpl) == 403);
		unlink(tmpl);
	}

	TEST_CASE("Un fichier executable renvoie 0")
	{
		char tmpl[] = "/tmp/webserv_cgi_XXXXXX";
		int fd = mkstemp(tmpl);
		REQUIRE(fd != -1);
		close(fd);
		chmod(tmpl, 0755);

		CHECK(checkExecutable(tmpl) == 0);
		unlink(tmpl);
	}
}

// =============================================================================
TEST_SUITE("CGI — buildCgiEnvs")
{
	TEST_CASE("Les variables CGI essentielles sont presentes")
	{
		Server	server;
		server.setPort("0"); // port ephemere pour eviter les conflits
		REQUIRE_NOTHROW(server.createSocket());

		Request	request;
		request.setServer(&server);
		request.setVersion("HTTP/1.1");
		request.setMethod("GET");
		request.setPath("/script.py?a=1");
		request.addHeader("content-type", "text/plain");
		request.addHeader("content-length", "7");

		CgiTarget	target;
		target.interpreter = "/usr/bin/python3";
		target.scriptName = "/script.py";
		target.pathInfo = "";
		target.queryString = "a=1";

		char**	envs = buildCgiEnvs(request, target, "127.0.0.1");
		REQUIRE(envs != NULL);

		std::vector<std::string>	vars;
		for (size_t i = 0; envs[i]; ++i)
			vars.push_back(envs[i]);
		freeCgiEnvs(envs);

		bool method = false, gateway = false, query = false;
		bool clen = false, script = false, addr = false;
		for (size_t i = 0; i < vars.size(); ++i)
		{
			if (vars[i] == "REQUEST_METHOD=GET") method = true;
			if (vars[i] == "GATEWAY_INTERFACE=CGI/1.1") gateway = true;
			if (vars[i] == "QUERY_STRING=a=1") query = true;
			if (vars[i] == "CONTENT_LENGTH=7") clen = true;
			if (vars[i] == "SCRIPT_NAME=/script.py") script = true;
			if (vars[i] == "REMOTE_ADDR=127.0.0.1") addr = true;
		}
		CHECK(method);
		CHECK(gateway);
		CHECK(query);
		CHECK(clen);
		CHECK(script);
		CHECK(addr);
	}
}

// =============================================================================
TEST_SUITE("CGI — execution bout en bout")
{
	TEST_CASE("GET : la query string est transmise au script")
	{
		std::string python = find_python();
		if (python.empty())
		{
			WARN("python3 introuvable, test ignore");
			return ;
		}

		// On reproduit l'environnement CGI minimal d'une requete GET.
		setenv("REQUEST_METHOD", "GET", 1);
		setenv("QUERY_STRING", "name=foo", 1);
		setenv("PATH_INFO", "", 1);
		unsetenv("CONTENT_LENGTH");

		std::vector<char>	out;
		bool ok = run_cgi(python, script_path("hello.py"), "", out, 5000);
		REQUIRE(ok == true);

		// La sortie brute est parsee comme le ferait le serveur.
		Server		server;
		Location	location(server);
		Request		request;
		Response	response(location, "HTTP/1.1", request);
		REQUIRE(response.buildFromCgiOutput(out) == true);

		std::vector<char> const& body = response.getBody();
		std::string text(body.begin(), body.end());
		CHECK(text.find("method=GET") != std::string::npos);
		CHECK(text.find("query=name=foo") != std::string::npos);
	}

	TEST_CASE("POST : le corps est transmis via stdin")
	{
		std::string python = find_python();
		if (python.empty())
		{
			WARN("python3 introuvable, test ignore");
			return ;
		}

		std::string payload = "ceci est le corps poste";
		char		len[32];
		std::sprintf(len, "%lu", (unsigned long)payload.size());

		setenv("REQUEST_METHOD", "POST", 1);
		setenv("CONTENT_LENGTH", len, 1);
		unsetenv("QUERY_STRING");

		std::vector<char>	out;
		bool ok = run_cgi(python, script_path("echo.py"), payload, out, 5000);
		REQUIRE(ok == true);

		Server		server;
		Location	location(server);
		Request		request;
		Response	response(location, "HTTP/1.1", request);
		REQUIRE(response.buildFromCgiOutput(out) == true);

		std::vector<char> const& body = response.getBody();
		CHECK(std::string(body.begin(), body.end()) == payload);
	}

	TEST_CASE("Le header Status d'un CGI est interprete")
	{
		std::string python = find_python();
		if (python.empty())
		{
			WARN("python3 introuvable, test ignore");
			return ;
		}

		std::vector<char>	out;
		bool ok = run_cgi(python, script_path("status.py"), "", out, 5000);
		REQUIRE(ok == true);

		Server		server;
		Location	location(server);
		Request		request;
		Response	response(location, "HTTP/1.1", request);
		REQUIRE(response.buildFromCgiOutput(out) == true);
		CHECK(response.getStatusCode() == 418);
	}

	TEST_CASE("Timeout CGI : un script lent est interrompu")
	{
		std::string python = find_python();
		if (python.empty())
		{
			WARN("python3 introuvable, test ignore");
			return ;
		}

		// slow.py dort 30s ; on lui accorde 2s. Il doit etre tue.
		std::vector<char>	out;
		time_t	start = std::time(NULL);
		bool ok = run_cgi(python, script_path("slow.py"), "", out, 2000);
		time_t	elapsed = std::time(NULL) - start;

		CHECK(ok == false);          // timeout declenche
		CHECK(out.empty());          // aucune sortie produite
		CHECK(elapsed < 10);         // tue rapidement, bien avant les 30s
	}
}
