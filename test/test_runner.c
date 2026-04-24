#include "test.h"

int _tests_run = 0;
int _tests_failed = 0;

void run_unit_tests(void);
void run_board_tests(void);
void run_combat_tests(void);

int main(void) {
    run_unit_tests();
    run_board_tests();
    run_combat_tests();
    TEST_SUMMARY_AND_EXIT();
}
