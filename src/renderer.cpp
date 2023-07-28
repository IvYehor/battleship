#include <cstdlib>
#include <ctime>
#include <vector>
#include <string>

#include "renderer.h"
#include "map.h"

Renderer::Renderer() {
	initscr();
	cbreak();
	noecho();
	keypad(stdscr, TRUE);
	
	if(!has_colors()) {
		endwin();
		exit(1);
	}

	start_color();
	init_color(0, 0, 0, 0); // Black
	init_color(1, 0, 156, 340); // Dark blue
	init_color(2, 129, 340, 539); // Blue
	init_color(3, 840, 840, 840); // White
	init_color(4, 300, 300, 300); // Grey
	
	init_pair(1, 1, 0); // Sea
	init_pair(2, 3, 4); // Ships

	attron(A_BOLD);

	getmaxyx(stdscr, height, width);
	finished = true;
}

Renderer::~Renderer() {
	echo();
	nocbreak();
	clear();
}

bool Renderer::getState() const { return finished; }


void Renderer::center() {
	cX = width/2;
	cY = height/2;
}
void Renderer::setDrawPos(int x, int y) {
	drawPosX = x;
	drawPosY = y;
}

void Renderer::drawLine(int x, int y, int len, bool hor, char ch) {
	if(hor)
		mvhline(drawPosY + y, drawPosX + x, ch, len);
	else
		mvvline(drawPosY + y, drawPosX + x, ch, len);
}

void Renderer::drawBox(int x, int y, int w, int h, char ch) {
	drawLine(x, y, w, true, ch);
	drawLine(x, y, h, false, ch);
	drawLine(x, y+h-1, w, true, ch);
	drawLine(x+w-1, y, h, false, ch);
}

void Renderer::drawRandomChars(int w, int h, int n, unsigned int seed, char ch) {
	srand(seed);
	for(int i = 0; i < n; ++i) {
		drawChar(rand() % w, rand() % h, ch);
	}
}

void Renderer::drawChar(int x, int y, char ch) {
	mvaddch(drawPosY + y, drawPosX + x, ch);
}
void Renderer::printMsg(int x, int y, std::string msg) {	
	mvprintw(drawPosY + y, drawPosX + x, msg.data());
}

void Renderer::drawTitle() {
	printMsg(0, 0, "  ____                _       _       _                      _         _           ");
	printMsg(0, 1, " |  _ \\              | |     | |     | |                    | |       (_)          ");
	printMsg(0, 2, " | |_) |     __ _    | |_    | |_    | |     ___     ___    | |__      _     _ __  ");
	printMsg(0, 3, " |  _ <     / _` |   | __|   | __|   | |    / _ \\   / __|   | '_ \\    | |   | '_ \\ ");
	printMsg(0, 4, " | |_) |   | (_| |   | |_    | |_    | |   |  __/   \\__ \\   | | | |   | |   | |_) |");
	printMsg(0, 5, " |____/     \\__,_|    \\__|    \\__|   |_|    \\___|   |___/   |_| |_|   |_|   | .__/ ");
	printMsg(0, 6, "                                                                            | |    ");
	printMsg(0, 7, "                                                                            |_|    ");
}

void Renderer::drawInstructions() {
	drawBox(-1, -1, 30, 5, '-');
	printMsg(0, 0, "Arrow UP/DOWN/LEFT/RIGHT");
	printMsg(0, 1, "\t or hjkl to move");
	printMsg(0, 2, "Enter to shoot and set fleet");
}

int Renderer::playerChoose(bool &choice) {
	choice = false;
	bool redraw = true;
	bool enter = false;
	int ch;
	int w,h;

	unsigned int waterSeed = time(0);
	
	while(1) {
		if(redraw) {
			clear();
			wborder(stdscr, '-', '-', '-', '-', '-', '-', '-', '-');

			setDrawPos(1, 1);
			attron(COLOR_PAIR(1));
			drawRandomChars(width-2, height-2, 50, waterSeed, '~');
			attroff(COLOR_PAIR(1));

			setDrawPos(width/2-40, 2);
			drawTitle();

			setDrawPos(2, height-5);
			drawInstructions();
			
			mvprintw(height/2, width/3, "Server");
			if(!choice)
				mvprintw(height/2+1, width/3, "  /\\");
			else 
				mvprintw(height/2+1, width/3, "    ");
			
			mvprintw(height/2, width/3*2, "Client");
			if(choice)
				mvprintw(height/2+1, width/3*2, "  /\\");
			else
				mvprintw(height/2+1, width/3*2, "    ");
			
			refresh();
			redraw = false;
		}
	
		getmaxyx(stdscr, h, w);
		if(h != height || w != width) {
			width = w;
			height = h;
			redraw = true;
		}

		ch = getch();
		if(ch == KEY_LEFT || ch == KEY_RIGHT) {
			choice = !choice;
			redraw = true;
		}
		else if(ch == '\n') {
			break;
		}
	}

	return 0;
}

