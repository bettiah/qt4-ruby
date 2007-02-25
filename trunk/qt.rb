class Symbol
    def to_qt_name
        to_s.gsub('_', '-')
    end
end

class String
    def to_qt_name
        to_s.gsub('_', '-')
    end
end

module Qt4

require "Native"

class NoMainWidgetError < RuntimeError
end

class BadSlotNameError < RuntimeError
end

class BadWidgetNameError < RuntimeError
end

class CouldNotConnectError < RuntimeError
end

class UnknownModelError < RuntimeError
end

def stop_gui()
    Native.stop_event_loop
end

class QWidget < Native::QtProxy
    def initialize(sym, *args, &block)
        @signals = {}
        if args.first.kind_of? QWidget
            parent = args.first
        else parent = nil
        end
        p "creating #{self} with parent #{parent}"
        super sym.to_qt_name, parent

        args.each do |arg|
            self.setp(arg.to_hash) if arg.respond_to?(:to_hash)
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
            case get_property_type name.to_qt_name
            when 1 then Native.set_property_bool case val 
                                            when TrueClass then 1
                                            when FalseClass then 0
                                            end
            when 2 then 
                case val
                    when Fixnum then Native.set_property_int val
                    else Native.set_property_enum val.to_qt_name
                    end
            when 10, 76 then Native.set_property_string val.to_qt_name
            when 14 then Native.set_property_date val
            when 15 then Native.set_property_time val
            when 16 then Native.set_property_date_time val
            when 17 then Native.set_property_url val
            when 19 then Native.set_property_rect val
            when 20 then Native.set_property_rect_f val
            when 21, 25 then Native.set_property_point_or_size val.to_s
            when 26 then Native.set_property_point_f val
            when 32 then Native.set_property_line val
            when 24 then Native.set_property_line_f val
            when 64 then Native.set_property_font val.to_s
            when 66 then Native.set_property_brush val
            when 67 then Native.set_property_color val
            when 71 then Native.set_property_polygon val
            when 74, 75 then Native.set_property_enum val.to_qt_name
            when 77 then Native.set_property_pen val
            end
        }
    end
    
    def getp(name)
        case get_property_type name.to_qt_name
            when 1 then (Native.get_property_bool == 1) ? true : false
            when 2 then Native.get_property_int
            when 10, 17 then Native.get_property_string
            when 14 then Native.get_property_date
            when 15 then Native.get_property_time
            when 16 then Native.get_property_date_time
            when 19 then Native.get_property_rect
            when 21, 25 then Native.get_property_point_or_size
            when 23 then Native.get_property_line
            when 24 then Native.get_property_line_f
            when 26 then Native.get_property_point_f
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
        signal = signal.to_qt_name
#            args = argument_types_(self, signal.to_s)
#            raise BadSlotNameError, "Could not find signal #{signal} in Object" unless args
        callbacks.each {|callback|
            cb = connect_ signal
            raise CouldNotConnectError, "Could not connect to signal #{signal}" if cb == nil
            case callback
            when Symbol
                raise CouldNotConnectError, "Could not connect to local symbol" unless self.respond_to? callback
                callback = method(callback)
            when Proc
            when Method
            end
            add_callbacks_for signal, callback, cb
        }
    end

    def connect_qt(signal, to, slot)
        connect_qt_ signal.to_qt_name, to, slot.to_qt_name, 1
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

    def add_callbacks_for(signal, aproc, cb = nil)
        @signals[signal] ||= []
        @signals[signal].push [aproc, cb]
        # add the callback to hash to keep reference count
        cb.set_callback aproc if cb
        #self.class.read_inheritable_attribute(signal.to_sym) or []
    end

    def disconnect()
    end

#Events
    def event_filter(signal, callback, eat=0)
        signal = signal.to_qt_name
        Native.event_filter_ signal, callback, eat
        add_callbacks_for(signal, callback)
    end

#Slot calls
    def method_missing(method_symbol, *parameters)#:nodoc:
        #p "method_missing slotcall for #{method_symbol}"
        slotcall method_symbol.to_qt_name, *parameters
    end
      
    def slotcall(name, *args)
        case call_type name.to_qt_name
        when 1 then Native.call_void
        when 2 then Native.call_bool args[0]
        when 3 then Native.call_int args[0]
        when 4 then Native.call_float args[0].to_f
        when 5 then Native.call_enum args[0]
        when 6 then Native.call_string args[0]
        when 7 then Native.call_string_list args.join("^")
        when 8 then Native.call_url args[0]
        when 9, 10, 11, 12, 13 then Native.call_vector args[0]
        when 14 then Native.call_color args[0]
        when 15 then Native.call_font args[0]
        when 16 then Native.call_pen args[0]
        when 17 then Native.call_brush args[0]
        when 18 then call_object args[0]
        when 19 then Native.call_int_int args[0], args[1]
        when 20 then Native.call_int_string args[0], args[1]
        when 21 then Native.call_int_string_list args.shift, args.join("^")
        when 22 then Native.call_float_float args[0].to_f, args[1].to_f
        when 23 then Native.call_float_color args[0].to_f, args[1]
        when 24 then Native.call_string_int args[0], args[1]
        when 25 then Native.call_vector_string args[0], args[1]
        when 26 then Native.call_vector_brush args[0], args[1]
        when 27 then call_enum_object args[0], args[1]
        when 28 then call_object_int args[0], args[1]
        when 29 then call_object_enum args[0], args[1]
        when 30 then call_object_string args[0], args[1]
        when 31 then Native.call_int_int_int args[0], args[1], args[2]
        when 32 then Native.call_int_int_enum args[0], args[1], args[2]
        when 33 then Native.call_int_int_string args[0], args[1], args[2]
        when 34 then Native.call_int_int_font args[0], args[1], args[2]
        when 36 then Native.call_vector_string_vector args[0], args[1], args[2]
        when 37 then Native.call_vector_int_int args[0], args[1], args[2]
        when 38 then call_object_string_string args[0], args[1], args[2]
        when 39 then call_int_object_string_string args[0], args[1], args[2], args[3]
        end
    end
end #QWidget

end #module Qt4


