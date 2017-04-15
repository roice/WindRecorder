#ifndef SERIAL_ANEMOMETERS_H
#define SERIAL_ANEMOMETERS_H

#include <time.h>
#include <vector>
#include <string>

#define SERIAL_MAX_ANEMOMETERS 20

typedef struct {
    int index;
    void* arg;
    int fd;
} Anemometer_Thread_Arguments_t;

typedef struct {
    float speed[3];
    float temperature;
    time_t t;
} Anemometer_Data_t;

bool sonic_anemometer_init(int, std::string*, std::string*);
void sonic_anemometer_close(void);
std::string* sonic_anemometer_get_port_paths(void);
std::string* sonic_anemometer_get_types(void);
Anemometer_Data_t* sonic_anemometer_get_wind_data(void);
std::vector<Anemometer_Data_t>* sonic_anemometer_get_wind_record(void);

#endif
