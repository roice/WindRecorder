/*
 * 3D View of Wind Field
 *          using OpenGL
 *
 * This file defines the classes of the view displaying for 
 * WindRecorder using OpenGL lib.
 * This file is included by UI.cxx, and the classes are
 * integrated into the FLTK UI.
 *
 * Author: Roice (LUO Bing)
 * Date: 2017-04-16 create this file
 */

#ifndef VIEW_H
#define VIEW_H

#include <sys/time.h>

void View_init(int, int);

void View_start_count_time(void);
void View_stop_count_time(void);

#endif
