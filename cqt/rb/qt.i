%module "Qt4::native"
%{
/* Includes the header in the wrapper code */
#include "../cqt_api.h"
%}

%typemap(in) (const char *signal, const unsigned long func) 
	(VALUE keys_arr, int count, VALUE key, VALUE val) {
	Check_Type($input, T_HASH);
	count = NUM2INT(rb_funcall($input, rb_intern("size"), 0, NULL));
	$1 = NULL;
	$2 = 0;
	
	if (count == 1) {
		keys_arr = rb_funcall($input, rb_intern("keys"), 0, NULL);
		key = rb_ary_entry(keys_arr, 0);
		val = rb_hash_aref($input, key);
		Check_Type(key, T_STRING);
		Check_Type(val, T_DATA);
		$1 = STR2CSTR(key);
		$2 = val ? val : Qnil;
		}
}

%typemap(in) (const unsigned long callback) {
  $1 = $input ? $input : Qnil;
}

%typemap(out) const unsigned long {
  $result = $1 ? $1 : Qnil;
}

/* Parse the header file to generate wrappers */
%include "../cqt_api.h"
