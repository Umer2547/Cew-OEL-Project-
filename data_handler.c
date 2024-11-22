#include "data_handler.h"
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include "config.h"

#define HIGH_TEMP_THRESHOLD 25.0  // High temperature threshold in Celsius
#define LOW_TEMP_THRESHOLD 5.0    // Low temperature threshold in Celsius
#define HIGH_HUMIDITY_THRESHOLD 90 // High humidity threshold in percent

// Function to save data to a file
void save_data_to_file(const char *data, const char *filepath) {
    FILE *file = fopen(filepath, "a");
    if (file == NULL) {
        fprintf(stderr, "Error opening file %s: %s\n", filepath, strerror(errno));
        return;
    }
    fprintf(file, "%s\n", data);
    fclose(file);
}

// Function to send desktop notifications
void send_notification(const char *message) {
    char cmd[1024];
    snprintf(cmd, sizeof(cmd), "notify-send 'Weather Alert' '%s'", message);
    system(cmd);  // Use system() to call notify-send
}

// Function to process weather data and generate alerts
void process_weather_data(cJSON* json) {
    if (!json) {
        fprintf(stderr, "Failed to parse data\n");
        return;
    }

    cJSON *main = cJSON_GetObjectItem(json, "main");
    if (!main) {
        fprintf(stderr, "No 'main' data found in JSON\n");
        return;
    }

    cJSON *temp = cJSON_GetObjectItem(main, "temp");
    cJSON *humidity = cJSON_GetObjectItem(main, "humidity");
    if (!temp || !humidity) {
        fprintf(stderr, "Temperature or humidity data not found\n");
        return;
    }

    double temperature = temp->valuedouble - 273.15;  // Convert from Kelvin to Celsius
    int humidity_value = humidity->valueint;

    // Time formatting for terminal output
    time_t now = time(NULL);
    struct tm *time_info = localtime(&now);
    char date_buffer[80];

    // Updated line for including time in 12-hour format with AM/PM
    strftime(date_buffer, sizeof(date_buffer), "%A, %B %d, %Y %I:%M %p", time_info);

    // Formatted terminal output
    char formatted_output[512];
    snprintf(formatted_output, sizeof(formatted_output),
             "Today's Weather Report - %s\nTemperature: %.2f°C\nHumidity: %d%%\n",
             date_buffer, temperature, humidity_value);
    printf("%s", formatted_output);

    // Save raw data in JSON format to raw data file
    char *raw_json = cJSON_PrintUnformatted(json);
    if (raw_json) {
        save_data_to_file(raw_json, RAW_DATA_PATH);
        free(raw_json);
    } else {
        fprintf(stderr, "Failed to serialize raw JSON data.\n");
    }

    // Save processed data to processed data file
    save_data_to_file(formatted_output, PROCESSED_DATA_PATH);

    // Notifications based on thresholds
    if (temperature > HIGH_TEMP_THRESHOLD) {
        send_notification("High Temperature Alert: It is hotter than 25°C. Please stay hydrated and limit sun exposure.");
    } else if (temperature < LOW_TEMP_THRESHOLD) {
        send_notification("Low Temperature Alert: It is colder than 5°C. Please wear appropriate warm clothing.");
    }
    if (humidity_value > HIGH_HUMIDITY_THRESHOLD) {
        send_notification("High Humidity Alert: Humidity levels are above 90%. It may feel uncomfortably sticky.");
    }
}