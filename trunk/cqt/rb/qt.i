%module "Qt4::Native"
%{
/* Includes the header in the wrapper code */
#include "../cqt_api.h"

%}

%inline %{

class QtProxy {
friend QtProxy* make_qt_gui();
friend void signal_sender(VALUE r_dummy);

public:
~QtProxy() {
    ::destroy_(_real_obj);
}

QtProxy(const char *name, const QtProxy *par) {
    _real_obj = ::object_(name, par ? par->_real_obj : NULL);
}

void show_delayed(int ms, int mode) {
    ::show_delayed(_real_obj, ms, mode);
}

const char *access_list_(int type, int enums_only) {
    return ::access_list_(_real_obj, type, enums_only);
}
    
const char *enum_list_(const char *name) {
    return ::enum_list_(_real_obj, name);
}

int get_property_type(const char *name) {
    return ::get_property_type(_real_obj, name);
}

void set_color_(int role, const char *name) {
    ::set_color_(_real_obj, role, name);
}

void box_add_layout(const QtProxy *add) {
    ::box_add_layout(_real_obj, add ? add->_real_obj : NULL);
}

void box_add_spacing (int size) {
    ::box_add_spacing(_real_obj, size);
}

void box_add_stretch(int stretch) {
    ::box_add_stretch(_real_obj, stretch);
}

void box_add_widget(const QtProxy *w) {
    ::box_add_widget(_real_obj, w ? w->_real_obj : NULL);
}

void grid_add_layout(const QtProxy *add, int row, int col, int row_span, int col_span) {
    ::grid_add_layout(_real_obj, add ? add->_real_obj : NULL, row, col, row_span, col_span);
}

void grid_add_widget(const QtProxy *w, int row, int col, int row_span, int col_span) {
    ::grid_add_widget(_real_obj, w ? w->_real_obj : NULL, row, col, row_span, col_span);
}

void connect_qt_(const char *signal, const QtProxy *to, const char *slot, int con) {
    ::connect_qt_(_real_obj, signal, to ? to->_real_obj : NULL, slot, con);
}

QtProxy *connect_(const char *signal) {
    void* obj = ::connect_(_real_obj, signal);
    QtProxy* proxy = new QtProxy;
    proxy->_real_obj = obj;
    return proxy;
}

const char *argument_types_(const char *signal) {
    return ::argument_types_(_real_obj, signal);
}

void set_callback(VALUE aproc) {
    unsigned long arg = 0;
    Check_Type(aproc, T_DATA);
    arg = (unsigned long)(aproc);

    ::set_callback(_real_obj, aproc);
}

int call_type(const char *slot) {
    return ::call_type(_real_obj, slot);
}

void call_object(const QtProxy *a) {
    ::call_object(a ? a->_real_obj : NULL);
}

void call_enum_object(const char *a, const QtProxy *b) {
    ::call_enum_object(a, b ? b->_real_obj : NULL);
}

void call_object_int(const QtProxy *a, int b) {
    ::call_object_int(a ? a->_real_obj : NULL, b);
}

void call_object_enum(const QtProxy *a, const char *b) {
    ::call_object_enum(a ? a->_real_obj : NULL, b);
}

void call_object_string(const QtProxy *a, const char *b) {
    ::call_object_string(a ? a->_real_obj : NULL, b);
}

void call_int_object_string(int a, const QtProxy *b, const char *c) {
    ::call_int_object_string(a, b ? b->_real_obj : NULL, c);
}

void call_object_string_string(const QtProxy *a, const char *b, const char *c) {
    ::call_object_string_string(a ? a->_real_obj : NULL, b, c);
}

void call_int_object_string_string(int a, const QtProxy *b, const char *c, const char *d) {
    ::call_int_object_string_string(a, b ? b->_real_obj : NULL, c, d);
}

void event_filter_(const char *type, const QtProxy *callback, int eat) {
    ::event_filter_(_real_obj, type, callback ? callback->_real_obj : NULL, eat);
}

int save_screenshot(const char *file) {
    return ::save_screenshot(_real_obj, file);
}

private:
QtProxy() :_real_obj(NULL)
{}

private:
void* _real_obj;
};

QtProxy* make_qt_gui()
{
	QtProxy* app = new QtProxy;
	app->_real_obj = make_gui();
	return app;
}

void signal_sender(VALUE r_dummy) {
    void *argp = 0 ;
    int res = SWIG_ConvertPtr(r_dummy, &argp, SWIGTYPE_p_QtProxy, 0 |  0 );
    if (!SWIG_IsOK(res)) {
//        SWIG_exception_fail(SWIG_ArgError(res), "in method '" "QtProxy" "', argument " "1"" of type '" "QtProxy const *""'"); 
      }

    QtProxy* dummy = reinterpret_cast< QtProxy * >(argp);
    if (rb_block_given_p()) {
        /* use the dummy object and substitute the real object for the caller */
        void* _old_real = dummy->_real_obj;
        dummy->_real_obj = sender();
        rb_yield(r_dummy);
        dummy->_real_obj = _old_real;
    }
}
%}

%typemap(out) const char *{
    $result = $1 ? rb_str_new2($1) : Qnil;
    /*free((char*)$1);*/
}

/* Parse the header file to generate wrappers */
void ini();
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

