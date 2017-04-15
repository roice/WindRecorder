/*
 * Anemometers
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <pthread.h>
#include <time.h> // nanosleep()
#include <vector>
#include <cmath>
#include "io/serial.h"
#include "io/serial_anemometers.h"
#include "io/serial_gill.h"

static int num_ports = 0;
static int fd[SERIAL_MAX_ANEMOMETERS]; // max number of sensors supported
static pthread_t    read_thread_handle[SERIAL_MAX_ANEMOMETERS];
static bool     exit_thread = false;

static Anemometer_Thread_Arguments_t  thread_args[SERIAL_MAX_ANEMOMETERS];
Anemometer_Data_t   wind_data[SERIAL_MAX_ANEMOMETERS];
std::vector<Anemometer_Data_t> wind_record[SERIAL_MAX_ANEMOMETERS];
std::string anemometer_port_path[SERIAL_MAX_ANEMOMETERS];
std::string anemometer_type[SERIAL_MAX_ANEMOMETERS];

bool sonic_anemometer_init(int n_ports = 1, std::string* ports = NULL, std::string* types = NULL)
{
    if (n_ports < 1 or n_ports > SERIAL_MAX_ANEMOMETERS)
        return false;

    if (!ports or !types) return false;

    // open serial port
    for (int i = 0; i < n_ports; i++) {
        fd[i] = serial_open(ports[i].c_str()); // blocking
        if (fd[i] == -1)
            return false;
        if (types[i] == "Gill WindSonic") {
            if (!serial_setup(fd[i], 9600)) // N81
                return false;
        }
        else if (types[i] == "Gill WindMaster") {
            if (!serial_setup(fd[i], 115200)) // N81
                return false;
        }
        else
            return false; // type not recognized
    }

    // create thread for receiving anemometer measurements
    exit_thread = false;
    num_ports = n_ports;

    for (int i = 0; i < n_ports; i++) {
        thread_args[i].arg = &exit_thread;
        thread_args[i].index = i;
        thread_args[i].fd = fd[i];
        if (types[i] == "Gill WindSonic") {
            if (pthread_create(&read_thread_handle[i], NULL, &gill_windsonic_read_loop, (void*)&thread_args[i]) != 0)
                return false;
        }
        else if (types[i] == "Gill WindMaster") {
            if (pthread_create(&read_thread_handle[i], NULL, &gill_windmaster_read_loop, (void*)&thread_args[i]) != 0)
                return false;
        }
    }

    return true;
}

void sonic_anemometer_close(void)
{
    if (!exit_thread and num_ports) // if still running
    {
        // exit threads
        exit_thread = true;
        for (int i = 0; i < num_ports; i++)
            pthread_join(read_thread_handle[i], NULL);
        // close serial port
        for (int i = 0; i < num_ports; i++) 
            serial_close(fd[i]);
        printf("Anemometer serial thread terminated.\n");
    }
}

std::string* sonic_anemometer_get_port_paths(void)
{
    return anemometer_port_path;
}

std::string* sonic_anemometer_get_types(void)
{
    return anemometer_type;
}

std::vector<Anemometer_Data_t>* sonic_anemometer_get_wind_record(void)
{
    return wind_record;
}

Anemometer_Data_t* sonic_anemometer_get_wind_data(void)
{
    return wind_data;
}
