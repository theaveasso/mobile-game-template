#include "test.h"

int _tests_run = 0;
int _tests_failed = 0;

void run_unit_tests(void);
void run_board_tests(void);
void run_combat_tests(void);
void run_shop_tests(void);
void run_bench_tests(void);
void run_game_tests(void);
void run_layout_tests(void);

int main(void) {
    run_unit_tests();
    run_board_tests();
    run_combat_tests();
    run_shop_tests();
    run_bench_tests();
    run_game_tests();
    run_layout_tests();
    TEST_SUMMARY_AND_EXIT();
}
