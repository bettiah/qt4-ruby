require 'qt'

module Gui
   Qt4::Native.ini

   set_trace_func proc { |event, file, line, id, binding, classname|
      printf "%8s %s:%-2d %10s %8s\n", event, file, line, id, classname
   }

   set_trace_func nil

   Widgets = Qt4::Native.object_list.gsub!('-','_').split('^')
   Widgets.each { |ob|
      cls = ob.split('_').each{|m| m.capitalize!}.join
      eval <<-END
      class #{cls} < Qt4::QWidget
         def initialize(*a, &b)
            super(:#{ob}, *a, &b)
         end
      end
      END
   }
   
   @@dummy = nil
   def sender()
      @@dummy = Qt4::QWidget.new :widget unless @@dummy
      Qt4::Native.signal_sender(@@dummy) 
      @@dummy
   end
  
   def with_gui
      Qt4::Native.make_gui
      main = nil
      if block_given?
         main = yield
         raise NoMainWidgetError unless main
         main.show
         Qt4::Native.start_event_loop
      end
      Qt4::Native.destroy_gui
   end

    #MessageBox
    def message_box(type, text, title=nil, but0="Yes", but1="No", but2="", default=0)
        case type
        when :info then Qt4::Native.message_box_(title ? title : "Information", text, 0)
        when :warning then Qt4::Native.message_box_(title ? title : "Warning", text, 1)
        when :error then Qt4::Native.message_box_(title ? title : "Error", text, 2)
        when :question then Qt4::Native.message_box_question(title ? title : "Question", text, but0, but1, but2, default)
        end
    end

    #InputDialog
    def input_dialog(type, label, caption="Input Dialog", cancel=-1, value=nil, min=-0x40000000, max=0x3fffffff, decimals=2)
        case type
        when :int 
            value = 0 if value == nil
            Qt4::Native.input_dialog_int caption, label, cancel, value, min, max
        
        when :float 
            value = 0 if value == nil
            Qt4::Native.input_dialog_float caption, label, Float(cancel), Float(value), min, max, decimals
        when :text 
            value = "" if value == nil
            Qt4::Native.input_dialog_text caption, label, value
        end
    end

    #FileDialog
    def file_dialog(type, dir="", title="File Dialog", filter="")
        file = case type
                when :dir then Qt4::Native.file_dialog_existing_directory title, dir
                when :"open-file" then Qt4::Native.file_dialog_ title, dir, filter, 0
                when :"save-file" then Qt4::Native.file_dialog_ title, dir, filter, 1
                end
        file
    end

    #FontDialog
    def font_dialog(default = nil)
        Qt4::Native.font_dialog_ default
    end

    #ColorDialog
    def color_dialog(default = nil)
        Qt4::Native.color_dialog_ default
    end

    def show_error_message(txt)
        error_message = ErrorMessage.new
        error_message.setp :modal=>true
        error_message.slotcall :show_message, txt
    end

    #ProgressDialog
    def show_progress_dialog(label, maximum)
        #pressed, progressCancelledException will be thrown
        progress = ProgressDialog.new
        progress.setp(:label_text=>label)
        progress.setp(:maximum=>maximum)
        set_progress = Proc.new { |val|
            if progress.getp(:was_canceled)
                throw :progressCancelledException
            end
            progress.setp(:value=>val)
            Qt4::Native.process_events
        }
        if block_given?
            yield set_progress
        end
        progress.destroy 
    end

   module View

      class Builder
         def initialize
            @widgets = Tree.new
            @layouts = []
            @parent_node = nil
            @current_node = nil
         end

         Widgets.each { |ob|
            module_eval <<-END
            def #{ob}(*a, &b)
               create_widget!(:#{ob.to_sym}, *a, &b)
            end
            END
         }

         def window(*args, &block)
            main_window(*args, &block)
         end

        private
         def create_widget!(sym, *args, &block)
            par = find_parent
            args.unshift par if par
            obj = Qt4::QWidget.new sym, *args
            raise BadWidgetNameError unless obj
            @current_node = @widgets.add_node_under(@parent_node, obj)
            args.shift if par
            if args.first.class == Symbol
               instance_variable_set("@#{args.first.to_s}", obj)
            end

            additional_methods_for sym, obj
            if block
                capture &block
            end
            obj
         end

         #return the first parent that is not a layout
         def find_parent
            parent_node = @parent_node
            while parent = parent_node
               break unless(@layouts.include? parent.item)
               parent_node = parent_node.parent
            end
            parent ? parent.item : nil
         end

         def capture(&block)
            old_node = @parent_node
            @parent_node = @current_node
            instance_eval(&block)
            @parent_node = old_node
         end

         def find_layout
            parent_node = @parent_node
            while parent = parent_node
               break if(@layouts.include? parent.item)
               parent_node = parent_node.parent
            end
            parent.item
         end

         def additional_methods_for(sym, obj)
            if sym.to_s =~ /grid_layout/
               @layouts.push obj
               add_grid_layout_methods_to self
            elsif sym.to_s =~ /box_layout/
               @layouts.push obj
               add_box_layout_methods_to self
            elsif sym.to_s =~ /graphics_view/
               add_graphics_view_methods_to self
            elsif sym.to_s =~ /list_view/
               add_view_methods_to obj
            end
         end

         def add_grid_layout_methods_to(obj)
            obj.instance_variable_set(:@row_count, 0)
            obj.instance_variable_set(:@col_count, 0)
            class << obj
               include GridLayoutMethods # unless defined? Layout
            end
         end

         def add_box_layout_methods_to(obj)
            class << obj
               include BoxLayoutMethods # unless defined? Layout
            end
         end

         def add_graphics_view_methods_to(obj)
            class << obj
               include GraphicsViewMethods
            end
         end

         def add_view_methods_to(obj)
             class << obj
                 include ViewMethods
             end
         end

      end #Builder

      module ViewMethods
          attr_reader :model
          @model = nil
          def set_as_model(model)
              raise UnKnownModelError, "Attempting to overwrite existing model" if @model
              case model
              when Array
                  adapter = ArrayAdapter.new model
              when Hash
                  adapter = HashAdapter.new model
              else 
                  raise UnKnownModelError, "Model has to be a list or an array"
              end
              @model = adapter
              obs = set_rb_model @model
              @model.set_observer obs
          end
      end

      class ArrayAdapter 
          def initialize(model)
              @model = model
              @views = []
          end

          def set_observer(view)
              @views.push view
          end

          def []=(idx, data)
              @model[idx] = data
              @views.each { |v|  v.update_view idx, 0 }
          end

          def at(idx)
              @model.at idx
          end

          def length()
              @model.length
          end

          def push(item, pos)
              @views.each { |v|  v.update_view idx, 0 }
              @model.push
          end
      end

      module GridLayoutMethods
        def grid(layout, type, add, row, col, row_span=1, col_span=1)
            case type
                when :layout then
                    layout.grid_add_layout(add, row, col, row_span, col_span)
                when :widget then
                    layout.grid_add_widget(add, row, col, row_span, col_span)
                end
        end
  
        def row(&block)
            @col_count = 0
            if block
               yield
            end
            @col_count = 0
            @row_count += 1
         end

         def item(arg = nil, options = {})
            conf = {:row_span => 1, :col_span => 1}
            conf.update options if options.is_a? Hash
            grid(find_layout, :widget, arg, @row_count, @col_count, conf[:row_span], conf[:col_span]) if arg
            @col_count += conf[:col_span]
         end

         def layout(options = {})
            conf = {:row_span => 1, :col_span => 1}
            conf.update options if options.is_a? Hash
            arg = yield if block_given?
            grid(find_layout, :layout, arg, @row_count, @col_count, conf[:row_span], conf[:col_span])
            @col_count += conf[:col_span]
         end
     end #GridLayout

     module BoxLayoutMethods
         def add_box(layout, type, add)
            case type
                when :layout then layout.box_add_layout(add)
                when :spacing then layout.box_add_spacing(add)
                when :stretch then layout.box_add_stretch(0)
                when :widget  then layout.box_add_widget(add)
                end
         end

         def box(arg = nil)
            if arg
               add_box(find_layout, :widget, arg)
            else
               add_box(find_layout, :stretch, nil)
            end
         end

         def box_layout(arg)
            add_box(find_layout, :layout, arg)
         end
      end

      module GraphicsViewMethods
         def method_missing(sym, *args, &block)
            if sym.to_s =~ /graphics.*item/
               create_widget!(sym, *args, &block)
            else
               raise NoMethodError
            end
         end
      end

      class Tree
         def initialize
            @nodes = []
         end

         def add_node_under(parent, element)
            node = Node.new element, parent
            @nodes.push node
            node
         end
      end

      class Node
         attr_reader :parent, :children, :item

         def initialize(item, parent = nil)
            @item = item
            @parent = parent
            @children = []
         end

         def ancestors
            node, nodes = self, []
            nodes << node = node.parent while node.parent
            nodes
         end

         def root
            node = self
            node = node.parent while node.parent
            node
         end

         def siblings
            self_and_siblings - [self]
         end

         def self_and_siblings
            parent ? parent.children : self.class.root
         end
      end #Node


   end #View
end #Gui
