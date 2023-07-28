#include <string>
#include <thread>
#include "servergame.h"


void ServerGame::Start() {
	if(exchangeNames() == -1)
		return;
	if(setFleet() == -1)
		return;

	int res;
	bool iWon = false;
	while(1) {
		res = myTurn();
		if(res == -1)
			return;
		else if (res == 1) {
			iWon = true;
			break;
		}
		
		res = theirTurn();
		if(res == -1) {
			return;
		}
		else if(res == 1) {
			break;
		}
	}

	endScreen(iWon);
}

int ServerGame::exchangeNames() {

	renderer.enterName(myName);

	Message<std::string> theirNameMsg{theirName, 0};	
	int result = server.ReadArray<std::string>(theirNameMsg);

	if(result == -1) {
		server.Stop();
		renderer.errorScreen();
		return -1;
	}
	
	theirName = theirNameMsg.data;

	Message<std::string> myNameMsg{myName, myName.size()};
	if(server.WriteArray<std::string>(myNameMsg) == -1) {
		server.Stop();
		renderer.errorScreen();
		return -1;
	}

	return 0;
}

int ServerGame::setFleet() {
	myMap = MyMap();

	renderer.setFleet(myMap, myName, theirName);
	
	theirMap = TheirMap();

	Message<bool> readyMSGTheir{};
	if(server.ReadPOD(readyMSGTheir) == -1 || !readyMSGTheir.data) {
		server.Stop();
		renderer.errorScreen();
		return -1;
	}
	
	Message<bool> readyMSG{true, sizeof(bool)};
	if(server.WritePOD(readyMSG) == -1) {
		server.Stop();
		renderer.errorScreen();
		return -1;
	}

	return 0;
}

int ServerGame::myTurn() {
	int shotX, shotY;
	renderer.myTurn(myMap, theirMap, myName, theirName, shotX, shotY);

	std::vector<int> coords = {shotX, shotY};
	Message<std::vector<int>> coordsMSG{coords, sizeof(int) * 2};
	if(server.WriteArray(coordsMSG) == -1) {
		server.Stop();
		renderer.errorScreen();
		return -1;
	}

	std::vector<int> answer(2);
	Message<std::vector<int>> answerMSG{answer, sizeof(int) * 2};
	if(server.ReadArray(answerMSG) == -1) {
		server.Stop();
		renderer.errorScreen();
		return -1;
	}

	theirMap.shoot(shotX, shotY, answerMSG.data[0]);

	return answerMSG.data[1]; // 0 - still playing, 1 - server won
	
}

int ServerGame::theirTurn() {
	bool rec = false;

	std::thread t([this, &rec](){ renderer.theirTurn(myMap, theirMap, myName, theirName, rec); });
	
	std::vector<int> coords(2);
	Message<std::vector<int>> coordsMSG{coords, sizeof(int) * 2};
	if(server.ReadArray(coordsMSG) == -1) {
		server.Stop();
		renderer.errorScreen();
		return -1;
	}

	int wasShot = myMap.shoot(coordsMSG.data[0], coordsMSG.data[1]) ? 2 : 1;
	
	int theyWon = myMap.theyWon() ? 1 : 0;

	std::vector<int> answer = {wasShot, theyWon};
	Message<std::vector<int>> answerMSG{answer, sizeof(int) * 2};
	if(server.WriteArray(answerMSG) == -1) {
		server.Stop();
		renderer.errorScreen();
		return -1;
	}

	rec = true;
	if(t.joinable())
		t.join();


	return theyWon;
}

void ServerGame::endScreen(bool iWon) {
	renderer.endScreen(iWon);
	
	server.Stop();
}
