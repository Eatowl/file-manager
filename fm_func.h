#ifndef FM_FUNC_H
#define FM_FUNC_H

char *malloc_array(char *array);
void realloc_all_array(unsigned length, char *directory, char *save_directory, char *temporary_directory);
bool move_between_directory(unsigned choice, char *directory, char *save_directory, char *temporary_directory, char **words);
void print_box_and_update(WINDOW **wins);
void init_wins(WINDOW **wins, int n, int row, int col);
void create_wins_and_panel(WINDOW **my_wins, PANEL **my_panels);
char **update_directory(char **words, char *directory, int *ptr);
char **sort_array(char **words, unsigned counter_sort);
void free_all(char **words, char *directory, char *save_directory, char *temporary_directory, int wordCounter);

#endif