int Renderer::waitingForClient(const bool &accepted) {
	if (!finished)
		return -1;

	unsigned int waterSeed = time(0);
	
	finished = false;
	std::thread t([this, &accepted, &waterSeed](){
		bool redraw = true;
		while(1) {
			if(accepted)
				break;

			int w,h;
			getmaxyx(stdscr, h, w);

			if(h != height || w != width) {
				width = w;
				height = h;
				redraw = true;
			}
			if (!redraw)
				continue;
			clear();
			wborder(stdscr, '-', '-', '-', '-', '-', '-', '-', '-');
			
			setDrawPos(1, 1);
			attron(COLOR_PAIR(1));
			drawRandomChars(width-2, height-2, 50, waterSeed, '~');
			attroff(COLOR_PAIR(1));

			setDrawPos(width/2-40, 2);
			drawTitle();

			mvprintw(height/2, width/2-10, "Waiting for client...");
			refresh();
			redraw = false;
		}
		finished = true;
	});
	t.detach();
	return 0;
}

int Renderer::enterIpAddress(std::string &ip) {
	unsigned int waterSeed = time(0);

	echo();
	nocbreak();

	clear();
	wborder(stdscr, '-', '-', '-', '-', '-', '-', '-', '-');
	
	setDrawPos(width/2-40, 2);
	drawTitle();

	setDrawPos(1, 1);
	attron(COLOR_PAIR(1));
	drawRandomChars(width-2, height-2, 50, waterSeed, '~');
	attroff(COLOR_PAIR(1));
	
	mvprintw(height/2, width/3, "Enter IP address:");
	
	char cip[50];
	scanw("%s", cip);
	ip = cip;

	noecho();
	cbreak();

	return 0;	
}

int Renderer::enterName(std::string &name) {
	unsigned int waterSeed = time(0);

	echo();
	nocbreak();

	clear();
	border('-', '-', '-', '-', '-', '-', '-', '-');
	
	setDrawPos(width/2-40, 2);
	drawTitle();

	setDrawPos(1, 1);
	attron(COLOR_PAIR(1));
	drawRandomChars(width-2, height-2, 50, waterSeed, '~');
	attroff(COLOR_PAIR(1));
	
	mvprintw(height/2, width/3, "Enter your name:");
	
	char cname[50];	
	scanw("%s", cname);
	name = cname;

	noecho();
	cbreak();

	return 0;
}

// # - ship
// X - missed
// * - got shot
// @ - cursor


