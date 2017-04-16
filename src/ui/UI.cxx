/*
 * User Interface of WindRecorder
 *         using FLTK
 *
 * Author: Roice (LUO Bing)
 * Date: 2017-04-16 create this file
 */

/* FLTK */
#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Pixmap.H>
#include <FL/Fl_Tabs.H>
#include <FL/fl_ask.H>
#include <FL/Fl_Shared_Image.H>
#include <FL/Fl_PNG_Image.H>
#include <FL/Fl_Value_Input.H>
#include <FL/Fl_Value_Slider.H>
#include <FL/Fl_Choice.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Scroll.H>
/* OpenGL */
#include <FL/Fl_Gl_Window.H>
#include <FL/gl.h>
#include <FL/glut.H>
/* WindRecorder */
#include "WR_config.h"
#include "ui/UI.h"
#include "ui/View.h"
#include "ui/icons/icons.h" // pixmap icons used in Tool bar

/*------- Configuration Dialog -------*/
struct ConfigDlg_Widgets { // for parameter saving
    // serial port receiving anemometer data
    Fl_Choice* num_of_anemometers;
    Fl_Input* anemo_serial_port[SERIAL_MAX_ANEMOMETERS];
    Fl_Choice* anemo_type[SERIAL_MAX_ANEMOMETERS];
};
class ConfigDlg : public Fl_Window
{
public:
    ConfigDlg(int xpos, int ypos, int width, int height, const char* title); 
    // widgets
    struct ConfigDlg_Widgets ws;
private:
    // callback funcs
    static void cb_close(Fl_Widget*, void*);
    static void cb_switch_tabs(Fl_Widget*, void*);
    // function to save current value of widgets to runtime configs
    static void save_value_to_configs(ConfigDlg_Widgets*);
};
void ConfigDlg::cb_close(Fl_Widget* w, void* data) {
    if (Fl::event() == FL_CLOSE) {
        // save widget values to GSRAO runtime configs when closing the dialog window
        struct ConfigDlg_Widgets *ws = (struct ConfigDlg_Widgets*)data;
        save_value_to_configs(ws);
        // close dialog
        ((Fl_Window*)w)->hide();
    }
}
void ConfigDlg::cb_switch_tabs(Fl_Widget *w, void *data)
{
    Fl_Tabs *tabs = (Fl_Tabs*)w; 
    // When tab changed, make sure it has same color as its group
    tabs->selection_color( (tabs->value())->color() );
}
void ConfigDlg::save_value_to_configs(ConfigDlg_Widgets* ws) {
    WR_Config_t* configs = WR_Config_get_configs(); // get runtime configs
}
ConfigDlg::ConfigDlg(int xpos, int ypos, int width, int height, 
        const char* title=0):Fl_Window(xpos,ypos,width,height,title)
{

    // add event handle to dialog window
    callback(cb_close, (void*)&ws);   
    // begin adding children
    begin();
    // Tabs
    int t_x = 5, t_y = 5, t_w = w()-10, t_h = h()-10;
    Fl_Tabs *tabs = new Fl_Tabs(t_x, t_y, t_w, t_h);
    {
        tabs->callback(cb_switch_tabs); // callback func when switch tabs

        // Tab Scenario
        Fl_Group *scenario = new Fl_Group(t_x,t_y+25,t_w,t_h-25,"Scenario");
        {
            // color of this tab
            scenario->color(0xebf4fa00); // water
            scenario->selection_color(0xebf4fa00); // water
        }
        scenario->end();

        // Tab Flow
        Fl_Group *flow = new Fl_Group(t_x,t_y+25,t_w,t_h-25,"Flow");
        {
            // color of this tab
            flow->color(0xe0ffff00); // light blue
            flow->selection_color(0xe0ffff00); // light blue

            // Serial port receiving anemometer data
            Fl_Box *anemometer_box = new Fl_Box(t_x+10, t_y+25+10, 370, 350,"Serial Ports of Anemometers");
            anemometer_box->box(FL_PLASTIC_UP_FRAME);
            anemometer_box->labelsize(16);
            anemometer_box->labelfont(FL_COURIER_BOLD_ITALIC);
            anemometer_box->align(Fl_Align(FL_ALIGN_TOP|FL_ALIGN_INSIDE));
            // number of anemometers
            ws.num_of_anemometers = new Fl_Choice(t_x+10+200, t_y+25+10+20, 100, 25,"Number of anemometers ");
            ws.num_of_anemometers->add("0");
            ws.num_of_anemometers->add("1");
            ws.num_of_anemometers->add("2");
            ws.num_of_anemometers->add("3");
            ws.num_of_anemometers->add("4");
            ws.num_of_anemometers->add("5");
            ws.num_of_anemometers->add("6");
            ws.num_of_anemometers->add("7");
            ws.num_of_anemometers->add("8");
            ws.num_of_anemometers->add("9");
            ws.num_of_anemometers->add("10");
            //ws.num_of_anemometers->callback(cb_change_num_of_anemometers, (void*)&ws);
            for (int i = 0; i < SERIAL_MAX_ANEMOMETERS; i++) {
                ws.anemo_serial_port[i] = new Fl_Input(t_x+10+50, t_y+25+10+50+30*i, 160, 25, "Port ");
                ws.anemo_type[i] = new Fl_Choice(t_x+10+100+160, t_y+25+10+50+30*i, 100, 25, "Type");
                ws.anemo_type[i]->add("RM Young 3D");
                ws.anemo_type[i]->add("Gill 3D");
            }
        }
        flow->end();
    }
    // Make sure default tab has same color as its group
    tabs->selection_color( (tabs->value())->color() );
    tabs->end();
    
    end();
    // set widget value according to runtime configs
    //get_value_from_configs(&ws);
    show();
}

