/*
 * Configuration file of WindRecorder
 *
 * This file contains declarations of the configuration data & methods
 * of WindRecorder
 * The implementations of the classes, functions and data are written in 
 * file WR_config.cxx.
 *
 * Author: Roice (LUO Bing)
 * Date: 2017-04-16 create this file
 */

#ifndef WR_CONFIG_H
#define WR_CONFIG_H

#include <string>

#ifndef SERIAL_MAX_ANEMOMETERS
#define SERIAL_MAX_ANEMOMETERS  20
#endif

typedef struct {
    /* width, length and height */
    float w;
    float l;
    float h;
} WR_Config_Arena_t;

typedef struct {
    int num_of_anemometers;
    std::string anemometer_serial_port_path[SERIAL_MAX_ANEMOMETERS];
    std::string anemometer_type[SERIAL_MAX_ANEMOMETERS];
} WR_Config_Anemometers_t;

/* configuration struct */
typedef struct {
    /* Arena */
    WR_Config_Arena_t arena;
    /* Anemometers */
    WR_Config_Anemometers_t anemo;
} WR_Config_t;

void WR_Config_restore(void);
void WR_Config_save(void);
void WR_Config_init(void);
// get pointer of configuration data
WR_Config_t* WR_Config_get_configs(void);

#endif

/* End of WR_Config.h */

