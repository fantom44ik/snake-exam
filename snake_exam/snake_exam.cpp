#include <iostream>
#include "Snake_lib.h"

using namespace std;

int main()
{
	// проверка на то начата ли игра
	bool game_started = false;

	// переменные для управления движением
	bool move_up = false;
	bool move_down = false;
	bool move_left = false;
	bool move_right = false;

	// вывод меню
	show_menu(game_started);

	HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
	system("title Snakek Game"); // изменение заголовка консоли

	// изменение размера, шрифта и цвета консоли
	CONSOLE_FONT_INFOEX font; // https://docs.microsoft.com/en-us/windows/console/console-font-infoex
	font.cbSize = sizeof(font);
	font.dwFontSize.Y = 25;
	font.FontFamily = FF_DONTCARE;
	font.FontWeight = FW_NORMAL;
	wcscpy_s(font.FaceName, 9, L"Consolas");
	SetCurrentConsoleFontEx(h, 0, &font);

	// скрытие курсора
	CONSOLE_CURSOR_INFO cursor;
	cursor.bVisible = false;
	cursor.dwSize = 100;
	SetConsoleCursorInfo(h, &cursor);

	// размер поля игры
	int height = 21;
	int width = 51;

	// счётчик съеденных яблок
	int apples_count = 0;

	// создание двумерного динамического массива
	int** map = new int* [height];
	for (int i = 0; i < height; ++i)
		map[i] = new int[width];

	// заполнение массива стенами и полем
	fill_2d_array(map, height, width);
	// вывод массива на экран
	print_2d_array(map, height, width, h);

	// массив для хранения сегментов змейки
	COORD snake_body[1000]; // максимум 1000 сегментов
	int snake_length = 1;
	snake_body[0].X = width / 2;
	snake_body[0].Y = height / 2;

	// отображение змейки и начального яблока
	spawn_apple(map, width, height, snake_body[0]);
	map[snake_body[0].Y][snake_body[0].X] = HALL;
	SetConsoleCursorPosition(h, snake_body[0]);
	SetConsoleTextAttribute(h, Color::BLUE);
	cout << "H";

	// информация о том как выйти из игры (через ESC)
	COORD game_exit_info;
	game_exit_info.X = width + 1;
	game_exit_info.Y = 1;
	SetConsoleCursorPosition(h, game_exit_info);
	SetConsoleTextAttribute(h, Color::YELLOW);
	cout << "Press ESC to exit the game\n";

	// информация о том как управлять змейкой
	COORD control_info;
	control_info.X = width + 1;
	control_info.Y = 3;
	SetConsoleCursorPosition(h, control_info);
	SetConsoleTextAttribute(h, Color::DARKYELLOW);
	cout << "Controls:  ";
	SetConsoleTextAttribute(h, Color::YELLOW);
	cout << "UP: W | ";
	cout << "DOWN: S | ";
	cout << "LEFT: A | ";
	cout << "RIGHT: D";

	// информация о том сколько яблок съедено
	COORD apples_info;
	apples_info.X = width + 1;
	apples_info.Y = 5;
	SetConsoleCursorPosition(h, apples_info);
	SetConsoleTextAttribute(h, Color::DARKRED);
	cout << "APPLES: ";
	SetConsoleTextAttribute(h, Color::RED);
	cout << apples_count;

	// начало игры (движок)
	while (game_started) {
		// управление движением
		if (_kbhit()) { // если нажата клавиша
			int code = _getch(); // считывание кода клавиши
			if (code == 224)
				code = _getch();

			if (code == Key::ESCAPE) {
				system("cls");
				SetConsoleTextAttribute(h, Color::DARKGREEN);
				COORD exiting_game;
				exiting_game.X = width / 2 - 5;
				exiting_game.Y = height / 2;
				SetConsoleCursorPosition(h, exiting_game);
				cout << "Exiting game...\n\n\n\n";
				Sleep(500);
				break;
			}

			// обновление направления движения
			update_direction_from_key(code, move_left, move_right, move_up, move_down);
		}

		// вычисляем новую позицию головы
		COORD new_head = snake_body[0]; // новая позиция головы
		if (move_left) new_head.X--;
		else if (move_right) new_head.X++;
		else if (move_up) new_head.Y--;
		else if (move_down) new_head.Y++;

		// Проверка на столкновение со стеной
		bool alive = true;
		if (new_head.X < 0 || new_head.X >= width || new_head.Y < 0 || new_head.Y >= height ||
			map[new_head.Y][new_head.X] == GameObject::WALL) {
			alive = false;
		}

		// Проверка на столкновение с телом
		for (int i = 1; i < snake_length; ++i) {
			if (snake_body[i].X == new_head.X && snake_body[i].Y == new_head.Y) {
				alive = false;
				break;
			}
		}

		if (!alive) {
			system("cls");
			SetConsoleTextAttribute(h, Color::RED);
			COORD game_over;
			game_over.X = width / 2 - 5;
			game_over.Y = height / 3;
			SetConsoleCursorPosition(h, game_over);
			cout << "GAME OVER";
			cout << "\n\n";
			game_over.Y = height / 3 + 2;
			SetConsoleCursorPosition(h, game_over);
			cout << "Your score: " << apples_count;
			cout << "\n\n";
			game_over.Y = height / 3 + 4;
			SetConsoleCursorPosition(h, game_over);
			cout << "Returning to main menu...\n\n\n\n";
			save_score_to_file(apples_count); // сохранение счёта в файл
			Sleep(5000);
			system("cls");
			game_started = false; // завершение игры
			show_menu(game_started); // возвращение в меню
			break;
		}

		// Сдвигаем тело змейки
		for (int i = snake_length; i > 0; --i) {
			snake_body[i] = snake_body[i - 1]; 
		}
		snake_body[0] = new_head;

		// Проверка на яблоко
		bool ate_apple = (map[new_head.Y][new_head.X] == GameObject::APPLE);
		if (ate_apple) {
			apples_count++;
			snake_length++;
			map[new_head.Y][new_head.X] = GameObject::HALL;
			COORD apples_info;
			apples_info.X = width + 1;
			apples_info.Y = 5;
			SetConsoleCursorPosition(h, apples_info);
			SetConsoleTextAttribute(h, Color::DARKRED);
			cout << "APPLES: ";
			SetConsoleTextAttribute(h, Color::RED);
			cout << apples_count << "  ";
			spawn_apple(map, width, height, snake_body[0]);
		}
		else {
			// Стираем хвост
			SetConsoleCursorPosition(h, snake_body[snake_length]);
			SetConsoleTextAttribute(h, Color::BLACK);
			cout << " ";
		}

		// Рисуем голову
		SetConsoleCursorPosition(h, snake_body[0]);
		SetConsoleTextAttribute(h, Color::BLUE);
		cout << "H";
		// Рисуем тело (опционально, можно другим цветом)
		SetConsoleTextAttribute(h, Color::BLUE);
		for (int i = 1; i < snake_length; ++i) {
			SetConsoleCursorPosition(h, snake_body[i]);
			cout << "o";
		}

		// скорость игры
		Sleep(200);
	}
	// освобождение памяти
	clear_2d_array(map, height);
	Sleep(300);
}

