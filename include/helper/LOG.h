
#pragma once
#include <stdio.h>

#define LOG_INFO(...) printf("[INFO]%s:%d - ", __FILE__, __LINE__); printf(__VA_ARGS__);printf("\n")
#define LOG_ERROR(...) printf("[ERROR]%s:%d - ", __FILE__, __LINE__); printf(__VA_ARGS__);perror("\n")