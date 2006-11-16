#include "cqt_api.h"

class QtProxy {
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

void set_callback(unsigned long aproc) {
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


