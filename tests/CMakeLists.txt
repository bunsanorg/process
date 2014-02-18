bunsan_testing_project_header()
bunsan_testing_project_aux_add_tests()

bunsan_tests_project_add_test(
    ${platform_tests}_process
    ${platform_tests}/process.cpp
)

bunsan_testing_project_footer()
