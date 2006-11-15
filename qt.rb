module Qt4

require "Native"
include Native


#Initialization
def Init
	Native.ini
end


class NoMainWidgetError < RuntimeError
end

class BadSlotNameError < RuntimeError
end

class BadWidgetNameError < RuntimeError
end

class CouldNotConnectError < RuntimeError
end

def add_methods_to(obj)
    class << obj

    def destroy()
        Native.destroy_(self)
    end

    def show(milliSecond=0, mode=0)
        Native.show_delayed(self, milliSecond, 
							case mode
							when :minimized then 1
							when :maximized then 2
							when :'full-screen' then 3
							else 0
							end)
    end
     
    def to_string(slot=nil)
        format("%s%s", slot.nil? ? '_' : '', self.to_s.gsub(/.*:/, ''))
    end
    
    def repaint_now()
        slotcall self, "(repaint)"
        Native.process_events
    end

#Property
    def setp(args)
        args.each { |name, val|
            case get_property_type self, name.to_s
            when 1 then set_property_bool case val 
                                            when TrueClass then 1
                                            when FalseClass then 0
                                            end
            when 2 then 
                case val
                    when Fixnum then set_property_int val
                    when Symbol then set_property_enum val.to_s
                    end
            when 10, 76 then set_property_string val
            when 14 then set_property_date val
            when 15 then set_property_time val
            when 16 then set_property_date_time val
            when 17 then set_property_url val
            when 19 then set_property_rect val
            when 20 then set_property_rect_f val
            when 21, 25 then set_property_point_or_size val.to_s
            when 26 then set_property_point_f val
            when 32 then set_property_line val
            when 24 then set_property_line_f val
            when 64 then set_property_font val.to_s
            when 66 then set_property_brush val
            when 67 then set_property_color val
            when 71 then set_property_polygon val
            when 74, 75 then set_property_enum val.to_s
            when 77 then set_property_pen val
            end
        }
		self
    end
    
    def getp(name)
        case Native.get_property_type self, name.to_s
            when 1 then (Native.get_property_bool == 1) ? true : false
            when 2 then Native.get_property_int
            when 10, 17 then Native.get_property_string
            when 14 then Native.get_property_date
            when 15 then Native.get_property_time
            when 16 then Native.get_property_date_time
            when 19 then Native.get_property_rect
            when 21, 25 then Native.get_property_point_or_size
            when 23 then get_property_line
            when 24 then get_property_line_f
            when 26 then get_property_point_f
            when 64 then Native.get_property_font
            when 67 then Native.get_property_color
            else "#{:native.class}: #{name.to_s} did not match"
            end
    end

    def access_list(type)
        Native.access_list_(self, (case type
             when :property then 0
             when :signal then 1
             when :slot then 2
             end), 0)
    end

    def enum_property_list()
        Native.access_list_(self, 0, 1)
    end
    
    def enum_list(name)    
         Native.enum_list_(self, (to_string name))
    end

#Connections
    def connect(signal, *callbacks, &block)
        cb = nil
        args = nil
        callbacks << block if block_given?
#            args = argument_types_(self, signal.to_s)
#            raise BadSlotNameError, "Could not find signal #{signal} in Object" unless args
 		callbacks.each {|callback|
			cb = connect_(self, signal.to_s)
            raise CouldNotConnectError, "Could not connect to signal" unless cb
			case callback
			when Symbol
				set_callback cb, method(callback)
			when Proc, Method
				set_callback cb, callback
			end
		}
    end

	def callback__(signal, *args)
		callbacks_for(signal).each do |callback|
          result = case callback
            when Symbol
              self.send(callback, *args)
            when String
              eval(callback, binding)
            when Proc, Method
              callback.call(self, *args)
            else
              if callback.respond_to?(signal)
                callback.send(signal, self)
              else
                raise CallbackError, "Callbacks must be a symbol denoting the method to call, a string to be evaluated, a block to be invoked, or an object responding to the callback method."
              end
          end
          return false if result == false
        end

        result = send(signal) if respond_to_without_attributes?(signal)
	end

	def callbacks_for(signal)
        self.class.read_inheritable_attribute(signal.to_sym) or []
	end

    def disconnect()
    end

