#include <iostream>
#include <windows.h>
#include <conio.h>
using namespace std;

enum GameObject : short { HALL, WALL, APPLE };
enum Color { BLACK = 1, DARKGREEN = 2, YELLOW = 14, RED = 12, BLUE = 9, WHITE = 15, DARKYELLOW = 6, DARKRED = 4 };
enum Key { ENTER = 13, ESCAPE = 27, SPACE = 32, LEFT = 75, RIGHT = 77, UP = 72, DOWN = 80, BACKSPACE = 8};


void show_menu(bool& game_started) {
	system("cls");
	cout << "=====================\n";
	cout << "  Welcome to snake!\n";
	cout << "=====================\n";
	cout << "1. Play\n";
	cout << "2. Records(under construct! u will be redirected to the game)\n";
	cout << "3. Exit\n";
	cout << "Choose an option: ";

	int choice;
	cin >> choice;

	switch (choice) {
	case 1:
		cout << "Starting game...\n";
		Sleep(1000);
		system("cls");
		game_started = true;
		break;
	case 2:
		system("cls");
		cout << "There will be your records!\n";
		Sleep(1000);
		system("cls");
		game_started = true;
		break;
	case 3:
		cout << "Exiting game...\n";
		exit(0);
	default:
		cout << "Wrang choise. Try again.\n";
		system("pause");
	}
}
void fill_2d_array(int** ar, int width, int height) {
    for (int y = 0; y < width; ++y) {
        for (int x = 0; x < height; ++x) {
			if (y == 0 || y == width - 1 || x == 0 || x == height - 1) {
				ar[y][x] = WALL;
			}
			else {
				ar[y][x] = HALL;
			}
        }
    }
}
void print_2d_array(int** ar, int width, int height, HANDLE h) {
	for (int y = 0; y < width; ++y) {
		for (int x = 0; x < height; ++x) {
			switch (ar[y][x]) {
			case HALL:
				cout << " ";
				break;
			case WALL:
				SetConsoleTextAttribute(h, Color::WHITE);
				cout << (char)178;
				break;
			case APPLE:
				SetConsoleTextAttribute(h, Color::RED);
				cout << "o";
				break;
			}
		}
		cout << "\n";
	}
}
void clear_2d_array(int**& ar, int width) {
	for (int i = 0; i < width; ++i) {
		delete[] ar[i];
		ar[i] = nullptr;
	}
	delete[] ar;
	ar = nullptr;
}
void update_direction_from_key(int code, bool& move_left, bool& move_right, bool& move_up, bool& move_down) {
	if (code == Key::LEFT && !move_right) {
		move_left = true;
		move_right = move_up = move_down = false;
	}
	else if (code == Key::RIGHT && !move_left) {
		move_right = true;
		move_left = move_up = move_down = false;
	}
	else if (code == Key::UP && !move_down) {
		move_up = true;
		move_down = move_left = move_right = false;
	}
	else if (code == Key::DOWN && !move_up) {
		move_down = true;
		move_up = move_left = move_right = false;
	}
}
void snake_move(int**& ar, int width, int height, bool& has_been_moved, bool move_left, bool move_right, bool move_up, bool move_down, COORD& snake) {

	if (move_left && snake.X > 0 && ar[snake.Y][snake.X - 1] != GameObject::WALL) {
		snake.X--;
		has_been_moved = true;
	}
	else if (move_right && snake.X < width - 1 && ar[snake.Y][snake.X + 1] != GameObject::WALL) {
		snake.X++;
		has_been_moved = true;
	}
	else if (move_up && snake.Y > 0 && ar[snake.Y - 1][snake.X] != GameObject::WALL) {
		snake.Y--;
		has_been_moved = true;
	}
	else if (move_down && snake.Y < height - 1 && ar[snake.Y + 1][snake.X] != GameObject::WALL) {
		snake.Y++;
		has_been_moved = true;
	}
}


int main()
{
	bool game_started = false;

	bool move_up = false;
	bool move_down = false;
	bool move_left = false;
	bool move_right = false;

	show_menu(game_started);

    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    srand(time(0));
    rand();
    system("title Snakek");

	CONSOLE_FONT_INFOEX font; // https://docs.microsoft.com/en-us/windows/console/console-font-infoex
	font.cbSize = sizeof(font);
	font.dwFontSize.Y = 25;
	font.FontFamily = FF_DONTCARE;
	font.FontWeight = FW_NORMAL;
	wcscpy_s(font.FaceName, 9, L"Consolas");
	SetCurrentConsoleFontEx(h, 0, &font);

    CONSOLE_CURSOR_INFO cursor;
    cursor.bVisible = false;
    cursor.dwSize = 100;
    SetConsoleCursorInfo(h, &cursor);

    int height = 21;
    int width = 51;
	int apples_count = 0;

	int** map = new int* [height];
	for (int i = 0; i < height; ++i)
		map[i] = new int[width];

    fill_2d_array(map, height, width);
	print_2d_array(map, height, width, h);

	COORD snake;
	snake.X = width / 2;
	snake.Y = height / 2;
	SetConsoleCursorPosition(h, snake);
	SetConsoleTextAttribute(h, Color::BLUE);
	cout << "H";

	COORD apples_info;
	apples_info.X = width + 1;
	apples_info.Y = 3;
	SetConsoleCursorPosition(h, apples_info);
	SetConsoleTextAttribute(h, Color::DARKRED);
	cout << "APPLES: ";
	SetConsoleTextAttribute(h, Color::RED);
	cout << apples_count;

	while (game_started) {
		if (_kbhit()) {
			int code = _getch();
			if (code == 224)
				code = _getch();

			if (code == Key::ESCAPE)
				break;

			update_direction_from_key(code, move_left, move_right, move_up, move_down);
		}

		COORD old_position = snake;
		bool has_been_moved = false;

		snake_move(map, width, height, has_been_moved, move_left, move_right, move_up, move_down, snake);

		if (has_been_moved) {
			SetConsoleCursorPosition(h, old_position);
			SetConsoleTextAttribute(h, Color::BLACK);
			cout << " ";
			SetConsoleCursorPosition(h, snake);
			SetConsoleTextAttribute(h, Color::BLUE);
			cout << "H";
		}

		if (map[snake.Y][snake.X] == GameObject::APPLE) {
			apples_count++;
			map[snake.Y][snake.X] = GameObject::HALL;
			COORD coins_info;
			coins_info.X = width + 1;
			coins_info.Y = 0;
			SetConsoleCursorPosition(h, coins_info);
			SetConsoleTextAttribute(h, Color::DARKRED);
			cout << "COINS: ";
			SetConsoleTextAttribute(h, Color::RED);
			cout << apples_count;
		}

		
		Sleep(200);
	}
	clear_2d_array(map, height);
	
	Sleep(INFINITE);
}