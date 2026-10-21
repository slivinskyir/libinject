python3 ../inject.py . -o tests/base/injected_functions.cpp

mkdir -p build/
# -MJ ../compile_commands.json 
clang++ -O0 -std=c++20 -DINJECT_AVAILABLE -I. -I../. -o build/tests\
                       app.cpp \
                       tests/base/injected_functions.cpp \
                       tests/base/base.cpp \
                       tests/test_main.cpp \
                       tests/test_main_2.cpp 
