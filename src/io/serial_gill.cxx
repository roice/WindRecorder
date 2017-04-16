/*
 * Serial Protocal for Gill Anemometers
 * Support List:
 *      Gill 2D sonic wind sensor --- WindSonic
 *      Gill 3D sonic wind sensor --- WindMaster (TODO)
 *
 * Author:
 *      Roice Luo (Bing Luo)
 * Date:
 *      2017.04.15      create this file
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <time.h> // nanosleep()
#include <vector>
#include <cmath>
#include "serial.h"
#include "serial_anemometers.h"

// Gill WindSonic Frame Type
typedef struct {
    // protocol
    char protocol[50] = {0x02, 'X', ',', 'X', 'X', 'X', ',', 'X', 'X', 'X', '.', 'X', 'X', ',', 'X', ',', 'X', 'X', ',', 0x03, 'C'};
    // frame
    char frame[100];
    // checksum
    char checksum;
    // pointer
    int pointer = 0;
    // parsed value
    char    addr; // WindSonic node address
    float   direction;  // wind direction (deg)
    float   speed;  // wind speed
    char    units; // units of measure (knots, m/s etc.)
    char    status; // anemometer status code
} Gill_WindSonic_ASCII_Frame_t;
typedef struct {
    // protocol
    char protocol[50] = {0x02, 'X', 'X', ',', 'X', 'X', ',', 'S', 'X', 'X', '.', 'X', 'X', ',', 'S', 'X', 'X', '.', 'X', 'X', ',', 'S', 'X', 'X', '.', 'X', 'X', ',', 'S', 'X', 'X', '.', 'X', 'X', ',',0x03};
    // frame
    char frame[100];
    // pointer
    int pointer = 0;
    // parsed value
    short    StaA; // status address
    short    StaD; // status data
    float  u;  // U vector cm/s
    float  v;  // V vector cm/s
    float  w;  // W vector cm/s
    float  T;  // absolute temperature
} Gill_WindMaster_ASCII_Frame_t;

static Gill_WindSonic_ASCII_Frame_t gill_frame_windsonic[SERIAL_MAX_ANEMOMETERS];
static Gill_WindMaster_ASCII_Frame_t gill_frame_windmaster[SERIAL_MAX_ANEMOMETERS];

static void gillProcessFrame_WindMaster(char* buf, int len, int index)
{
    if (index < 0 or index >= SERIAL_MAX_ANEMOMETERS)
        return;

    if (len <= 0)
        return;

    for (int i = 0; i < len; i++) {
        switch (gill_frame_windmaster[index].protocol[gill_frame_windmaster[index].pointer]) {
            case 0x02: // start frame
                if (buf[i] == 0x02)
                    gill_frame_windmaster[index].frame[gill_frame_windmaster[index].pointer++] = buf[i];
                break;
            case 'X': // number, not ',' or '.', or '+', or '-'
                if (buf[i] != ',' and buf[i] != '.' and buf[i] != '+' and buf[i] != '-')
                    gill_frame_windmaster[index].frame[gill_frame_windmaster[index].pointer++] = buf[i];
                else
                    gill_frame_windmaster[index].pointer = 0;
                break;
            case 'S': // '+' or '-'
                if (buf[i] == '+' or buf[i] == '-')
                    gill_frame_windmaster[index].frame[gill_frame_windmaster[index].pointer++] = buf[i];
                else
                    gill_frame_windmaster[index].pointer = 0;
                break;
            case ',':
                if (buf[i] == ',')
                    gill_frame_windmaster[index].frame[gill_frame_windmaster[index].pointer++] = buf[i];
                else
                    gill_frame_windmaster[index].pointer = 0;
                break;
            case '.':
                if (buf[i] == '.')
                    gill_frame_windmaster[index].frame[gill_frame_windmaster[index].pointer++] = buf[i];
                else
                    gill_frame_windmaster[index].pointer = 0;
                break;
            case 0x03: // end frame
                if (buf[i] == 0x03) { // received a complete frame
                    gill_frame_windmaster[index].frame[gill_frame_windmaster[index].pointer++] = buf[i]; 
// Debug
    //gill_frame_windmaster[index].frame[gill_frame_windmaster[index].pointer] = 0x0;
    //printf("gill_frame_windmaster[index] = %s\n", gill_frame_windmaster[index].frame);

                    gill_frame_windmaster[index].StaA = ((short)gill_frame_windmaster[index].frame[1] << 8) & ((short)gill_frame_windmaster[index].frame[2]);
                    gill_frame_windmaster[index].StaD = ((short)gill_frame_windmaster[index].frame[4] << 8) & ((short)gill_frame_windmaster[index].frame[5]);
                    char temp_f[10] = {0};
                    memcpy(temp_f, &gill_frame_windmaster[index].frame[7], 6*sizeof(char));
                    gill_frame_windmaster[index].u = atof(temp_f);
                    memcpy(temp_f, &gill_frame_windmaster[index].frame[14], 6*sizeof(char));
                    gill_frame_windmaster[index].v = atof(temp_f);
                    memcpy(temp_f, &gill_frame_windmaster[index].frame[21], 6*sizeof(char));
                    gill_frame_windmaster[index].w = atof(temp_f);
                    memcpy(temp_f, &gill_frame_windmaster[index].frame[28], 6*sizeof(char));
                    gill_frame_windmaster[index].T = atof(temp_f);

// Debug
    //printf("wind = [%f, %f, %f], T = %f\n", gill_frame_windmaster[index].u, gill_frame_windmaster[index].v, gill_frame_windmaster[index].w, gill_frame_windmaster[index].T);

                    // save data
                    Anemometer_Data_t *wind_data = sonic_anemometer_get_wind_data();
                    wind_data[index].speed[0] = (float)gill_frame_windmaster[index].u;
                    wind_data[index].speed[1] = (float)gill_frame_windmaster[index].v;
                    wind_data[index].speed[2] = (float)gill_frame_windmaster[index].w;
                    wind_data[index].temperature = (float)gill_frame_windmaster[index].T;

                    gill_frame_windmaster[index].pointer = 0; // clear pointer
                }
                break;
            default:
                break;
        }
    }
}

// Gill WindSonic Polar Continuous
void gillProcessFrame_WindSonic(char* buf, int len, int index)
{
    if (index < 0 or index >= SERIAL_MAX_ANEMOMETERS)
        return;

    if (len <= 0)
        return;

    for (int i = 0; i < len; i++) {
        switch (gill_frame_windsonic[index].protocol[gill_frame_windsonic[index].pointer]) {
            case 0x02: // start frame
                if (buf[i] == 0x02) {
                    gill_frame_windsonic[index].frame[gill_frame_windsonic[index].pointer++] = buf[i];
                    gill_frame_windsonic[index].checksum = 0;
                }
                break;
            case 'X': // number, not ',' or '.'
                if (buf[i] != ',' and buf[i] != '.') { // normal
                    gill_frame_windsonic[index].frame[gill_frame_windsonic[index].pointer++] = buf[i];
                    gill_frame_windsonic[index].checksum ^= buf[i];
                }
                else if (buf[i] == ',' and gill_frame_windsonic[index].pointer == 3) { // wind speed < 0.05 m/s, no direction data
                    gill_frame_windsonic[index].frame[gill_frame_windsonic[index].pointer++] = 0;
                    gill_frame_windsonic[index].frame[gill_frame_windsonic[index].pointer++] = 0;
                    gill_frame_windsonic[index].frame[gill_frame_windsonic[index].pointer++] = 0;
                    gill_frame_windsonic[index].frame[gill_frame_windsonic[index].pointer++] = buf[i];
                    gill_frame_windsonic[index].checksum ^= buf[i];
                }
                else {
                    gill_frame_windsonic[index].pointer = 0;
                    gill_frame_windsonic[index].checksum = 0;
                }
                break;
            case ',':
                if (buf[i] == ',') {
                    gill_frame_windsonic[index].frame[gill_frame_windsonic[index].pointer++] = buf[i];
                    gill_frame_windsonic[index].checksum ^= buf[i];
                }
                else {
                    gill_frame_windsonic[index].pointer = 0;
                    gill_frame_windsonic[index].checksum = 0;
                }
                break;
            case '.':
                if (buf[i] == '.') {
                    gill_frame_windsonic[index].frame[gill_frame_windsonic[index].pointer++] = buf[i];
                    gill_frame_windsonic[index].checksum ^= buf[i];
                }
                else {
                    gill_frame_windsonic[index].pointer = 0;
                    gill_frame_windsonic[index].checksum = 0;
                }
                break;
            case 0x03:
                if (buf[i] == 0x03)
                    gill_frame_windsonic[index].frame[gill_frame_windsonic[index].pointer++] = buf[i];
                else
                    gill_frame_windsonic[index].pointer = 0;
                break;
            case 'C': // received checksum
                gill_frame_windsonic[index].frame[gill_frame_windsonic[index].pointer] = buf[i];
                gill_frame_windsonic[index].pointer = 0; // clear pointer
                if (buf[i] != gill_frame_windsonic[index].checksum)
                    break;
                else { // received a complete frame
// Debug
    //gill_frame_windsonic[index].frame[gill_frame_windsonic[index].pointer] = 0x0;
    printf("gill_frame_windsonic[index] = %s\n", gill_frame_windsonic[index].frame);

                    gill_frame_windsonic[index].addr = (char)gill_frame_windsonic[index].frame[1];
                    char temp_f[10] = {0};
                    memcpy(temp_f, &gill_frame_windsonic[index].frame[3], 3*sizeof(char));
                    gill_frame_windsonic[index].direction = atoi(temp_f);
                    memcpy(temp_f, &gill_frame_windsonic[index].frame[7], 6*sizeof(char));
                    gill_frame_windsonic[index].speed = atof(temp_f);
                    gill_frame_windsonic[index].units = gill_frame_windsonic[index].frame[14];
                    memset(temp_f, 0, 10*sizeof(char));
                    memcpy(temp_f, &gill_frame_windsonic[index].frame[16], 2*sizeof(char));
                    gill_frame_windsonic[index].status = atoi(temp_f);

// Debug
    //printf("wind = [%f, %f]\n", gill_frame_windsonic[index].direction, gill_frame_windsonic[index].speed);
 
                    // save data, transform from polar to uv (EU)
                    Anemometer_Data_t *wind_data = sonic_anemometer_get_wind_data();
                    wind_data[index].speed[0] = -std::sin((float)gill_frame_windsonic[index].direction);
                    wind_data[index].speed[1] = -std::cos((float)gill_frame_windsonic[index].direction);
                    wind_data[index].speed[2] = 0.;
                    wind_data[index].temperature = 0.;
                    wind_data[index].t = time(NULL);
                }
                break;
            default:
                break;
        }
    }
}

void* gill_windsonic_read_loop(void* args)
{
    int nbytes;
    char frame[512];
    
    while (!*((bool*)(((Anemometer_Thread_Arguments_t*)args)->arg)))
    {
        nbytes = serial_read(((Anemometer_Thread_Arguments_t*)args)->fd, frame, 512);
        if (nbytes > 0) {
            gillProcessFrame_WindSonic(frame, nbytes, ((Anemometer_Thread_Arguments_t*)args)->index);
        }
    }
    return 0;
}

void* gill_windmaster_read_loop(void* args)
{
    int nbytes;
    char frame[512];
    
    while (!*((bool*)(((Anemometer_Thread_Arguments_t*)args)->arg)))
    {
        nbytes = serial_read(((Anemometer_Thread_Arguments_t*)args)->fd, frame, 512);
        if (nbytes > 0)
            gillProcessFrame_WindMaster(frame, nbytes, ((Anemometer_Thread_Arguments_t*)args)->index);
    }
    return 0;
}
