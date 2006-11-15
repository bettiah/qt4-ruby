require 'dl/import'

module Qt extend ::DL::Importable

    library_name = case RUBY_PLATFORM.downcase
      when /win32/
        "gui.dll"
      else
        abort <<-EOF
== * UNSUPPORTED PLATFORM ======================================================
The platform '#{RUBY_PLATFORM}' is unsupported. 
#{__FILE__}
=========================================================================== * ==
        EOF
    end

    dlload library_name

	typealias "bool",			"void*"
	typealias "string-list",	"void*"
	typealias "point",			"void*"
	typealias "rect",			"void*"
	typealias "font",			"void*"
	typealias "color",			"void*"
	typealias "enum",			"void*"
	typealias "url",			"void*"
	
	#QApplication
	extern "void make_gui()"
	extern "void start_event_loop()"
	extern "void process_events()"
	extern "void destroy_gui()"
	extern "void set_wait_cursor()"
	extern "void restore_cursor()"
	extern "int save_screenshot(void *, const char *)"
	extern "void *sender()"

	#QObject
	extern "void *new_object(const char *, const void *)"
	extern "void destroy(void *)"
	extern "void show_delayed(void *, int, int)"
	extern "const char *object_list()"

	#Properties
	extern "int property_bool(const void *, const char *)"
	extern "int property_int(const void *, const char *)"
	extern "const char *property_string(const void *, const char *)"
	extern "void *property_object(const void *, const char *)"
	extern "const char *property_point_or_size(const void *, const char *)"
	extern "const char *property_rect(const void *, const char *)"
	extern "const char *property_font(const void *)"
	extern "const char *property_color(const void *)"
	extern "const char *property_enum(const void *, const char *)"

	extern "void set_property_rect(const void *, const char *, const char *)"
	extern "void set_property_enum(const void *, const char *, const char *)"
	extern "void set_property_bool(const void *, const char *, int)"
	extern "void set_property_int(const void *, const char *, int)"
	extern "void set_property_string(const void *, const char *, const char *)"
	extern "void set_property_point_or_size(const void *, const char *, const char *)"

	extern "const char *get_access_list(const void *, int, int)"
	extern "const char *get_enum_list(const void *, const char *)"
	extern "void widget_set_font(const void *, const char *, int, int, int, int, int)"
	extern "void widget_set_color(const void *, int, const char *)"

	#QMessageBox
	extern "void show_message_box(const char *, const char *, int)"
	extern "int message_box_question(const char *, const char *, const char *, const char *, const char *, int)"
	extern "float input_dialog_float(const char *, const char *, float, float, float, float, int)"
	extern "int input_dialog_int(const char *, const char *, int, int, int, int)"
	extern "const char *input_dialog_text(const char *, const char *, const char *)"

	#QFileDialog
	extern "const char *file_dialog_name(const char *, const char *, const char *, int)"
	extern "const char *file_dialog_existing_directory(const char *, const char *)"

	#QFontDialog
	extern "const char *font_string_dialog(const char *)"

	#QColorDialog
	extern "const char *color_string_dialog(const char *)"
#Layout
	extern "void box_layout_add_layout(const void *, const void *)"
	extern "void box_layout_add_spacing (const void *, int)"
	extern "void box_layout_add_stretch(const void *, int)"
	extern "void box_layout_add_widget(const void *, const void *)"
	extern "void grid_layout_add_layout(const void *, const void *, int, int, int, int)"
	extern "void grid_layout_add_widget(const void *, const void *, int, int, int, int)"

	#Connections
	extern "void connect_signal_slot(const void *, const char *, const void *, const char *)"
	extern "void connect_signal_lisp_slot(const void *, const char *, const void *, const char *)"
	extern "void disconnect_signal_slot(const void *, const char *, const void *, const char *)"
	extern "void disconnect_signal_lisp_slot(const void *, const char *, const void *, const char *)"
	extern "void call_slot_void(const void *, const char *)"
	extern "void call_slot_bool(const void *, const char *, int)"
	extern "void call_slot_int(const void *, const char *, int)"
	extern "void call_slot_string(const void *, const char *, const char *)"
	extern "void call_slot_string_list(const void *, const char *, const char *)"
	extern "void call_slot_int_bool(const void *, const char *, int, int)"
	extern "void call_slot_int_int(const void *, const char *, int, int)"
	extern "void call_slot_int_string(const void *, const char *, int, const char *)"
	extern "void call_slot_int_string_list(const void *, const char *, int, const char *)"
	extern "void call_slot_int_int_int(const void *, const char *, int, int, int)"
	extern "void call_slot_int_int_string(const void *, const char *, int, int, const char *)"
	extern "void call_slot_int_int_enum(const void *, const char *, int, int, const char *)"
	extern "void call_slot_object(const void *, const char *, const void *)"
	extern "void call_slot_object_string(const void *, const char *, const void *, const char *)"
	extern "void call_slot_enum_object(const void *, const char *, const char *, const void *)"

	def to_string(obj, slot)
		format("%s%s", slot.nil? ? '_' : '', obj.to_s.gsub(/.*:/, ''))
	end

