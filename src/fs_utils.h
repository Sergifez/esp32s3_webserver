#pragma once
#include <Arduino.h>

bool fs_init();
void fs_list();
bool fs_exists(const char *path);
