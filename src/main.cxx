/*
 * Main function of Ground Station
 *
 * Author: Roice (LUO Bing)
 * Date: 
 *       2016-04-16 create this file (GSRAO)
 */
#include "stdio.h"
#include "FL/Fl.H"
#include "ui/UI.h" // control panel and GL view
#include "WR_config.h" // settings

#include "serial_anemometers.h"

/***************************************************************/
/**************************** MAIN *****************************/
/***************************************************************/

int main(int argc, char **argv) 
{
    printf("start\n");

    /* initialize GS settings */
    //WR_Config_restore();
   
    /* initialize communication among threads */
    //WR_init_thread_comm();

    // Create a window for the display of the experiment data
    UI ui(700, 500, "Ground Station of Robot Active Olfaction System");
    
    // Run
    Fl::run();

    /*
    // save configs before closing
    WR_Config_save();
    std::string port[20]; 
    port[0] = "/dev/ttyUSB0";
    std::string type[20]; 
    type[0] = "Gill WindSonic";
    int err = sonic_anemometer_init(1, port, type);
    printf("err = %d\n", err);

    for (;;);
*/
    return 0;
}

/* End of main.cxx */