#Slot calls
    def method_missing(method_symbol, *parameters)#:nodoc:
        slotcall method_symbol.id2name, *parameters
    end
      
    def slotcall(name, *args)
        case call_type self, name.to_s
        when 1 then call_void
        when 2 then call_bool args[0]
        when 3 then call_int args[0]
        when 4 then call_float args[0].to_f
        when 5 then call_enum args[0]
        when 6 then call_string args[0]
        when 7 then call_string_list args[0]
        when 8 then call_url args[0]
        when 9, 10, 11, 12, 13 then call_vector arg[0]
        when 14 then call_color args[0]
        when 15 then call_font args[0]
        when 16 then call_pen args[0]
        when 17 then call_brush args[0]
        when 18 then call_object args[0]
        when 19 then call_int_int args[0], args[1]
        when 20 then call_int_string args[0], args[1]
        when 21 then call_int_string_list args[0], args[1]
        when 22 then call_float_float args[0].to_f, args[1].to_f
        when 23 then call_float_color args[0].to_f, args[1]
        when 24 then call_string_int args[0], args[1]
        when 25 then call_vector_string args[0], args[1]
        when 26 then call_vector_brush args[0], args[1]
        when 27 then call_enum_object args[0], args[1]
        when 28 then call_object_int args[0], args[1]
        when 29 then call_object_enum args[0], args[1]
        when 30 then call_object_string args[0], args[1]
        when 31 then call_int_int_int args[0], args[1], args[2]
        when 32 then call_int_int_enum args[0], args[1], args[2]
        when 33 then call_int_int_string args[0], args[1], args[2]
        when 35 then call_int_int_font args[0], args[1], args[2]
        when 36 then call_vector_string_vector args[0], args[1], args[2]
        when 37 then call_vector_int_int args[0], args[1], args[2]
        when 38 then call_object_string_string args[0], args[1], args[2]
        when 39 then call_int_object_string_string args[0], args[1], args[2], args[3]
        end
    end
    end # Metaclass
end #add_methods_to

def stop_gui()
    Native.stop_event_loop
end

def to_widget_name(sym)
	sym.to_s.gsub('_', '-')
end

def Widget(name, parent=nil)
   	p "creating #{name.to_s} with parent #{parent}"
    w = Native.object_(name.to_s, parent) 
	raise BadWidgetNameError unless w
   	add_methods_to w
	w
end # widget

#QObject
    def sender
        obj = Native.sender
        add_methods_to obj
        return obj
    end

    def object_list
        Native.object_list
    end

#MessageBox
    def message_box(type, text, title=nil, but0="Yes", but1="No", but2="", default=0)
        case type
        when :info then Native.message_box_(title ? title : "Information", text, 0)
        when :warning then Native.message_box_(title ? title : "Warning", text, 1)
        when :error then Native.message_box_(title ? title : "Error", text, 2)
        when :question then Native.message_box_question(title ? title : "Question", text, but0, but1, but2, default)
        end
    end

#InputDialog
    def input_dialog(type, label, caption="Input Dialog", cancel=-1, value=nil, min=-0x40000000, max=0x3fffffff, decimals=2)
        case type
        when :int 
            value = 0 if value == nil
            Native.input_dialog_int caption, label, cancel, value, min, max
        
        when :float 
            value = 0 if value == nil
            Native.input_dialog_float caption, label, Float(cancel), Float(value), min, max, decimals
        when :text 
            value = "" if value == nil
            Native.input_dialog_text caption, label, value
        end
    end
#InputDialog
    def file_dialog(type, dir="", title="File Dialog", filter="")
        file = case type
                when :dir then Native.file_dialog_existing_directory title, dir
                when :"open-file" then Native.file_dialog_ title, dir, filter, 0
                when :"save-file" then Native.file_dialog_ title, dir, filter, 1
                end
        file
    end

#FontDialog
    def font_dialog(default = nil)
        Native.font_dialog_ default
    end

#ColorDialog
    def color_dialog(default = nil)
        Native.color_dialog_ default
    end

    def error_message(txt)
        error_message = Widget :'error-message'
        error_message.setp :modal=>true
        error_message.slotcall :'show-message', txt
    end

#ProgressDialog
    def progress_dialog(label, maximum)
        #pressed, progressCancelledException will be thrown
        progress = Widget :'progress-dialog'
        progress.setp(:'label-text'=>label)
        progress.setp(:maximum=>maximum)
        set_progress = Proc.new { |val|
            if progress.getp(:'was-canceled')
                throw :progressCancelledException
            end
            progress.setp(:value=>val)
            Native.process_events
        }
        if block_given?
            yield set_progress
        end
        progress.destroy 
    end

#Layout   
    def add_box(layout1, type, add)
        case type
            when :layout then Native.box_add_layout(layout1, add)
            when :spacing then Native.box_add_spacing(layout1, add)
            when :stretch then Native.box_add_stretch(layout1, 0)
            when :widget  then Native.box_add_widget(layout1, add)
            end
    end

    def grid(layout, type, add, row, col, row_span=1, col_span=1)
        case type
            when :layout then 
                Native.grid_add_layout(layout, add, row, col, row_span, col_span)
            when :widget then 
                Native.grid_add_widget(layout, add, row, col, row_span, col_span)
            end
    end
end