int Renderer::setFleet(MyMap &myMap, std::string &myName, std::string &theirName) {	
	int cursorX = 0, cursorY = 0;
	int ch;

	int W = MapC::WIDTH;
	int H = MapC::HEIGHT;

	std::vector<int> mapcFleet = MapC::FLEET;
	int fleetSize = mapcFleet.size();
	int currentShip = 2;
	bool setHorizontal = false;

	unsigned int waterSeed = time(0);

	while(1) {	
		getmaxyx(stdscr, height, width);
		center();

		clear();
		border('-', '-', '-', '-', '-', '-', '-', '-');
	
		setDrawPos(1, 1);
		attron(COLOR_PAIR(1));
		drawRandomChars(width-2, height-2, 50, waterSeed, '~');
		attroff(COLOR_PAIR(1));
		
		mvprintw(1, 2, "You: %s | Opponent: %s", myName.data(), theirName.data());
		mvprintw(2, 2, "Available ships:");

		// Draw available fleet
		attron(COLOR_PAIR(2));
		int xpos = 0;
		for(int f = 0; f < fleetSize; ++f) {
			for(int i = 0; i < mapcFleet[f]; ++i)
				mvvline(4, 2 + (xpos + i) * 3, '#', f+2);
			xpos += mapcFleet[f];
		}
		attroff(COLOR_PAIR(2));

		// Draw borders of your map
		setDrawPos(cX-W/2+1, cY-H/2+1);
		drawBox(-1, -1, W+2, H+2, '=');
			
		setDrawPos(cX-W/2, cY-H/2);
		printMsg(0, H+4, "# - ship");
		printMsg(0, H+5, "@ - cursor");
		
		setDrawPos(cX-W/2+1, cY-H/2+1);
		attron(COLOR_PAIR(2));
		// Draw current ship
		if(currentShip-2 < fleetSize)
			drawLine(cursorX, cursorY, currentShip, setHorizontal, '@');
		
		// Draw the rest of the ships on the map
		for(auto &ship : myMap.fleet) {
			drawLine(ship.x, ship.y, ship.size, ship.rot, '#');
		}
		attroff(COLOR_PAIR(2));
		
		refresh();	


		// Keyboard

		if(currentShip-2 >= fleetSize)
			break;
		
		ch = getch();

		
		// If enter was pressed, place a ship
		if(ch == '\n') {
			std::vector<int> shipShots;
			for(int i = 0; i < currentShip; ++i)
				shipShots.push_back(0);

			myMap.fleet.push_back({cursorX, cursorY, setHorizontal, currentShip});
			myMap.fleetShots.push_back(shipShots);

			mapcFleet[currentShip-2]--;
			
			// Select the next available ship
			while(mapcFleet[currentShip-2] == 0 && currentShip-2 < fleetSize)
				currentShip++;
			
			if(currentShip-2 == fleetSize) {
				// Draw one more time after the last ship
				continue;
			}
		}
		


		setHorizontal = (ch == 'r' || ch == 'R') ? !setHorizontal : setHorizontal;

		cursorX += (ch == KEY_LEFT) ? -1 : 0;
		cursorX += (ch == KEY_RIGHT) ? 1 : 0;
		cursorY += (ch == KEY_UP) ? -1 : 0;
		cursorY += (ch == KEY_DOWN) ? 1 : 0;
		
		cursorX = (cursorX < 0) ? 0 : cursorX;
		if (setHorizontal)
			cursorX = (cursorX > W-currentShip) ? W-currentShip : cursorX;
		else
			cursorX = (cursorX > W-1) ? W-1 : cursorX;

		cursorY = (cursorY < 0) ? 0 : cursorY;
		if (!setHorizontal)
			cursorY = (cursorY > H-currentShip) ? W-currentShip : cursorY;
		else
			cursorY = (cursorY > W-1) ? W-1 : cursorY;
	}
	return 0;
}

int Renderer::myTurn(MyMap &myMap, TheirMap &theirMap, std::string &myName, std::string &theirName, int &shotX, int &shotY) {
	int ch;
	int cursorX = 0, cursorY = 0;
	
	int W = MapC::WIDTH;
	int H = MapC::HEIGHT;

	unsigned int waterSeed = time(0);
	
	while(1) {	
		getmaxyx(stdscr, height, width);
		center();

		clear();
		border('-', '-', '-', '-', '-', '-', '-', '-');
	
		setDrawPos(1, 1);
		attron(COLOR_PAIR(1));
		drawRandomChars(width-2, height-2, 50, waterSeed, '~');
		attroff(COLOR_PAIR(1));

		// Draw borders
		setDrawPos(cX-W-W/2+1, cY-H/2+1);
		printMsg(-1, -3, myName);
		drawBox(-1, -1, W+2, H+2, '=');
		setDrawPos(cX+W-W/2+1, cY-H/2+1);	
		printMsg(-1, -3, theirName);
		drawBox(-1, -1, W+2, H+2, '=');

		setDrawPos(cX-W-W/2, cY-H/2);
		printMsg(0, H+4, "# - ship");
		printMsg(0, H+5, "* - got hit");
		printMsg(0, H+6, "X - missed");
		printMsg(0, H+7, "@ - cursor");
	
		//Draw my map
		setDrawPos(cX-W-W/2+1, cY-H/2+1);
		for(int y = 0; y < myMap.shots.size(); ++y) {
			for(int x = 0; x < myMap.shots[y].size(); ++x) {
				if(myMap.shots[y][x])
					drawLine(x, y, 1, true, 'X');
			}
		}

		attron(COLOR_PAIR(2));
		for(int f = 0; f < myMap.fleet.size(); ++f) {
			auto &ship = myMap.fleet[f];
				
			drawLine(ship.x, ship.y, ship.size, ship.rot, '#');

			for(int s = 0; s < ship.size; ++s) {
				int xOff = (ship.rot) ? s : 0;
				int yOff = (ship.rot) ? 0 : s;

				if(!myMap.fleetShots[f][s])
					continue;
				
				drawLine(ship.x + xOff, ship.y + yOff, 1, true, '*');
			}
		}
		attron(COLOR_PAIR(1));

		// Draw their map
		setDrawPos(cX+W-W/2+1, cY-H/2+1);
		attron(COLOR_PAIR(2));
		for(int y = 0; y < theirMap.shots.size(); ++y) {
			for(int x = 0; x < theirMap.shots[y].size(); ++x) {
				int ch = (theirMap.shots[y][x] == 2) ? '*' : ' ';
				ch = (theirMap.shots[y][x] == 1) ? 'X' : ch;
				drawLine(x, y, 1, true, ch);
			}
		}
		attroff(COLOR_PAIR(2));
		
		drawChar(cursorX, cursorY, '@');

		refresh();
		
		ch = getch();
		
		cursorX += (ch == KEY_LEFT) ? -1 : 0;
		cursorX += (ch == KEY_RIGHT) ? 1 : 0;
		cursorY += (ch == KEY_UP) ? -1 : 0;
		cursorY += (ch == KEY_DOWN) ? 1 : 0;

		cursorX = (cursorX < 0) ? 0 : cursorX;
		cursorX = (cursorX > W-1) ? W-1 : cursorX;
		cursorY = (cursorY < 0) ? 0 : cursorY;
		cursorY = (cursorY > W-1) ? W-1 : cursorY;

		if(ch == '\n') {
			shotX = cursorX;
			shotY = cursorY;
			return 0;
		}
	}
}

