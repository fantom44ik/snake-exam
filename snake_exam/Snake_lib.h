#pragma once
#include <iostream>
#include <windows.h>
#include <conio.h>
#include <string>
using namespace std;

// перечисления для стен, яблок и цветов
enum GameObject : short { HALL, WALL, APPLE };
enum Color { BLACK = 1, DARKGREEN = 2, YELLOW = 14, RED = 12, BLUE = 9, WHITE = 15, DARKYELLOW = 6, DARKRED = 4 };
enum Key { ENTER = 13, ESCAPE = 27, SPACE = 32, W = 119, A = 97, S = 115, D = 100, LEFT = 75, RIGHT = 77, UP = 72, DOWN = 80, BACKSPACE = 8 };

// прототипы функций
void show_menu(bool& game_started);
void fill_2d_array(int** ar, int width, int height);
void print_2d_array(int** ar, int width, int height, HANDLE h);
void clear_2d_array(int**& ar, int width);
void update_direction_from_key(int code, bool& move_left, bool& move_right, bool& move_up, bool& move_down);
bool snake_move(int**& ar, int width, int height, bool& has_been_moved,
	bool move_left, bool move_right, bool move_up, bool move_down, COORD& snake);
void spawn_apple(int** ar, int width, int height, const COORD& snake);
void save_score_to_file(int score);
void print_scores_from_file();
void clear_scores_file();

