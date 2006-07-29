#ifndef __CQT_API_H__
#define __CQT_API_H__

#ifdef __cplusplus
extern "C" {
#endif

#define CQT_EXPORT 

CQT_EXPORT void ini();
CQT_EXPORT void make_gui();
CQT_EXPORT void start_event_loop();
CQT_EXPORT void process_events();
CQT_EXPORT void destroy_gui();
CQT_EXPORT void set_silent_error_messages_(int sil);
CQT_EXPORT void *object_(const char *name, const void *parent);
CQT_EXPORT void destroy_(void *object);
CQT_EXPORT void show_delayed(void *obj, int ms, int mode);
CQT_EXPORT const unsigned long object_list();
CQT_EXPORT int get_property_type(const void *obj, const char *name);
CQT_EXPORT int get_property_bool();
CQT_EXPORT int get_property_int();
CQT_EXPORT const unsigned long get_property_string();
CQT_EXPORT const unsigned long get_property_point_or_size();
CQT_EXPORT const unsigned long get_property_rect();
CQT_EXPORT const unsigned long get_property_font();
CQT_EXPORT const unsigned long get_property_color();
CQT_EXPORT const unsigned long get_property_date();
CQT_EXPORT const unsigned long get_property_time();
CQT_EXPORT const unsigned long get_property_date_time();
CQT_EXPORT void set_property_bool(const void *object, const char *name, int value);
CQT_EXPORT void set_property_int(const void *object, const char *name, int value);
CQT_EXPORT void set_property_string(const void *object, const char *name, const char *value);
CQT_EXPORT void set_property_point_or_size(const void *object, const char *name, const char *value);
CQT_EXPORT void set_property_rect(const void *object, const char *name, const char *value);
CQT_EXPORT void set_property_enum(const void *object, const char *name, const char *value);
CQT_EXPORT const unsigned long access_list_(const void *obj, int type, int enums_only);
CQT_EXPORT const unsigned long enum_list_(const void *obj, const char *name);
CQT_EXPORT void set_font_(const void *obj, const char *name, int size, int bold, int italic, int underline, int strikeout);
CQT_EXPORT void set_color_(const void *obj, int role, const char *name);
CQT_EXPORT void message_box_(const char *caption, const char *text, int type);
CQT_EXPORT int message_box_question(const char *caption, const char *text, const char *btn0, const char *btn1, const char *btn2, int def);
CQT_EXPORT float input_dialog_float(const char *caption, const char *lab, float cancel_val, float val, float min, float max, int dec);
CQT_EXPORT int input_dialog_int(const char *caption, const char *lab, int cancel_val, int val, int min, int max);
CQT_EXPORT const unsigned long input_dialog_text(const char *caption, const char *lab, const char *text);
CQT_EXPORT const unsigned long file_dialog_(const char *caption, const char *dir, const char *filter, int type);
CQT_EXPORT const unsigned long file_dialog_existing_directory(const char *caption, const char *start);
CQT_EXPORT const unsigned long font_dialog_(const char *initial);
CQT_EXPORT const unsigned long color_dialog_(const char *initial);
CQT_EXPORT void box_add_layout(const void *lay, const void *add);
CQT_EXPORT void box_add_spacing (const void *lay, int size);
CQT_EXPORT void box_add_stretch(const void *lay, int stretch);
CQT_EXPORT void box_add_widget(const void *lay, const void *w);
CQT_EXPORT void grid_add_layout(const void *lay, const void *add, int row, int col, int row_span, int col_span);
CQT_EXPORT void grid_add_widget(const void *lay, const void *w, int row, int col, int row_span, int col_span);
CQT_EXPORT void connect_qt_(const void *from, const char *signal, const void *to, const char *slot, int con);
CQT_EXPORT const unsigned long argument_types_(const void *from, const char *signal);
CQT_EXPORT void *connect_(const void *from, const char *signal, const unsigned long func);
CQT_EXPORT void set_callback(const void *caller, const unsigned long callback);
CQT_EXPORT void slotcall_void(const void *obj, const char *slot);
CQT_EXPORT void slotcall_int(const void *obj, const char *slot, int a);
CQT_EXPORT void slotcall_string(const void *obj, const char *slot, const char *a);
CQT_EXPORT void slotcall_string_list(const void *obj, const char *slot, const char *a);
CQT_EXPORT void slotcall_object(const void *obj, const char *slot, const void *a);
CQT_EXPORT void slotcall_int_int(const void *obj, const char *slot, int a, int b);
CQT_EXPORT void slotcall_int_string(const void *obj, const char *slot, int a, const char *b);
CQT_EXPORT void slotcall_int_string_list(const void *obj, const char *slot, int a, const char *b);
CQT_EXPORT void slotcall_enum_object(const void *obj, const char *slot, const char *a, const void *b);
CQT_EXPORT void slotcall_object_string(const void *obj, const char *slot, const void *a, const char *b);
CQT_EXPORT void slotcall_int_int_int(const void *obj, const char *slot, int a, int b, int c);
CQT_EXPORT void slotcall_int_int_enum(const void *obj, const char *slot, int a, int b, const char *c);
CQT_EXPORT void slotcall_int_int_string(const void *obj, const char *slot, int a, int b, const char *c);
CQT_EXPORT void *sender();
CQT_EXPORT void set_wait_cursor();
CQT_EXPORT void restore_cursor();
CQT_EXPORT int save_screenshot(void *obj, const char *file);

#ifdef __cplusplus
} // end extern "C"
#endif

#endif //__CQT_API_H__
   
