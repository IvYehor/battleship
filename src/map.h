#ifndef MAP_H_INCLUDED
#define MAP_H_INCLUDED

#include <vector>
#include <array>
#include <string>

struct Ship {
	int x;
	int y;
	bool rot;
	int size;
};

// Map constants
struct MapC { 
	static constexpr int WIDTH = 10;
	static constexpr int HEIGHT = 10;
	
	// Number of ships of size 2 to 5
	// The zeroth ship has the size of 2
	static const std::vector<int> FLEET;
};

inline const std::vector<int> MapC::FLEET{2, 2, 1, 0};

class MyMap {
public:	
	MyMap() {
		for(int y = 0; y < MapC::HEIGHT; ++y)
			for(int x = 0; x < MapC::WIDTH; ++x)
				shots[y][x] = 0;
	}

	bool shoot(int x, int y) {
		shots[y][x] = 1;
		for(int i = 0; i < fleet.size(); ++i) {
			auto &f = fleet[i];
			if(!f.rot && x == f.x && y >= f.y && y < f.y+f.size) {
				fleetShots[i][y-f.y] = 1;
				return true;
			}
			else if(f.rot && y == f.y && x >= f.x && x < f.x+f.size) {
				fleetShots[i][x-f.x] = 1;
				return true;
			}
		}
		return false;
	}

	bool theyWon() {
		for(auto &shipS : fleetShots) {
			for(int i = 0; i < shipS.size(); ++i) {
				if(shipS[i] == 0)
					return false;
			}
		}
		return true;
	}
	
	std::vector<Ship> fleet;
	std::vector<std::vector<int>> fleetShots; // 0 - was not shot, 1 - was shot

	std::array<std::array<int, MapC::WIDTH>, MapC::HEIGHT> shots; // 0 - was not shot, 1 - was shot
};

class TheirMap {
public:	
	TheirMap() {
		for(int y = 0; y < MapC::HEIGHT; ++y)
			for(int x = 0; x < MapC::WIDTH; ++x)
				shots[y][x] = 0;
	}

	void shoot(int x, int y, int wasHit) {
		shots[y][x] = wasHit;
	}

	std::array<std::array<int, MapC::WIDTH>, MapC::HEIGHT> shots; // 0 - was not shot, 1 - missed, 2 - was shot
};

#endif
