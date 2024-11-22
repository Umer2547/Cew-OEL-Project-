#ifndef DATA_HANDLER_H
#define DATA_HANDLER_H

#include <cjson/cJSON.h>

void process_weather_data(cJSON* json);
void save_raw_data_to_file(const char *data);
void save_processed_data_to_file(const char *data);

#endif
