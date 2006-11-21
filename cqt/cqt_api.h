
#ifndef __CQT_API_H__
#define __CQT_API_H__

#ifdef _MSC_VER
#define CQT_EXPORT //so that we dont get these exports
#endif

#ifdef __cplusplus
extern "C" {
#endif
void ini();
void make_gui();
void start_event_loop();
void process_events();
void destroy_gui();
void set_silent_error_messages_(int sil);
void *object_(const char *name, const void *par);
void destroy_(void *obj);
void show_delayed(void *obj, int ms, int mode);
const char *object_list();
const char *access_list_(const void *obj, int type, int enums_only);
const char *enum_list_(const void *obj, const char *name);
const char *event_list();
int get_property_type(const void *obj, const char *name);
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
void set_color_(const void *obj, int role, const char *name);
void message_box_(const char *caption, const char *text, int type);
int message_box_question(const char *caption, const char *text, const char *btn0, const char *btn1, const char *btn2, int def);
float input_dialog_float(const char *caption, const char *lab, float cancel_val, float val, float min, float max, int dec);
int input_dialog_int(const char *caption, const char *lab, int cancel_val, int val, int min, int max);
const char *input_dialog_text(const char *caption, const char *lab, const char *text);
const char *file_dialog_(const char *caption, const char *dir, const char *filter, int type);
const char *file_dialog_existing_directory(const char *caption, const char *start);
const char *font_dialog_(const char *initial);
const char *color_dialog_(const char *initial);
void box_add_layout(const void *lay, const void *add);
void box_add_spacing (const void *lay, int size);
void box_add_stretch(const void *lay, int stretch);
void box_add_widget(const void *lay, const void *w);
void grid_add_layout(const void *lay, const void *add, int row, int col, int row_span, int col_span);
void grid_add_widget(const void *lay, const void *w, int row, int col, int row_span, int col_span);
void connect_qt_(const void *from, const char *signal, const void *to, const char *slot, int con);
void *connect_(const void *from, const char *signal);
const char *argument_types_(const void *from, const char *signal);
void set_callback(const void *caller, unsigned long aproc);
void *sender();
int call_type(const void *obj, const char *slot);
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
void call_object(const void *a);
void call_int_int(int a, int b);
void call_int_string(int a, const char *b);
void call_int_string_list(int a, const char *b);
void call_float_float(float a, float b);
void call_float_color(float a, const char *b);
void call_string_int(const char *a, int b);
void call_vector_string(const char *a, const char *b);
void call_vector_brush(const char *a, const char *b);
void call_enum_object(const char *a, const void *b);
void call_object_int(const void *a, int b);
void call_object_enum(const void *a, const char *b);
void call_object_string(const void *a, const char *b);
void call_int_int_int(int a, int b, int c);
void call_int_int_enum(int a, int b, const char *c);
void call_int_int_string(int a, int b, const char *c);
void call_int_int_font(int a, int b, const char *c);
void call_vector_string_vector(const char *a, const char *b, const char *c);
void call_int_object_string(int a, const void *b, const char *c);
void call_vector_int_int(const char *a, int b, int c);
void call_object_string_string(const void *a, const char *b, const char *c);
void call_int_object_string_string(int a, const void *b, const char *c, const char *d);
void event_filter_(const void *object, const char *type, const void *callback, int eat);
void set_wait_cursor();
void restore_cursor();
int save_screenshot(void *obj, const char *file);
const char *dir(const char *path);

#ifdef __cplusplus
} // end extern "C"
#endif

#endif //__CQT_API_H__


