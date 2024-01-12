#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <string>
#include <map>
#include <chrono>
#include <ctime>
#include <thread>
#include <random>
#include <fstream>
#include <functional>
#include <iomanip>
#include <mutex>

enum class CommandType
{
	DATE,
	TIME,
	EXIT,
	FASTER,
	SLOWER,
	PAUSE,
	RESUME,
	LEVEL_0,
	LEVEL_1,
	LEVEL_2,
	STAT
};

struct Event
{
	std::chrono::system_clock::time_point timestamp;
	int eventId;
	int param1;
	int param2;
	int param3;
	CommandType commandType;
};

class Logger
{
protected:
	std::string fileName;

public:
	Logger(const std::string& fileName) : fileName(fileName) {}
	virtual ~Logger() {}

	virtual Logger* Write(const Event& event) = 0;
};

class ZeroLevelLogger : public Logger
{
public:
	ZeroLevelLogger(const std::string& fileName) : Logger(fileName) {}

	Logger* Write(const Event& event) override
	{
		std::time_t timestamp = std::chrono::system_clock::to_time_t(event.timestamp);
		std::cout << "Timestamp: " << std::ctime(&timestamp);
		std::cout << "Event ID: " << event.eventId << std::endl;
		return this;
	}
};

class FirstLevelLogger : public Logger
{
public:
	FirstLevelLogger(const std::string& fileName) : Logger(fileName) {}

	Logger* Write(const Event& event) override
	{
		std::time_t timestamp = std::chrono::system_clock::to_time_t(event.timestamp);
		std::cout << "Timestamp: " << std::ctime(&timestamp);
		std::cout << "Event ID: " << event.eventId << std::endl;
		std::cout << "Param1: " << event.param1 << std::endl;
		return this;
	}
};

class SecondLevelLogger : public Logger
{
public:
	SecondLevelLogger(const std::string& fileName) : Logger(fileName) {}

	Logger* Write(const Event& event) override
	{
		std::time_t timestamp = std::chrono::system_clock::to_time_t(event.timestamp);
		std::cout << "Timestamp: " << std::ctime(&timestamp);
		std::cout << "Event ID: " << event.eventId << std::endl;
		std::cout << "Param1: " << event.param1 << std::endl;
		std::cout << "Param2: " << event.param2 << std::endl;
		std::cout << "Param3: " << event.param3 << std::endl;
		return this;
	}
};

class EventGenerator
{
private:
	std::chrono::milliseconds interval;
	std::mt19937 rng;
	std::uniform_int_distribution<int> distEventId;
	std::uniform_int_distribution<int> distParams;
	bool paused;
	std::mutex pauseMutex;
	int logLevel;

public:
	EventGenerator(int minInterval, int maxInterval) : logLevel(1)
	{
		std::random_device rd;
		rng.seed(rd());
		distEventId = std::uniform_int_distribution<int>(1, 100);
		distParams = std::uniform_int_distribution<int>(1, 100);
		setInterval(minInterval, maxInterval);
		paused = false;
	}

	void setInterval(int minInterval, int maxInterval)
	{
		interval = std::chrono::milliseconds(distParams(rng) % (maxInterval - minInterval + 1) + minInterval);
	}
	void setLogLevel(int level)
	{
		logLevel = level;
	}
	void Pause()
	{
		std::lock_guard<std::mutex> lock(pauseMutex);
		paused = true;
	}

	void Resume()
	{
		std::lock_guard<std::mutex> lock(pauseMutex);
		paused = false;;
	}
	void Faster()
	{
		if (interval.count() > 5000)
		{
			interval -= std::chrono::milliseconds(5000);
			std::cout << "Interval decreased to " << interval.count() << " milliseconds." << std::endl;
		}
	}

	void Slower()
	{
		interval += std::chrono::milliseconds(10000);
		std::cout << "Interval increased to " << interval.count() << " milliseconds." << std::endl;
	}

	void operator()(Logger* logger)
	{
		ZeroLevelLogger level0Logger("level0.log");
		FirstLevelLogger level1Logger("level1.log");
		SecondLevelLogger level2Logger("level2.log");
		while (true)
		{
			if (!paused)
			{
				Event event;
				event.timestamp = std::chrono::system_clock::now();
				event.eventId = distEventId(rng);
				event.param1 = distParams(rng);
				event.param2 = distParams(rng);
				event.param3 = distParams(rng);
				switch (logLevel)
				{
				case 0:
					logger = &level0Logger;
					break;
				case 1:
					logger = &level1Logger;
					break;
				case 2:
					logger = &level2Logger;
					break;
				default:
					logger = nullptr;
					break;
				}
				if (logger)
				{
					logger->Write(event);
				}
			}
			std::this_thread::sleep_for(interval);
		}
	}
};

int main()
{
	EventGenerator eventGenerator(4000, 4000);
	Logger* logger = nullptr;

	std::map<std::string, std::function<void()>> commands;
	commands["date"] = [&]() {
		std::time_t currentTime = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
		std::cout << "Current date: " << std::put_time(std::localtime(&currentTime), "%F") << std::endl;
		};
	commands["time"] = [&]() {
		std::time_t currentTime = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
		std::cout << "Current time: " << std::put_time(std::localtime(&currentTime), "%T") << std::endl;
		};
	commands["exit"] = [&]() {
		if (logger)
		{
			delete logger;
		}
		exit(0);
		};
	commands["faster"] = [&]() {
		eventGenerator.Faster();
		};
	commands["slower"] = [&]() {
		eventGenerator.Slower();
		};
	commands["pause"] = [&]() {
		eventGenerator.Pause();
		};
	commands["resume"] = [&]() {
		eventGenerator.Resume();
		};
	commands["level0"] = [&]() {
		eventGenerator.setLogLevel(0);
		};
	commands["level1"] = [&]() {
		eventGenerator.setLogLevel(1);
		};
	commands["level2"] = [&]() {
		eventGenerator.setLogLevel(2);
		};
	commands["stat"] = [&]() {
		std::ifstream file("level1.log");
		std::string line;
		int count = 0;
		while (std::getline(file, line))
		{
			count++;
		}
		std::cout << "Number of events in level1.log: " << count << std::endl;
		};

	std::thread eventThread(&EventGenerator::operator(), &eventGenerator, logger);

	std::string command;
	while (true)
	{
		std::cout << "Enter a command: ";
		std::cin >> command;

		auto it = commands.find(command);
		if (it != commands.end())
		{
			it->second();
		}
		else
		{
			std::cout << "Unknown command" << std::endl;
		}
	}

	eventThread.join();
	return 0;
}