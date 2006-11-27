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

class QWidget < QtProxy
    def initialize(sym, *args, &block)
        @signals = {}
        if args.first.class == QWidget
			parent = args.first
		else parent = nil
		end
        p "creating #{sym.to_s} with parent #{parent}"
		super to_qt_name(sym), parent

		args.each do |arg|
			if arg.respond_to?(:to_hash)
				self.setp arg.to_hash
			end
		end
		yield self if block
    end

    def show(milliSecond=0, mode=0)
        show_delayed(milliSecond, 
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
        slotcall :repaint
        Native.process_events
    end

#Property
    def setp(args)
        args.each { |name, val|
            case get_property_type to_qt_name(name)
            when 1 then set_property_bool case val 
                                            when TrueClass then 1
                                            when FalseClass then 0
                                            end
            when 2 then 
                case val
                    when Fixnum then set_property_int val
					else set_property_enum to_qt_name(val)
                    end
            when 10, 76 then set_property_string to_qt_name(val)
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
            when 74, 75 then set_property_enum to_qt_name(val)
            when 77 then set_property_pen val
            end
        }
    end
    
    def getp(name)
        case get_property_type to_qt_name(name)
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
        access_list_(case type
             when :property then 0
             when :signal then 1
             when :slot then 2
             end, 0)
    end

    def enum_property_list()
        access_list_(0, 1)
    end
    
    def enum_list(name)    
         enum_list_ to_string(name)
    end

#Connections
    def connect(signal, *callbacks, &block)
        cb = nil
        args = nil
        callbacks << block if block_given?
		signal = to_qt_name signal
#            args = argument_types_(self, signal.to_s)
#            raise BadSlotNameError, "Could not find signal #{signal} in Object" unless args
 		callbacks.each {|callback|
			cb = connect_ signal
            raise CouldNotConnectError, "Could not connect to signal" unless cb
			case callback
			when Symbol
                raise CouldNotConnectError, "Could not connect to local symbol" unless self.respond_to? callback
				callback = method(callback)
			when Proc
            when Method
			end
            add_callbacks_for signal, cb, callback
		}
    end

	def connect_qt(signal, to, slot)
		connect_qt_ to_qt_name(signal), to, to_qt_name(slot), 1
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

	def add_callbacks_for(signal, cb, aproc)
        @signals[signal] ||= []
        @signals[signal].push [cb, aproc]
		# add the callback to hash to keep reference count
		cb.set_callback aproc
        #self.class.read_inheritable_attribute(signal.to_sym) or []
	end

    def disconnect()
    end

#Slot calls
    def method_missing(method_symbol, *parameters)#:nodoc:
        p "method_missing slotcall for #{method_symbol}"
        slotcall to_qt_name(method_symbol), *parameters
    end
      
    def slotcall(name, *args)
        case call_type to_qt_name(name)
        when 1 then call_void
        when 2 then call_bool args[0]
        when 3 then call_int args[0]
        when 4 then call_float args[0].to_f
        when 5 then call_enum args[0]
        when 6 then call_string args[0]
        when 7 then call_string_list args.join("^")
        when 8 then call_url args[0]
        when 9, 10, 11, 12, 13 then call_vector arg[0]
        when 14 then call_color args[0]
        when 15 then call_font args[0]
        when 16 then call_pen args[0]
        when 17 then call_brush args[0]
        when 18 then call_object args[0]
        when 19 then call_int_int args[0], args[1]
        when 20 then call_int_string args[0], args[1]
        when 21 then call_int_string_list args[0], args.join("^")
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
end #QWidget

def stop_gui()
    Native.stop_event_loop
end

def to_qt_name(sym)
	sym.to_s.gsub('_', '-')
end

#QObject
	@@dummy = nil
    def sender()
    	@@dummy = QWidget.new :widget unless @@dummy
		signal_sender() 
		@@dummy
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
        error_message = QWidget.new :'error-message'
        error_message.setp :modal=>true
        error_message.slotcall :show_message, txt
    end

#ProgressDialog
    def progress_dialog(label, maximum)
        #pressed, progressCancelledException will be thrown
        progress = QWidget.new :'progress-dialog'
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
    def add_box(layout, type, add)
        case type
            when :layout then layout.box_add_layout(add)
            when :spacing then layout.box_add_spacing(add)
            when :stretch then layout.box_add_stretch(0)
            when :widget  then layout.box_add_widget(add)
            end
    end

    def grid(layout, type, add, row, col, row_span=1, col_span=1)
        case type
            when :layout then
                layout.grid_add_layout(add, row, col, row_span, col_span)
            when :widget then
                layout.grid_add_widget(add, row, col, row_span, col_span)
            end
    end

end #module


