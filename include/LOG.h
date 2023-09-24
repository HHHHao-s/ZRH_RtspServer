
#pragma once
#include <stdio.h>

#define LOG_INFO(...) printf("[INFO]%s:%d - ", __FILE__, __LINE__); printf(__VA_ARGS__);perror("")
#define LOG_ERROR(...) printf("[ERROR]%s:%d - ", __FILE__, __LINE__); printf(__VA_ARGS__)