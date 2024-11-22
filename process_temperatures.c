#include <stdio.h>
#define MAX_READINGS 5
#define HIGH_TEMP_THRESHOLD 30.0
#define LOW_TEMP_THRESHOLD 10.0

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        return 1;
    }

    FILE *file = fopen(argv[1], "r");
    if (!file) {
        perror("Failed to open file");
        return 1;
    }

    double temps[MAX_READINGS];
    double temp, sum = 0;
    int count = 0;

    // Read temperatures from file
    while (count < MAX_READINGS && fscanf(file, "%lf", &temp) == 1) {
        temps[count++] = temp;
        sum += temp;
    }
    fclose(file);

    if (count == MAX_READINGS) {
        double average = sum / MAX_READINGS;
        FILE *pfile = fopen("processed_data.txt", "w");
        if (pfile) {
            fprintf(pfile, "Average Temperature: %.2f°C\n", average);
            fclose(pfile);
        }

        printf("Average Temperature: %.2f°C\n", average);
        if (average >= HIGH_TEMP_THRESHOLD) {
            printf("Alert: High temperature! Average is %.2f°C\n", average);
        } else if (average <= LOW_TEMP_THRESHOLD) {
            printf("Alert: Low temperature! Average is %.2f°C\n", average);
        }
    } else {
        printf("Error: Insufficient data.\n");
    }

    return 0;
}