/* ================================
 * ========= ToolBar ==============
 * ================================*/
struct ToolBar_Widgets
{
    Fl_Button*  start;  // start button
    Fl_Button*  stop;   // stop button
    Fl_Button*  config; // config button
    Fl_Box*     msg_zone; // message zone
};
struct ToolBar_Handles // handles of dialogs/panels opened by corresponding buttons
{
    ConfigDlg* config_dlg; // handle of config dialog opened by config button
};
class ToolBar : public Fl_Group
{
public:
    ToolBar(int Xpos, int Ypos, int Width, int Height, void *win);
    struct ToolBar_Widgets ws;
    static struct ToolBar_Handles hs;
    void restore_from_configs(ToolBar_Widgets*, void*);
private:
    static void cb_button_start(Fl_Widget*, void*);
    static void cb_button_stop(Fl_Widget*, void*);
    static void cb_button_config(Fl_Widget*, void*);
};
struct ToolBar_Handles ToolBar::hs = {NULL};
void ToolBar::restore_from_configs(ToolBar_Widgets* ws, void *data)
{
    WR_Config_t* configs = WR_Config_get_configs(); // get runtime configs
}
void ToolBar::cb_button_start(Fl_Widget *w, void *data)
{
    WR_Config_t* configs = WR_Config_get_configs(); // get runtime configs

    ToolBar_Widgets* widgets = (ToolBar_Widgets*)data;

    if (((Fl_Button*)w)->value()) { // if start button is pressed down
        // lock config button
        widgets->config->deactivate();
        widgets->msg_zone->label(""); // clear message zone
        // clear anemometer record
        //
        // start counting experiment time
        View_start_count_time();
    }
    else {
        // user is trying to release start button when pause is not pressed
        ((Fl_Button*)w)->value(1);
    }
}
void ToolBar::cb_button_stop(Fl_Widget *w, void *data)
{
    // release start buttons
    struct ToolBar_Widgets *widgets = (struct ToolBar_Widgets*)data;
    widgets->start->clear();

    WR_Config_t* configs = WR_Config_get_configs(); // get runtime configs

    View_stop_count_time(); // stop counting experiment time

    // clear message zone
    widgets->msg_zone->label("");
    // unlock config button
    widgets->config->activate();

    // save robot record
    //WR_Save_Data();
}
void ToolBar::cb_button_config(Fl_Widget *w, void *data)
{
    if (hs.config_dlg != NULL)
    {
        if (hs.config_dlg->shown()) // if shown, do not open again
        {}
        else
        {
            hs.config_dlg->show(); 
        }
    }
    else // first press this button
    {// create config dialog
        Fl_Window* window=(Fl_Window*)data;
        hs.config_dlg = new ConfigDlg(window->x()+20, window->y()+20, 
            400, 400, "Settings");
    }
}
ToolBar::ToolBar(int Xpos, int Ypos, int Width, int Height, void *win) :
Fl_Group(Xpos, Ypos, Width, Height)
{
    begin();
    Fl_Box *bar = new Fl_Box(FL_UP_BOX, 0, 0, Width, Height, "");
    Ypos += 2; Height -= 4; Xpos += 3; Width = Height;
    // widgets of this toolbar
    //struct ToolBar_Widgets ws;
    // instances of buttons belong to tool bar
    ws.start = new Fl_Button(Xpos, Ypos, Width, Height); Xpos += Width + 5;
    ws.stop = new Fl_Button(Xpos, Ypos, Width, Height); Xpos += Width + 5;
    ws.config = new Fl_Button(Xpos, Ypos, Width, Height); Xpos += Width + 5;
    ws.msg_zone = new Fl_Box(FL_DOWN_BOX, Xpos, Ypos, bar->w()-Xpos, Height, "");
    ws.msg_zone->align(Fl_Align(FL_ALIGN_CENTER|FL_ALIGN_INSIDE));
    resizable(ws.msg_zone); // protect buttons from resizing
    // icons
    Fl_Pixmap *icon_start = new Fl_Pixmap(pixmap_icon_play);
    Fl_Pixmap *icon_stop = new Fl_Pixmap(pixmap_icon_stop);
    Fl_Pixmap *icon_config = new Fl_Pixmap(pixmap_icon_config);
    // link icons to buttons
    ws.start->image(icon_start);
    ws.stop->image(icon_stop);
    ws.config->image(icon_config);
    // tips for buttons
    ws.start->tooltip("Start Searching");
    ws.stop->tooltip("Stop Searching");
    ws.config->tooltip("Settings");
    // types of buttons
    ws.start->type(FL_TOGGLE_BUTTON);
    // link call backs to buttons
    ws.start->callback(cb_button_start, (void*)&ws);
    //  start buttons will be released when stop button is pressed
    ws.stop->callback(cb_button_stop, (void*)&ws);
    //  config dialog will pop up when config button pressed
    ws.config->callback(cb_button_config, (void*)win);
    end();
}