int Renderer::theirTurn(MyMap &myMap, TheirMap &theirMap, std::string &myName, std::string &theirName, bool &received) {
	int W = MapC::WIDTH;
	int H = MapC::HEIGHT;

	unsigned int waterSeed = time(0);
	
	getmaxyx(stdscr, height, width);
	center();

	clear();
	border('-', '-', '-', '-', '-', '-', '-', '-');

	setDrawPos(1, 1);
	attron(COLOR_PAIR(1));
	drawRandomChars(width-2, height-2, 50, waterSeed, '~');
	attroff(COLOR_PAIR(1));
	
	// Draw borders
	setDrawPos(cX-W-W/2+1, cY-H/2+1);
	printMsg(-1, -3, myName);
	drawBox(-1, -1, W+2, H+2, '=');
	setDrawPos(cX+W-W/2+1, cY-H/2+1);	
	printMsg(-1, -3, theirName);
	drawBox(-1, -1, W+2, H+2, '=');
		
	setDrawPos(cX-W-W/2, cY-H/2);
	printMsg(0, H+4, "# - ship");
	printMsg(0, H+5, "* - got hit");
	printMsg(0, H+6, "X - missed");
	printMsg(0, H+7, "@ - cursor");
	
	
	//Draw my map
	setDrawPos(cX-W-W/2+1, cY-H/2+1);
	for(int y = 0; y < myMap.shots.size(); ++y) {
		for(int x = 0; x < myMap.shots[y].size(); ++x) {
			if(myMap.shots[y][x])
				drawLine(x, y, 1, true, 'X');
		}
	}

	for(int f = 0; f < myMap.fleet.size(); ++f) {
		auto &ship = myMap.fleet[f];
		
		drawLine(ship.x, ship.y, ship.size, ship.rot, '#');

		for(int s = 0; s < ship.size; ++s) {
			int xOff = (ship.rot) ? s : 0;
			int yOff = (ship.rot) ? 0 : s;
			
			if(!myMap.fleetShots[f][s])
				continue;

			drawLine(ship.x + xOff, ship.y + yOff, 1, true, '*');
		}
	}
	
	// Draw their map
	setDrawPos(cX+W-W/2+1, cY-H/2+1);
	for(int y = 0; y < theirMap.shots.size(); ++y) {
		for(int x = 0; x < theirMap.shots[y].size(); ++x) {
			int ch = (theirMap.shots[y][x] == 2) ? '*' : ' ';
			ch = (theirMap.shots[y][x] == 1) ? 'X' : ch;
			drawLine(x, y, 1, true, ch);
		}
	}

	refresh();

	while(!received);
	return 0;	
}

int Renderer::endScreen(bool iwon) {
	unsigned int waterSeed = time(0);

	getmaxyx(stdscr, height, width);
		
	clear();
	border('-', '-', '-', '-', '-', '-', '-', '-');
	
	setDrawPos(1, 1);
	attron(COLOR_PAIR(1));
	drawRandomChars(width-2, height-2, 50, waterSeed, '~');
	attroff(COLOR_PAIR(1));
	
	mvprintw(height/2, width/3, (iwon) ?  "You won!" : "You lost!");
	
	refresh();
	
	getch();

	return 0;
}

void Renderer::errorScreen() {
	getmaxyx(stdscr, height, width);
		
	clear();
	border('-', '-', '-', '-', '-', '-', '-', '-');
	
	mvprintw(height/2, width/3, "Error!");
	
	refresh();
	
	getch();
}
