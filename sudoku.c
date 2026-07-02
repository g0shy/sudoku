#include<stdio.h>
#include<termios.h>
#include<unistd.h>
#include<stdlib.h>
#include<time.h>
#define SHOW_CURSOR      "\033[?25h"
#define HIDE_CURSOR      "\033[?25l"
#define CLEAR_SCREEN     "\033[2J"
#define HOME             "\033[H"
#define COLOR_HIGHLIGHT  "\033[46m"
#define COLOR_CURSOR     "\033[43m"
#define COLOR_RESET      "\033[0m"
#define COLOR_TEXT       "\033[34m"

struct termios orig_termios;

typedef struct {
	int grid[9][9];
	int fixed[9][9];
	int solution[9][9];
	int difficulty;
} sudoku;

void init_empty_grid(sudoku *s) {
	for(int i = 0; i < 9; i++) {
		for(int j = 0; j < 9; j++) {
			s->grid[i][j] = 0;
			s->fixed[i][j] = 0;
			s->solution[i][j] = 0;
		}
	}
};
void init_base_grid(sudoku *s) {
	int base[9][9] = {
		{1,2,3,4,5,6,7,8,9},
		{4,5,6,7,8,9,1,2,3},
		{7,8,9,1,2,3,4,5,6},
		{2,3,4,5,6,7,8,9,1},
		{5,6,7,8,9,1,2,3,4},
		{8,9,1,2,3,4,5,6,7},
		{3,4,5,6,7,8,9,1,2},
		{6,7,8,9,1,2,3,4,5},
		{9,1,2,3,4,5,6,7,8}
	};
	for(int i = 0; i < 9; i++) {
		for(int j = 0; j < 9; j++) {
			s->grid[i][j] = base[i][j];
			s->solution[i][j] = base[i][j];
			s->fixed[i][j] = 0;
		}
	}
}
void transpose(int grid[9][9]) {
	for(int i = 0; i < 9; i++) {
		for(int j = i + 1; j < 9; j++) {
			int temp = grid[i][j];
			grid[i][j] = grid[j][i];
			grid[j][i] = temp;
		}
	}
};
void swap_rows_small(int grid[9][9], int area, int row1, int row2) {
	for(int col = 0; col < 9; col++) {
		int temp = grid[area*3 + row1][col];
		grid[area*3 + row1][col] = grid[area*3 + row2][col];
		grid[area*3 + row2][col] = temp; 
	}
};
void swap_columns_small(int grid[9][9], int area, int col1, int col2) {
	for(int row = 0; row < 9; row++) {
		int temp = grid[row][area*3 + col1];
		grid[row][area*3 + col1] = grid[row][area*3 + col2];
		grid[row][area*3 + col2] = temp; 
	}
};
void swap_rows_area(int grid[9][9], int area1, int area2) {
	for(int i = 0; i < 3; i++) {
		for(int col = 0; col < 9; col++) {
			int temp = grid[area1*3 + i][col];
			grid[area1*3 + i][col] = grid[area2*3 + i][col];
			grid[area2*3 + i][col] = temp;
		}
	}
};
void swap_columns_area(int grid[9][9], int area1, int area2) {
	for(int i = 0; i < 3; i++) {
		for(int row = 0; row < 9; row++) {
			int temp = grid[row][area1*3 + i];
			grid[row][area1*3 + i] = grid[row][area2*3 + i];
			grid[row][area2*3 + i] = temp;
		}
	}
};
void mix_sudoku(int grid[9][9]) {
	for(int i = 0; i < 500; i++) {
		int operation = rand() % 5;
		switch(operation) {
			case 0:
				transpose(grid);
				break;
			case 1:
				swap_rows_small(grid, rand() % 3, rand() % 3, rand() % 3);
				break;
			case 2:
				swap_columns_small(grid, rand() % 3, rand() % 3, rand() % 3);
				break;
			case 3:
				swap_rows_area(grid, rand() % 3, rand() % 3);
				break;
			case 4:
				swap_columns_area(grid, rand() % 3, rand() % 3);
				break;
		}
	}
};
int is_safe(int grid[9][9], int row, int col, int num) {
	for(int i = 0; i < 9; i++) {
		if (grid[i][col] == num) return 0;
	}
	for(int j = 0; j < 9; j++) {
		if (grid[row][j] == num) return 0;
	}
	int start_row = (row / 3) * 3;
	int start_col = (col / 3) * 3;
	for(int i = 0; i < 3; i++) {
		for(int j = 0; j < 3; j++) {
			if(grid[start_row + i][start_col + j] == num) return 0;
		}
	}
	return 1;
}
int solve_sudoku(int grid[9][9]) {
	for(int row = 0; row < 9; row++) {
		for(int col = 0; col < 9; col++) {
			if (grid[row][col] == 0) {
				for(int num = 1; num <= 9; num++) {
					if (is_safe(grid, row, col, num)) {
						grid[row][col] = num;
						if (solve_sudoku(grid)) return 1;
						grid[row][col] = 0;
					}
				}
			}
		}
	}
	return 1;
}
int is_valid_sudoku(int grid[9][9]) {
	for(int row = 0; row < 9; row++) {
		for(int col = 0; col < 9; col++) {
			if (grid[row][col] != 0) {
				int num = grid[row][col];
				grid[row][col] = 0;
				if (!is_safe(grid, row, col, num)) {
					grid[row][col] = num;
					return 0;
				}
				grid[row][col] = num;
			}
		}
	}
	return 1;
}
void count_solutions(int grid[9][9], int *solutions, int limit) {
	for(int row = 0; row < 9; row++) {
		for(int col = 0; col < 9; col++) {
			if (grid[row][col] == 0) {
				for(int num = 1; num <= 9; num++) {
					if (is_safe(grid, row, col, num)) {
						grid[row][col] = num;
						count_solutions(grid, solutions, limit);
						if (*solutions >= limit) return;
						grid[row][col] = 0;
					}
				}
				return;
			}
		}
	}
	(*solutions)++;
}
int has_unique_solution(int grid[9][9]) {
	int temp[9][9];
	for(int i = 0; i < 9; i++) {
		for(int j = 0; j < 9; j++) {
			temp[i][j] = grid[i][j];
		}
	}
	int solutions = 0;
	count_solutions(temp, &solutions, 2);
	return solutions == 1;
}
void remove_cells(sudoku *s, int cells_to_remove) {
	int removed = 0;
	while(removed < cells_to_remove) {
		int row = rand() % 9;
		int col = rand() % 9;
		if (s->grid[row][col] != 0) {
			int backup = s->grid[row][col];
			s->grid[row][col] = 0;

			if (has_unique_solution(s->grid)) {
				s->fixed[row][col] = 0;
				removed++;
			}
			else {
				s->grid[row][col] = backup;
			}
		}
	}
}
void disable_raw_mode() {
	tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
	printf("%s",SHOW_CURSOR);
	fflush(stdout);
}
void enable_raw_mode() {
	tcgetattr(STDIN_FILENO, &orig_termios);
	atexit(disable_raw_mode);
	struct termios raw = orig_termios;
	raw.c_lflag &= ~(ICANON | ECHO);
	tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
	printf("%s", HIDE_CURSOR);
	fflush(stdout);
}
void clear_screen() {
	printf("%s", CLEAR_SCREEN);
	printf("%s", HOME);
	fflush(stdout);
}
char read_key() {
	char c;
	while(read(STDIN_FILENO, &c, 1) != 1);
	return c;
}
void wait_enter() {
	printf("\n\nНажмите ENTER для продолжения...\n");
	fflush(stdout);
	int c;
	while (1) {
		c = read_key();
		if (c == '\n' || c == '\r') break;
	}
}
void show_error_message() {
	printf("\033[25;1H");
	printf("Цифра нарушает правила");
	fflush(stdout);
	wait_enter();
	printf(HOME);
	fflush(stdout);
}
void screensaver() {
	clear_screen();
	printf("\nSudoku, Copyright 2026\n");
	printf("        By Georgij Trifonuyk\n");
	printf("        Version 0.0.1\n");
	wait_enter();
}
void help() {
	clear_screen();
	printf("\nСправка по работе с программой\n\n");
	printf("Перемещение по пунктам меню - ↑,↓\n");
	printf("Подтвердить выбор - ENTER\n");
	printf("Справка - H\n");
	printf("Выход - Q\n");
	printf("Решить судоку - S\n");
	printf("Поставить цийру - 1-9\n");
	printf("Удалить цифру - 0 или D\n");
	wait_enter();
}
void quit() {
	clear_screen();
	printf("Вы уверены, что хотите выйти?[Y]\n");
	fflush(stdout);
	char c = read_key();
	if (c == 'y' || c == 'Y') {
		clear_screen();
		exit(0);
	}
	return;
}
void author_info() {
	clear_screen();
	printf("Информация об авторе\n\n");
	printf("Студент: Георгий Трифонюк\n");
	wait_enter();
}
int run_menu() {

	int choice = 1;
	char c;

	while(1) {
		clear_screen();
		printf("\n");
		printf("╔═════════════════════════╗\n");
		printf("║ ░░░ ░ ░ ░░  ░░░ ░ ░ ░ ░ ║\n");
		printf("║ ░   ░ ░ ░ ░ ░ ░ ░ ░ ░ ░ ║\n");
		printf("║ ░░░ ░ ░ ░ ░ ░ ░ ░░  ░ ░ ║\n");
		printf("║   ░ ░ ░ ░ ░ ░ ░ ░ ░ ░ ░ ║\n");
		printf("║ ░░░ ░░░ ░░  ░░░ ░ ░ ░░░ ║\n");
		printf("╚═════════════════════════╝\n");
		printf("%s╔═════════════════════════╗%s\n", choice == 1 ? COLOR_HIGHLIGHT : "", COLOR_RESET);
		printf("%s║ Новая игра              ║%s\n", choice == 1 ? COLOR_HIGHLIGHT : "", COLOR_RESET);
		printf("%s╚═════════════════════════╝%s\n", choice == 1 ? COLOR_HIGHLIGHT : "", COLOR_RESET);
		printf("%s╔═════════════════════════╗%s\n", choice == 2 ? COLOR_HIGHLIGHT : "", COLOR_RESET);
		printf("%s║ Ввести головоломку      ║%s\n", choice == 2 ? COLOR_HIGHLIGHT : "", COLOR_RESET);
		printf("%s╚═════════════════════════╝%s\n", choice == 2 ? COLOR_HIGHLIGHT : "", COLOR_RESET);
		printf("%s╔═════════════════════════╗%s\n", choice == 3 ? COLOR_HIGHLIGHT : "", COLOR_RESET);
		printf("%s║ Информация об авторе    ║%s\n", choice == 3 ? COLOR_HIGHLIGHT : "", COLOR_RESET);
		printf("%s╚═════════════════════════╝%s\n", choice == 3 ? COLOR_HIGHLIGHT : "", COLOR_RESET);
		printf("%s╔═════════════════════════╗%s\n", choice == 4 ? COLOR_HIGHLIGHT : "", COLOR_RESET);
		printf("%s║ Выход                   ║%s\n", choice == 4 ? COLOR_HIGHLIGHT : "", COLOR_RESET);
		printf("%s╚═════════════════════════╝%s\n", choice == 4 ? COLOR_HIGHLIGHT : "", COLOR_RESET);
		printf("\nq - Выход, h - Справка\n");

		c = read_key();
		if (c == 27) {
			char seq1 = read_key();
			char seq2 = read_key();
			if (seq1 == 91) {
				if (seq2 == 'A') choice--;
				else if (seq2 == 'B') choice++; 
				if (choice<1) choice = 4;
				if (choice>4) choice = 1;
			}
		}
		else if (c == '\n' || c == '\r') return choice; 
		else if (c == 'h' || c == 'H') help(); 
		else if (c == 'q' || c == 'Q') return 4;
	}
}
void generate_sudoku(sudoku *s, int difficulty) {
	if (difficulty == 4) return;
	s->difficulty = difficulty;
	init_base_grid(s);
	mix_sudoku(s->grid);
	for(int i = 0; i < 9; i++) {
		for(int j = 0; j < 9; j++) {
			s->solution[i][j] = s->grid[i][j];
			s->fixed[i][j] = 1;
		}
	}
	int cells_to_remove;
	switch(difficulty) {
		case 1:
			cells_to_remove = 40;
			break;
		case 2:
			cells_to_remove = 50;
			break;
		case 3:
			cells_to_remove = 55;
			break;
	}
	remove_cells(s, cells_to_remove);
}
int start_new_game(sudoku *s) {
	int choice = 1;
	char c;

	while(1) {
		clear_screen();
		printf("\n");
		printf("Выберите уровень сложности\n");
		printf("%s╔═════════════════════════╗%s\n", choice == 1 ? COLOR_HIGHLIGHT : "", COLOR_RESET);
		printf("%s║ Лёгкий                  ║%s\n", choice == 1 ? COLOR_HIGHLIGHT : "", COLOR_RESET);
		printf("%s╚═════════════════════════╝%s\n", choice == 1 ? COLOR_HIGHLIGHT : "", COLOR_RESET);
		printf("%s╔═════════════════════════╗%s\n", choice == 2 ? COLOR_HIGHLIGHT : "", COLOR_RESET);
		printf("%s║ Средний                 ║%s\n", choice == 2 ? COLOR_HIGHLIGHT : "", COLOR_RESET);
		printf("%s╚═════════════════════════╝%s\n", choice == 2 ? COLOR_HIGHLIGHT : "", COLOR_RESET);
		printf("%s╔═════════════════════════╗%s\n", choice == 3 ? COLOR_HIGHLIGHT : "", COLOR_RESET);
		printf("%s║ Сложный                 ║%s\n", choice == 3 ? COLOR_HIGHLIGHT : "", COLOR_RESET);
		printf("%s╚═════════════════════════╝%s\n", choice == 3 ? COLOR_HIGHLIGHT : "", COLOR_RESET);
		printf("%s╔═════════════════════════╗%s\n", choice == 4 ? COLOR_HIGHLIGHT : "", COLOR_RESET);
		printf("%s║ Выйти в меню            ║%s\n", choice == 4 ? COLOR_HIGHLIGHT : "", COLOR_RESET);
		printf("%s╚═════════════════════════╝%s\n", choice == 4 ? COLOR_HIGHLIGHT : "", COLOR_RESET);
	
		printf("\nq - Выход, h - Справка\n");
		c = read_key();
		
		if (c == 27) {
			char seq1 = read_key();
			char seq2 = read_key();
			if (seq1 == 91) {
				if (seq2 == 'A') choice--;
				else if (seq2 == 'B') choice++; 
				if (choice < 1) choice = 4;
				if (choice > 4) choice = 1;
			}
		}
		else if (c == '\n' || c == '\r') {
			if(choice != 4) {
				generate_sudoku(s, choice);
			}
			return choice;
		}
		else if (c == 'h' || c == 'H') help();
		else if (c == 'q' || c == 'Q') quit();
	}
}