/* ====================================
 * ============== UI ==================
 * ==================================== */
void UI::cb_close(Fl_Widget* w, void* data) {
    //GSRAO_Config_t* configs = GSRAO_Config_get_configs(); // get runtime configs
    // close GSRAO
    if (Fl::event() == FL_CLOSE) {
        // close main window
        ((Fl_Window*)w)->hide();
    }
}
UI::UI(int width, int height, const char* title=0)
{
    /* Main Window, control panel */
    Fl_Double_Window *ui = new Fl_Double_Window(1600, 0, width, height, title);
    ui->resizable(ui); 
 
    ui->show(); // glut will die unless parent window visible
    /* begin adding children */
    ui->begin();
    // Add tool bar, it's width is equal to panel's
    ToolBar* tool = new ToolBar(0, 0, width, 34, (void*)ui);
    ws.toolbar = tool;
    tool->clear_visible_focus(); //just use mouse, no TABs
    // protect buttons from resizing
    Fl_Box *r = new Fl_Box(FL_NO_BOX, width, tool->h(), 0, height-tool->h(), "right_border");
    r->hide();
    ui->resizable(r);
    /* Add RAO view */    
    glutInitWindowSize(width-10, height-tool->h()-10);// be consistent with View_init
    glutInitWindowPosition(5, tool->h()+5); // place it inside parent window
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_MULTISAMPLE);
    glutCreateWindow("Experiment view");
    /* end adding children */
    ui->end();
    ui->resizable(glut_window);
    ui->callback(cb_close, &ws);// callback
 
    // init view
    View_init(width-10, height-tool->h()-10);// pass gl window size

    // open panels according to last use info
    tool->restore_from_configs(&(tool->ws), (void*)ui);
};
/* End of UI.cxx */
