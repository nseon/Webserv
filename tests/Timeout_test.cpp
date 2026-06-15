// =============================================================================
// Timeout_test.cpp
// Feature : timeouts (clients et CGI)
//
// La boucle serveur (ServerManager::serverLoop) decide d'expirer un socket
// lorsque difftime(now, lastTimeUsed) >= TIMEOUT. On teste ici la primitive
// de suivi du temps portee par ASocket (getLastTimeUsed / updateLastTimeUsed)
// ainsi que la condition de declenchement, sans faire tourner la boucle
// infinie.
// =============================================================================

#include "doctest.h"

#include "socket/ASocket.hpp"
#include "manager/ServerManager.hpp" // CLIENT_TIMEOUT / CGI_TIMEOUT

#include <ctime>

// Socket concret minimal : permet d'anti-dater _lastTimeUsed pour simuler
// un socket inactif depuis longtemps (membre protege, accessible en derive).
class TimeSocket : public ASocket
{
public:
	TimeSocket() : ASocket((Server*)0) {}
	void backdate(time_t seconds) { _lastTimeUsed -= seconds; }
	virtual int socketBehavior(void*) { return (0); }
};

// Reproduit la condition exacte de serverLoop().
static bool is_expired(ASocket const& s, int timeout)
{
	return (difftime(std::time(NULL), s.getLastTimeUsed()) >= timeout);
}

// =============================================================================
TEST_SUITE("Timeout — primitive de suivi du temps")
{
	TEST_CASE("Un socket fraichement cree n'est pas expire")
	{
		TimeSocket	s;
		CHECK(is_expired(s, CLIENT_TIMEOUT) == false);
		CHECK(difftime(std::time(NULL), s.getLastTimeUsed()) < 2);
	}

	TEST_CASE("updateLastTimeUsed reinitialise le compteur")
	{
		TimeSocket	s;
		s.backdate(CLIENT_TIMEOUT + 10);
		REQUIRE(is_expired(s, CLIENT_TIMEOUT) == true);

		s.updateLastTimeUsed();
		CHECK(is_expired(s, CLIENT_TIMEOUT) == false);
	}
}

// =============================================================================
TEST_SUITE("Timeout — clients")
{
	TEST_CASE("Un client inactif au-dela de CLIENT_TIMEOUT est expire")
	{
		TimeSocket	s;
		s.backdate(CLIENT_TIMEOUT + 1);
		CHECK(is_expired(s, CLIENT_TIMEOUT) == true);
	}

	TEST_CASE("Un client recemment actif n'est pas expire")
	{
		TimeSocket	s;
		s.backdate(CLIENT_TIMEOUT - 5);
		CHECK(is_expired(s, CLIENT_TIMEOUT) == false);
	}
}

// =============================================================================
TEST_SUITE("Timeout — CGI")
{
	TEST_CASE("Un CGI inactif au-dela de CGI_TIMEOUT est expire")
	{
		TimeSocket	s;
		s.backdate(CGI_TIMEOUT + 1);
		CHECK(is_expired(s, CGI_TIMEOUT) == true);
	}

	TEST_CASE("Un CGI dans les temps n'est pas expire")
	{
		TimeSocket	s;
		s.backdate(CGI_TIMEOUT - 5);
		CHECK(is_expired(s, CGI_TIMEOUT) == false);
	}
}
