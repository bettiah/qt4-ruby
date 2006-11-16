%module "Qt4::Native"
%{
/* Includes the header in the wrapper code */
#include "../cqt_api.h"
#include "../proxy.h"


QtProxy *signal_sender() {
    void* obj = sender();
    return reinterpret_cast< QtProxy * >(obj);   
}

%}


/* This sets the caller to the right type */
%typemap(in) (unsigned long aproc) 
    {
    arg2 = 0;
    Check_Type($input, T_DATA);
    arg2 = (unsigned long)($input);
    }

%typemap(out) const char *{
    $result = $1 ? rb_str_new2($1) : Qnil;
    /*free((char*)$1);*/
}

/* Parse the header file to generate wrappers */
QtProxy *signal_sender();
void ini();
void make_gui();
void start_event_loop();
void process_events();
void destroy_gui();
void set_silent_error_messages_(int sil);
const char *object_list();
const char *event_list();
int get_property_bool();
int get_property_int();
const char *get_property_string();
const char *get_property_point_or_size();
const char *get_property_point_f();
const char *get_property_rect();
const char *get_property_rect_f();
const char *get_property_line();
const char *get_property_line_f();
const char *get_property_font();
const char *get_property_color();
const char *get_property_date();
const char *get_property_time();
const char *get_property_date_time();
void set_property_bool(int value);
void set_property_int(int value);
void set_property_string(const char *value);
void set_property_url(const char *value);
void set_property_color(const char *value);
void set_property_date(const char *value);
void set_property_time(const char *value);
void set_property_date_time(const char *value);
void set_property_font(const char *value);
void set_property_point_or_size(const char *value);
void set_property_point_f(const char *value);
void set_property_rect(const char *value);
void set_property_rect_f(const char *value);
void set_property_line(const char *value);
void set_property_line_f(const char *value);
void set_property_pen(const char *value);
void set_property_brush(const char *value);
void set_property_polygon(const char *value);
void set_property_enum(const char *value);
void message_box_(const char *caption, const char *text, int type);
int message_box_question(const char *caption, const char *text, const char *btn0, const char *btn1, const char *btn2, int def);
float input_dialog_float(const char *caption, const char *lab, float cancel_val, float val, float min, float max, int dec);
int input_dialog_int(const char *caption, const char *lab, int cancel_val, int val, int min, int max);
const char *input_dialog_text(const char *caption, const char *lab, const char *text);
const char *file_dialog_(const char *caption, const char *dir, const char *filter, int type);
const char *file_dialog_existing_directory(const char *caption, const char *start);
const char *font_dialog_(const char *initial);
const char *color_dialog_(const char *initial);
void call_void();
void call_bool(int a);
void call_int(int a);
void call_float(float a);
void call_enum(const char *a);
void call_string(const char *a);
void call_string_list(const char *a);
void call_url(const char *a);
void call_vector(const char *a);
void call_color(const char *a);
void call_font(const char *a);
void call_pen(const char *a);
void call_brush(const char *a);
void call_int_int(int a, int b);
void call_int_string(int a, const char *b);
void call_int_string_list(int a, const char *b);
void call_float_float(float a, float b);
void call_float_color(float a, const char *b);
void call_string_int(const char *a, int b);
void call_vector_string(const char *a, const char *b);
void call_vector_brush(const char *a, const char *b);
void call_int_int_int(int a, int b, int c);
void call_int_int_enum(int a, int b, const char *c);
void call_int_int_string(int a, int b, const char *c);
void call_int_int_font(int a, int b, const char *c);
void call_vector_string_vector(const char *a, const char *b, const char *c);
void call_vector_int_int(const char *a, int b, int c);
void set_wait_cursor();
void restore_cursor();
const char *dir(const char *path);

%include "../proxy.h"
