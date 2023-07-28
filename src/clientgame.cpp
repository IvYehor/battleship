#include <string>
#include <iostream>
#include <thread>
#include "clientgame.h"


void ClientGame::Start() {
	if(exchangeNames() == -1)
		return;
	if(setFleet() == -1)
		return;

	int res;
	bool iWon = false;
	while(1) {
		res = theirTurn();
		if(res == -1) {
			return;
		}
		else if(res == 1) {
			break;
		}
		
		res = myTurn();
		if(res == -1)
			return;
		else if (res == 1) {
			iWon = true;
			break;
		}
	}

	endScreen(iWon);
}

int ClientGame::exchangeNames() {

	renderer.enterName(myName);

	Message<std::string> myNameMsg{myName, myName.size()};
	int result1 = client.WriteArray<std::string>(myNameMsg);
	
	if(result1 == -1) {
		client.Stop();
		renderer.errorScreen();
		return -1;	
	}

	Message<std::string> theirNameMsg{theirName, 0};	
	int result = client.ReadArray<std::string>(theirNameMsg);

	if(result == -1) {
		client.Stop();
		renderer.errorScreen();
		return -1;
	}

	theirName = theirNameMsg.data;

	return 0;
}

int ClientGame::setFleet() {
	myMap = MyMap();

	renderer.setFleet(myMap, myName, theirName);
	
	theirMap = TheirMap();

	Message<bool> readyMSG{true, sizeof(bool)};
	if(client.WritePOD(readyMSG) == -1) {
		client.Stop();
		renderer.errorScreen();
		return -1;
	}
	
	Message<bool> readyMSGTheir{};
	if(client.ReadPOD(readyMSGTheir) == -1 || !readyMSGTheir.data) {
		client.Stop();
		renderer.errorScreen();
		return -1;
	}


	return 0;
}

int ClientGame::myTurn() {
	int shotX, shotY;
	renderer.myTurn(myMap, theirMap, myName, theirName, shotX, shotY);

	std::vector<int> coords = {shotX, shotY};
	Message<std::vector<int>> coordsMSG{coords, sizeof(int) * 2};
	if(client.WriteArray(coordsMSG) == -1) {
		client.Stop();
		renderer.errorScreen();
		return -1;
	}

	std::vector<int> answer(2); 
	Message<std::vector<int>> answerMSG{answer, sizeof(int) * 2};
	if(client.ReadArray(answerMSG) == -1) {
		client.Stop();
		renderer.errorScreen();
		return -1;
	}

	theirMap.shoot(shotX, shotY, answerMSG.data[0]);
	return answerMSG.data[1]; // 0 - still playing, 1 - client won
	
}

int ClientGame::theirTurn() {
	bool rec = false;

	std::thread t([this, &rec](){ renderer.theirTurn(myMap, theirMap, myName, theirName, rec); });

	std::vector<int> coords(2);
	Message<std::vector<int>> coordsMSG{coords, sizeof(int) * 2};
	if(client.ReadArray(coordsMSG) == -1) {
		client.Stop();
		renderer.errorScreen();
		return -1;
	}

	int wasShot = myMap.shoot(coordsMSG.data[0], coordsMSG.data[1]) ? 2 : 1;
	
	int theyWon = myMap.theyWon() ? 1 : 0;

	std::vector<int> answer = {wasShot, theyWon};
	Message<std::vector<int>> answerMSG{answer, sizeof(int) * 2};
	if(client.WriteArray(answerMSG) == -1) {
		client.Stop();
		renderer.errorScreen();
		return -1;
	}

	rec = true;
	if(t.joinable())
		t.join();

	return theyWon;
}

void ClientGame::endScreen(bool iWon) {
	renderer.endScreen(iWon);
	
	client.Stop();
}