void show_menu(bool& game_started) {
	int choice; // переменная для выбора пункта меню

	HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
	system("title Snakek Menu"); // изменение заголовка консоли
	CONSOLE_FONT_INFOEX font; // https://docs.microsoft.com/en-us/windows/console/console-font-infoex
	font.cbSize = sizeof(font);
	font.dwFontSize.Y = 25;
	font.FontFamily = FF_DONTCARE;
	font.FontWeight = FW_NORMAL;
	wcscpy_s(font.FaceName, 9, L"Consolas");
	SetCurrentConsoleFontEx(h, 0, &font);
	SetConsoleTextAttribute(h, Color::WHITE);

	do {
		system("cls"); // очистка консоли
		cout << "=====================\n";
		cout << "  Welcome to snake!\n";
		cout << "=====================\n";
		cout << "1. Play\n";
		cout << "2. Records\n";
		cout << "3. Clear records\n";
		cout << "4. Exit\n";
		cout << "Choose an option: ";

		cin >> choice;

		// проверка на то, какой пункт меню был выбран
		switch (choice) {
		case 1:
			// запускает игру заменой переменной game_started на true
			cout << "Starting game...\n";
			Sleep(1000);
			system("cls");
			game_started = true;
			return; // выход из меню в игру
		case 2:
			// выводит результаты в консоль
			system("cls");
			print_scores_from_file(); // вывод результатов
			Sleep(2000); // задержка 2 секунды
			cout << "\n\nPress any key and you will be redirected to the main menu...\n";
			_getch(); // ожидание нажатия клавиши
			break; // возвращение в меню
		case 3:
			// очищает файл с результатами
			system("cls");
			int confirm;
			cout << "Are you sure you want to clear the records? (1 - Yes, 0 - No): "; // подтверждение очистки файла
			cin >> confirm;
			if (confirm == 1) {
				cout << "Clearing records...\n";
				Sleep(1000);
				clear_scores_file(); // очистка файла
				cout << "Records cleared.\n";
				Sleep(2000);
			}
			else {
				cout << "Records not cleared.\n";
				Sleep(2000);
			}
			break; // возвращение в меню
		case 4:
			cout << "Exiting game...\n";
			Sleep(1000);
			exit(0); // выход из программы
		default:
			// в том случае, если был введён некорректный пункт меню
			cout << "Wrong choice. Try again.\n";
			Sleep(2000);
		}
	} while (true); // бесконечный цикл, пока не выбрана команда "Exit"
}
void fill_2d_array(int** ar, int width, int height) {
	// заполнение двумерного массива стенами и полем
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
	// вывод двумерного массива на экран и задание цвета вместе с символом для стен
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
	// освобождение памяти
	for (int i = 0; i < width; ++i) {
		delete[] ar[i];
		ar[i] = nullptr;
	}
	delete[] ar;
	ar = nullptr;
}
void update_direction_from_key(int code, bool& move_left, bool& move_right, bool& move_up, bool& move_down) {
	// обновление направления движения в зависимости от нажатой клавиши
	if ((code == Key::LEFT || code == Key::A) && !move_right) {
		move_left = true;
		move_right = move_up = move_down = false;
	}
	else if ((code == Key::RIGHT || code == Key::D) && !move_left) {
		move_right = true;
		move_left = move_up = move_down = false;
	}
	else if ((code == Key::UP || code == Key::W) && !move_down) {
		move_up = true;
		move_down = move_left = move_right = false;
	}
	else if ((code == Key::DOWN || code == Key::S) && !move_up) {
		move_down = true;
		move_up = move_left = move_right = false;
	}
}
void spawn_apple(int** ar, int width, int height, const COORD& snake) {
	// создание яблока в случайной позиции
	int x, y;
	do {
		x = rand() % (width - 1);
		y = rand() % (height - 1);
	} while (ar[y][x] != HALL || (x == snake.X && y == snake.Y));

	ar[y][x] = APPLE;

	// отображение яблока на экране
	COORD apple_position;
	apple_position.X = x;
	apple_position.Y = y;
	HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleCursorPosition(h, apple_position);
	SetConsoleTextAttribute(h, Color::RED);
	cout << "o";
}
void save_score_to_file(int score) {
	// сохранение счёта в файл
	FILE* file;
	int error_code = fopen_s(&file, "snake_save.txt", "a");

	if (error_code == 0 && file != nullptr) {
		string str = to_string(score) + "\n";
		fputs(str.c_str(), file);
		fclose(file);
	}
}
void print_scores_from_file() {
	// вывод результатов из файла
	FILE* file;
	int error_code = fopen_s(&file, "snake_save.txt", "r");

	if (error_code != 0 || file == nullptr) {
		cout << "\nError: unable to open save file.\n";
		return;
	}

	char line[100];
	int index = 1;
	cout << "\n=== Previous results ===\n";
	while (fgets(line, sizeof(line), file)) {
		cout << index++ << ") Points: " << line;
	}
	cout << "========================\n";

	fclose(file);
}
void clear_scores_file() {
	// очистка файла с результатами
	FILE* file;
	int error_code = fopen_s(&file, "snake_save.txt", "w");
	if (error_code == 0 && file != nullptr) {
		fclose(file);
	}
	else {
		cout << "\nError: unable to open save file.\n";
	}
}