#Property
    def setp(obj, args)
        args.each { |name,value|
            set_prop(obj, name, value)
        }
    end
    
	def set_prop(obj, name, val)
        case "#{val.class}"
            when "String" then set_property_string(obj, name.to_s, val)
            when "Fixnum" then set_property_int(obj, name.to_s, val)
            when "Symbol" then set_property_enum(obj, name.to_s, val.to_s)
            when "Array"
                case val.length 
                    when 2 then set_property_point_or_size(obj, name.to_s, val)
                    when 4 then set_property_rect(obj, name.to_s, val)
                    end
            when "TrueClass" then set_property_bool(obj, name.to_s, 1)
			when "FalseClass" then set_property_bool(obj, name.to_s, 0)
            else p "#{val.class}: #{name.to_s} did not match"
            end
	end

	def getp(obj, name)
	end

	def set_font(obj, size, family="", bold=0, italic=0, underline=0, strikeout=0)
		widget_set_font(obj, family, size, bold, italic, underline, strikeout)
	end

	MAX_SIZE=32767
	def allow_expanding(widget)
		setp widget, :'maximum-width'=>MAX_SIZE, :'maximum-height'=>MAX_SIZE
	end

#Layout   
	def box(layout, type, add)
		case type
			when :layout then box_layout_add_layout(layout, add)
			when :spacing then box_layout_add_spacing(layout, add)
			when :stretch then box_layout_add_stretch(layout, (add ? add : 0))
			when :widget  then box_layout_add_widget(layout, add)
			end
	end

	def grid_lay_add(layout1, type, add1, row, col, row_span, col_span)
		case type
			when :layout then grid_layout_add_layout(layout1, add1, row, col, row_span, col_span)
			when :widget then 
				grid_layout_add_widget(layout1, add1, row, col, row_span, col_span)
			end
	end

	def grid(layout, type, add, row, col, row_span=1, col_span=1)
		grid_lay_add(layout ,type ,add ,row ,col, row_span, col_span)
	end

	def message_box(type, text, title=nil, but0="Yes", but1="No", but2="", default=0)
        case type
        when :info then show_message_box(title ? title : "Information", text, 0)
        when :warning then show_message_box(title ? title : "Warning", text, 1)
        when :error then show_message_box(title ? title : "Error", text, 2)
        when :question then message_box_question(title ? title : "Question", text, but0, but1, but2, default)
        end
    end

#Signal Slots
    def connect(from, signal, &block)
		slot_handler_callback = ::DL.callback( "I" ) do 
			block.call() || 0
		end

		#func is a function pointer, slot is the argument signature
		connect_signal_lisp_slot(from, signal.to_s, block&&slot_handler_callback, "()")
    end

    def disconnect()
    end

#Object
	def object(name, parent=nil)
		p "creating #{name.to_s} with parent #{parent.class}"
		new_object(name.to_s, parent)
	end

	def show(mainWidget, milliSecond=0)
		show_delayed(mainWidget, milliSecond, 0)
	end
	
	def with_qt()
		make_gui()
		if block_given?
			yield
			start_event_loop
			end
		destroy_gui
	end
		
end

