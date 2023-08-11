# Battleship

A Battleship is an open-source two-player CLI game written in C++ with the use of ASIO framework. 

## Contributing

You are welcome to contribute by creating issues.

## Build

The game uses ncurses library for rendering so it should be installed on your machine before compiling the program:

>Ubuntu/Debian
```
sudo apt-get install libncurses5-dev libncursesw5-dev
```

>CentOS/RHEL/Fedora Linux
```
sudo yum install ncurses-devel
```

>Fedora Linux 22.x+
```
sudo dnf install ncurses-devel
```

Use the following commands to build the game:

```
git clone https://github.com/IvYehor/battleship.git
cd battleship
mkdir build
cd build
cmake ../
make
```


