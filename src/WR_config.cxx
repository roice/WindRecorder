/*
 * Configuration file of WindRecorder
 *
 * This file contains configuration data and methods of WindRecorder.
 * The declarations of the classes, functions and data are written in 
 * file WR_config.h, which is included by main.cxx and
 * user interface & drawing files.
 *
 * Author: Roice (LUO Bing)
 * Date: 2017-04-16 create this file
 */
#include "WR_config.h"
// for .ini file reading
#include <boost/property_tree/ptree.hpp>  
#include <boost/property_tree/ini_parser.hpp>

/* Configuration data */
static WR_Config_t settings;

/* Restore settings from configuration file */
void WR_Config_restore(void)
{
    char name[256];

    /* check if there exists a config file */
    if(access("settings.cfg", 0))
    {// config file not exist
        WR_Config_init(); // load default config
        // create new config file
        FILE *fp;
        fp = fopen("settings.cfg", "w+");
        fclose(fp);
    }
    else // config file exist
    {
        /* read configuration files */
        boost::property_tree::ptree pt;
        boost::property_tree::ini_parser::read_ini("settings.cfg", pt);
        /* restore configs */
        // arena
        settings.arena.w = pt.get<float>("Arena.width");
        settings.arena.l = pt.get<float>("Arena.length");
        settings.arena.h = pt.get<float>("Arena.height");
        // Anemometers
        settings.anemo.num_of_anemometers = pt.get<int>("Anemometers.num_of_anemometers");
    }
}

/* Save settings to configuration file */
void WR_Config_save(void)
{
    /* prepare to write configuration files */
    boost::property_tree::ptree pt;
    // arena size
    pt.put("Arena.width", settings.arena.w);
    pt.put("Arena.length", settings.arena.l);
    pt.put("Arena.height", settings.arena.h);
    // anemometers
    char name[256];
    for (int idx = 0; idx < SERIAL_MAX_ANEMOMETERS; idx++) {
        snprintf(name, sizeof(name), "Anemometers.serial_port_path_anemometer_%d", idx+1);
        pt.put(name, settings.anemo.anemometer_serial_port_path[idx]);
        snprintf(name, sizeof(name), "Anemometers.type_anemometer_%d", idx+1);
        pt.put(name, settings.anemo.anemometer_type[idx]);
    }
    pt.put("Anemometers.num_of_anemometers", settings.anemo.num_of_anemometers);
    /* write */
    boost::property_tree::ini_parser::write_ini("settings.cfg", pt);
}

/* init settings (obsolete) */
void WR_Config_init(void)
{
    /* init arena settings */
    // arena
    settings.arena.w = 10; // x
    settings.arena.l = 10; // y
    settings.arena.h = 10; // z
    // anemometers
    char name[256];
    for (int i = 0; i < SERIAL_MAX_ANEMOMETERS; i++) {
        snprintf(name, sizeof(name), "/dev/ttyUSB_WR_ANEMOMETER_%d", i+1);
        settings.anemo.anemometer_serial_port_path[i] = name;
        snprintf(name, sizeof(name), "Gill WindSonic");
        settings.anemo.anemometer_type[i] = name;
    }
    settings.anemo.num_of_anemometers = 3;
}

/* get pointer of config data */
WR_Config_t* WR_Config_get_configs(void)
{
    return &settings;
}

/* End of WR_Config.cxx */

