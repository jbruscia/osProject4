all: driver

driver: driver.cpp
	g++ driver.cpp -lpthread -lcurl -o driver -std=c++11
