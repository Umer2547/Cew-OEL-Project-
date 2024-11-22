#ifndef API_H
#define API_H

#include <cjson/cJSON.h>

cJSON* fetch_weather_data(const char* city);

#endif
