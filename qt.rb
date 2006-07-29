require 'native'

module Qt4

#Initialization
Native.ini

def add_methods_to(obj)
    class << obj

    def destroy()
        Native.destroy_(self)
    end

    def show(milliSecond=0)
        Native.show_delayed(self, milliSecond, 0)
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
        args.each { |name,value|
            set_prop(name, value)
        }
    end
    
    def set_prop(name, val)
        case "#{val.class}"
            when "String" then Native.set_property_string(self, name.to_s, val)
            when "Fixnum" then Native.set_property_int(self, name.to_s, val)
            when "Symbol" then Native.set_property_enum(self, name.to_s, val.to_s)
            when "Array"
                case val.length 
                    when 2 then Native.set_property_point_or_size(self, name.to_s, val.join(' '))
                    when 4 then Native.set_property_rect(self, name.to_s, val.join(' '))
                    end
            when "TrueClass" then Native.set_property_bool(self, name.to_s, 1)
            when "FalseClass" then Native.set_property_bool(self, name.to_s, 0)
            else p "#{val.class}: #{name.to_s} did not match"
            end
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
            when 64 then Native.get_property_font
            when 67 then Native.get_property_color
            else "#{:native.class}: #{name.to_s} did not match"
            end
    end

    def set_font(size, family="", bold=0, italic=0, underline=0, strikeout=0)
        Native.set_font_(self, family, size, bold, italic, underline, strikeout)
    end
    
    def set_color(role, name)
        Native.set_color_(self, (case role
            when :window then 10
            when :'window-text' then 0
            when :base then 9
            when :'alternate-base' then 16
            when :text then 6
            when :button then 1
            when :'button-text' then 8 
            end), name)
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

    MAX_SIZE=32767
    def allow_expanding()
        setp :'maximum-width'=>MAX_SIZE, :'maximum-height'=>MAX_SIZE
    end

#Signal Slots
    def connect(signal, &aproc)
        Native.connect_(self, signal.to_s=>aproc) if block_given?; return
    end

    def eval_callbacks
        
        @caller = nil; @callbacks = nil
    end

    def disconnect()
    end

    def method_missing(method_symbol, *parameters)#:nodoc:
        slotcall method_symbol.id2name, *parameters
    end
      
    def slotcall(name, *args)
        name = name.to_s
        case args.length
        when 0
            p "slotcall #{name}"
            Native.slotcall_void self ,name
        when 1
            p "slotcall #{name} #{args[0].class}"
            case "#{args[0].class}"
            when "Fixnum"
                Native.slotcall_int self, name, args[0]
            when "String"
                Native.slotcall_string self, name, args[0]
            when "Array"
                Native.slotcall_string_list self, name, args[0]
            when "SWIG::TYPE_p_void"
                Native.slotcall_object self, name, args[0]
            end
        when 2
            p "slotcall #{name} #{args[0].class} #{args[1].class}"
            case "#{args[0].class}"
            when "Fixnum"
                case "#{args[1].class}"
                when "Fixnum"
                    Native.slotcall_int_int self, name, args[0], args[1]
                when "String"
                    Native.slotcall_int_string self, name, args[0], args[1]
                when "Array"
                    Native.slotcall_int_string_list self, name, args[0], args[1]
                end
            when "Symbol"
                Native.slotcall_enum_object self ,name, args[0], args[1]
            when "SWIG::TYPE_p_void"
                Native.slotcall_object_string self, name, args[0], args[1]
            end
        when 3
            p "slotcall #{name} #{args[0].class} #{args[1].class} #{args[2].class}"
            case "#{args[2].class}"
            when "Fixnum"
                Native.slotcall_int_int_int self ,name, args[0], args[1], args[2]
            when "Symbol"
                Native.slotcall_int_int_enum self, name, args[0], args[1], args[2]
            when "String"
                Native.slotcall_int_int_string self ,name, args[0], args[1], args[2]
            end
        end
    end
    end # Metaclass
end

#MainLoop
def with_qt()
    Native.make_gui
    if block_given?
        yield
        Native.start_event_loop
        end
    Native.destroy_gui
end

def Widget(name, parent=nil, *syms)
    p "creating #{name.to_s} with parent #{parent.class}"

    if syms.empty?
        obj = Native.object_(name.to_s, parent) 
        add_methods_to obj
        return obj
    end
    syms.each { |sym| 
        eval "@#{sym} = Widget(name.to_s, parent)"
    }
end # widget

#QObject
    def connect(from, signal, aproc)
        Native.connect_(from, signal.to_s=>aproc) if block_given?; return 
    end

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
        p type
        file = case type
                when :dir then Native.file_dialog_existing_directory title, dir
                when :"open-file" then Native.file_dialog_ title, dir, filter, 0
                when :"save-file" then Native.file_dialog_ title, dir, filter, 1
                end
        p file
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
    def box(layout1, type, add)
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
