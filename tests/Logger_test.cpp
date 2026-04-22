#include "doctest.h"
#include "Logger.hpp"
#include <sstream>
#include <iostream>
#include <string>

// Redirects std::cout to a stringstream for the lifetime of the object
class CoutCapture
{
public:
	CoutCapture() : _old(std::cout.rdbuf(_ss.rdbuf())) {}
	~CoutCapture() { std::cout.rdbuf(_old); }
	std::string str() const { return _ss.str(); }

private:
	std::stringstream	_ss;
	std::streambuf*		_old;
};

TEST_CASE("Logger::log - NO_LEVEL")
{
	CoutCapture cap;
	Logger::log(Logger::NO_LEVEL) << "msg";
	CHECK(cap.str().find("[NO_LVL]") != std::string::npos);
}

TEST_CASE("Logger::log - INFO")
{
	CoutCapture cap;
	Logger::log(Logger::INFO) << "msg";
	CHECK(cap.str().find("[INFO]") != std::string::npos);
}

TEST_CASE("Logger::log - DEBUG")
{
	CoutCapture cap;
	Logger::log(Logger::DEBUG) << "msg";
	CHECK(cap.str().find("[DEBUG]") != std::string::npos);
}

TEST_CASE("Logger::log - WARNING")
{
	CoutCapture cap;
	Logger::log(Logger::WARNING) << "msg";
	CHECK(cap.str().find("[WARNING]") != std::string::npos);
}

TEST_CASE("Logger::log - ERROR")
{
	CoutCapture cap;
	Logger::log(Logger::ERROR) << "msg";
	CHECK(cap.str().find("[ERROR]") != std::string::npos);
}

TEST_CASE("Logger::log - CRITICAL")
{
	CoutCapture cap;
	Logger::log(Logger::CRITICAL) << "msg";
	CHECK(cap.str().find("[CRITICAL]") != std::string::npos);
}

TEST_CASE("Logger::info shorthand matches log(INFO)")
{
	CoutCapture cap;
	Logger::info() << "msg";
	CHECK(cap.str().find("[INFO]") != std::string::npos);
}

TEST_CASE("Logger::debug shorthand matches log(DEBUG)")
{
	CoutCapture cap;
	Logger::debug() << "msg";
	CHECK(cap.str().find("[DEBUG]") != std::string::npos);
}

TEST_CASE("Logger::warning shorthand matches log(WARNING)")
{
	CoutCapture cap;
	Logger::warning() << "msg";
	CHECK(cap.str().find("[WARNING]") != std::string::npos);
}

TEST_CASE("Logger::error shorthand matches log(ERROR)")
{
	CoutCapture cap;
	Logger::error() << "msg";
	CHECK(cap.str().find("[ERROR]") != std::string::npos);
}

TEST_CASE("Logger::critical shorthand matches log(CRITICAL)")
{
	CoutCapture cap;
	Logger::critical() << "msg";
	CHECK(cap.str().find("[CRITICAL]") != std::string::npos);
}

TEST_CASE("Logger output starts with a date prefix")
{
	CoutCapture cap;
	Logger::info() << "";
	std::string out = cap.str();
	// Format: [DD Mon YYYY HH:MM:SS] => 22 chars
	REQUIRE(out.size() >= 22);
	CHECK(out[0] == '[');
	CHECK(out[21] == ']');
}

TEST_CASE("Logger stream is usable for chaining")
{
	CoutCapture cap;
	Logger::info() << "hello" << " " << "world";
	CHECK(cap.str().find("hello world") != std::string::npos);
}

TEST_CASE("Logger message is appended after the prefix")
{
	CoutCapture cap;
	Logger::warning() << "something bad";
	std::string out = cap.str();
	CHECK(out.find("[WARNING]") != std::string::npos);
	CHECK(out.find("something bad") != std::string::npos);
	CHECK(out.find("[WARNING]") < out.find("something bad"));
}
