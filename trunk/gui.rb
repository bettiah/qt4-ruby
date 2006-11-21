require 'qt'
include Qt4

module Gui

   Qt4.Init

   set_trace_func proc { |event, file, line, id, binding, classname|
      printf "%8s %s:%-2d %10s %8s\n", event, file, line, id, classname
   }

   set_trace_func nil

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

         def window(*args, &block)
            method_missing(:main_window, *args, &block)
         end

         def method_missing(to_create, *args, &block)
            p "method missing for #{to_create}"
            obj = nil
            if instance_variables.include?("@#{to_create}")
               instance_variable_get("@#{to_create}")
            elsif @node and item = @node.item and item.respond_to? to_create
               p "sending to #{item}"
               return item.send(to_create, *args, &block)
            else
               obj = create_widget!(to_create, *args)
            end

            if block
               capture &block
            end
            obj
         end

         private
         def capture(&block)
            old_node = @parent_node
            @parent_node = @node
            instance_eval(&block)
            @parent_node = old_node
         end

         def create_widget!(sym, *args)
            obj = QWidget.new(sym, find_parent)
            raise BadWidgetNameError unless obj
            p "created #{obj}"
            @node = @widgets.add_node_under(@parent_node, obj)
            if args.first.class == Symbol
               instance_variable_set("@#{args.first.to_s}", obj)
            end
            args.each do |arg|
               if arg.respond_to?(:to_hash)
                  obj.setp arg.to_hash
               end
            end
            additional_methods_for sym, obj
            obj
         end

         def find_parent
            parent_node = @parent_node
            while parent = parent_node
               break unless(@layouts.include? parent.item)
               parent_node = parent_node.parent
            end
            parent ? parent.item : nil
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
            end
         end

         def add_grid_layout_methods_to(obj)
            obj.instance_variable_set(:@row_count, 0)
            obj.instance_variable_set(:@col_count, 0)
            class << obj
               include GridLayout # unless defined? Layout
            end
         end

         def add_box_layout_methods_to(obj)
            class << obj
               include BoxLayout # unless defined? Layout
            end
         end

      end #Builder

      module GridLayout
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

      module BoxLayout
         def box(arg = nil)
            if arg
               add_box(find_layout, :widget, arg)
            else
               add_box(find_layout, :stretch, nil)
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
