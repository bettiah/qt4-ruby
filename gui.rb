require 'qt'
include Qt4

module Gui

   Qt4.Init

   set_trace_func proc { |event, file, line, id, binding, classname|
      printf "%8s %s:%-2d %10s %8s\n", event, file, line, id, classname
   }

   set_trace_func nil

   Widgets = object_list.gsub!('-','_').split('^')
   Widgets.each { |ob|
      cls = ob.split('_').each{|m| m.capitalize!}.join
      eval <<-END
      class #{cls} < QWidget
         def initialize(*a, &b)
            super(:#{ob}, *a, &b)
         end
      end
      END
   }

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

   module View

      class Builder
         def initialize
            @widgets = Tree.new
            @layouts = []
            @parent_node = nil
            @node = nil
         end

         Widgets.each { |ob|
            eval <<-END
            def #{ob}(*a, &b)
               create_widget!(:#{ob.to_sym}, *a, &b)
            end
            END
         }

         def window(*args, &block)
            main_window(*args, &block)
         end

         def find_parent
            parent_node = @parent_node
            while parent = parent_node
               break unless(@layouts.include? parent.item)
               parent_node = parent_node.parent
            end
            parent ? parent.item : nil
         end

         private
         def create_widget!(sym, *args, &block)
            p = find_parent
            args.unshift p if p
            obj = QWidget.new sym, *args
            raise BadWidgetNameError unless obj
            @node = @widgets.add_node_under(@parent_node, obj)
            args.shift if p
            if args.first.class == Symbol
               instance_variable_set("@#{args.first.to_s}", obj)
            end
            additional_methods_for sym, obj

            if block
               capture &block
            end

            obj
         end

         def capture(&block)
            old_node = @parent_node
            @parent_node = @node
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