void print_sudoku_game(sudoku *s, int cursor_x, int cursor_y) {
	clear_screen();
	printf("\n");
	printf("╔═══════════════════════════════════╗\n");
	printf("║         SUDOKU BY GEORGIJ         ║\n");
	printf("╚═══════════════════════════════════╝\n");
	printf("╔═══╤═══╤═══╦═══╤═══╤═══╦═══╤═══╤═══╗\n");
	for(int row = 0; row < 9; row++) {
		for(int col = 0; col < 9; col++) {
			if(col % 3 == 0) printf("║");
			else printf("│");
			if (row == cursor_y && col == cursor_x) {
				printf(COLOR_CURSOR);
				if (s->grid[row][col] == 0) printf("   ");
				else printf(" %d ", s->grid[row][col]);
				printf(COLOR_RESET);
			}
			else if (s->fixed[row][col] == 1 && s->grid[row][col] != 0) {
				printf(COLOR_TEXT);
				printf(" %d ", s->grid[row][col]);
				printf(COLOR_RESET);
			}
			else if (s->grid[row][col] == 0) printf("   ");
			else printf(" %d ", s->grid[row][col]);
		}
		printf("║\n");
		if(row < 8) {
			if(row == 2 || row == 5) printf("╠═══╪═══╪═══╬═══╪═══╪═══╬═══╪═══╪═══╣\n");
			else printf("╟───┼───┼───╫───┼───┼───╫───┼───┼───╢\n");
		}
	}
	printf("╚═══╧═══╧═══╩═══╧═══╧═══╩═══╧═══╧═══╝\n");
	printf("H - справка | Q - выход\n");
}
int check_win(sudoku *s) {
	for(int i = 0; i < 9; i++) {
		for(int j = 0; j < 9; j++) {
			if (s->grid[i][j] == 0) return 0;
		}
	}
	return is_valid_sudoku(s->grid);
}
int solve_sudoku_visual(sudoku *s) {
	int delay;
	switch(s->difficulty) {
		case 1:
			delay = 30000;
			break;
		case 2:
			delay = 20000;
			break;
		case 3:
			delay = 100;
			break;
	}	
	for(int row = 0; row < 9; row++) {
		for(int col = 0; col < 9; col++) {
			if (s->grid[row][col] == 0) {
				for(int num = 1; num <= 9; num++) {
					if (is_safe(s->grid, row, col, num)) {
						s->grid[row][col] = num;
						clear_screen();
						print_sudoku_game(s, col, row);
						usleep(delay);
						if(solve_sudoku_visual(s)) return 1;
						s->grid[row][col] = 0;
					}
				}
				return 0;
			}
		}
	}
	return 1;
}
void reset_to_fixed(sudoku *s) {
	for(int i = 0; i < 9; i++) {
		for(int j = 0; j < 9; j++) {
			if (s->fixed[i][j] == 0) {
				s->grid[i][j] = 0;
			}
		}
	}
}
int run_game(sudoku *s) {
	int cursor_x = 0;
	int cursor_y = 0;
	char c;
	while(1) {
		print_sudoku_game(s, cursor_x, cursor_y);
		c = read_key();
		if (c == 27) {
			char seq1 = read_key();
			char seq2 = read_key();
			if (seq1 == 91) {
				switch(seq2) {
					case 'A': if (cursor_y > 0) cursor_y--; break;
					case 'B': if (cursor_y < 8) cursor_y++; break;
					case 'C': if (cursor_x < 8) cursor_x++; break;
					case 'D': if (cursor_x > 0) cursor_x--; break;
				}
			}
		}
		else if (c>='1' && c <= '9') {
			if(s->fixed[cursor_y][cursor_x] == 0) {
				int num = c - '0';
				int backup = s->grid[cursor_y][cursor_x];
				s->grid[cursor_y][cursor_x] = num;

				if (is_valid_sudoku(s->grid)) {
					if (check_win(s)) {
						clear_screen();
						print_sudoku_game(s, cursor_x, cursor_y);
						printf("\n Победа!");
						wait_enter();
						break;
					}
				}
				else {
					s->grid[cursor_y][cursor_x] = backup;
					show_error_message();
				}
			}
		}
		else if (c == 127 || c == 'd' || c == 'D') {
			if (s->fixed[cursor_y][cursor_x] == 0) {
				s->grid[cursor_y][cursor_x] = 0;
			}
		}
		else if (c == 's' || c == 'S') {
			reset_to_fixed(s);
			solve_sudoku_visual(s);
			if (check_win(s)) {
				clear_screen();
				print_sudoku_game(s, cursor_x, cursor_y);
				printf("\nпобеда!\n");
				wait_enter();
				break;
			}
		}
		else if (c == 'h' || c == 'H') help();
		else if (c == 'q' || c == 'Q') {
			clear_screen();
			printf("Выйти в меню? [Y]\n");
			char confirm = read_key();
			if (confirm == 'y' || confirm == 'Y') break;
		}
	}
}
void print_sudoku_input(sudoku *s, int cursor_x, int cursor_y) {
	clear_screen();
	printf("\n");
	printf("╔═══════════════════════════════════╗\n");
	printf("║            ВВОД СУДОКУ            ║\n");
	printf("╚═══════════════════════════════════╝\n");
	printf("╔═══╤═══╤═══╦═══╤═══╤═══╦═══╤═══╤═══╗\n");
	for(int row = 0; row < 9; row++) {
		for(int col = 0; col < 9; col++) {
			if(col % 3 == 0) printf("║");
			else printf("│");
			if (row == cursor_y && col == cursor_x) {
				printf(COLOR_CURSOR);
				if (s->grid[row][col] == 0) printf("   ");
				else printf(" %d ", s->grid[row][col]);
				printf(COLOR_RESET);
			}
			else if (s->grid[row][col] == 0) printf("   ");
			else printf(" %d ", s->grid[row][col]);
		}
		printf("║\n");
		if(row < 8) {
			if(row == 2 || row == 5) printf("╠═══╪═══╪═══╬═══╪═══╪═══╬═══╪═══╪═══╣\n");
			else printf("╟───┼───┼───╫───┼───┼───╫───┼───┼───╢\n");
		}
	}
	printf("╚═══╧═══╧═══╩═══╧═══╧═══╩═══╧═══╧═══╝\n");
	printf("H - справка | Q - выход\n");
}
int input_sudoku(sudoku *s) {
	init_empty_grid(s);
	s->difficulty = 3;
	int cursor_x = 0;
	int cursor_y = 0;
	char c;
	while(1) {
		print_sudoku_input(s, cursor_x, cursor_y);
		c = read_key();
		if (c == 27) {
			char seq1 = read_key();
			char seq2 = read_key();
			if (seq1 == 91) {
				switch(seq2) {
					case 'A': if (cursor_y > 0) cursor_y--; break;
					case 'B': if (cursor_y < 8) cursor_y++; break;
					case 'C': if (cursor_x < 8) cursor_x++; break;
					case 'D': if (cursor_x > 0) cursor_x--; break;
				}
			}
		}
		else if (c>='1' && c <= '9') s->grid[cursor_y][cursor_x] = c - '0';
		else if (c == 127 || c == 'd' || c == 'D') s->grid[cursor_y][cursor_x] = 0;
		else if (c == '\n' || c == '\r') {
			if (is_valid_sudoku(s->grid)) {
				for(int i = 0; i < 9; i++) {
					for(int j = 0; j < 9; j++) {
						if(s->grid[i][j] != 0) s->fixed[i][j] = 1;
						else s->fixed[i][j] = 0;
					}
				}
			
			for(int i = 0; i < 9; i++) {
				for(int j = 0; j < 9; j++) {
					s->solution[i][j] = s->grid[i][j];
				}
			}
			solve_sudoku(s->solution);
			return 1;
			}
			else {
				clear_screen();
				printf("Некорректное судоку\n");
				wait_enter();
			}
		}
		else if (c == 'h' || c == 'H') help();
		else if (c == 'q' || c == 'Q') {
			clear_screen();
			printf("Выйти в меню? [Y]\n");
			char confirm = read_key();
			if (confirm == 'y' || confirm == 'Y') break;
		}

	}
}

int main() {
	srand(time(NULL));
	sudoku game;
	enable_raw_mode();
	screensaver();
	clear_screen();
	while(1) {
		switch(run_menu()) {
			case 1:
				int diff = start_new_game(&game);
				if(diff != 4) run_game(&game);
				break;
			case 2:
				if(input_sudoku(&game) == 1) run_game(&game);
				break;
			case 3:
				author_info();
				break;
			case 4:
				quit();
				break;
		}
	}
	return 0;
}
