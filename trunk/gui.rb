require 'qt'
include Qt4

module Gui

   GC.disable
   Qt4.Init

   def with_gui
      $app = Qt4::Native.make_qt_gui unless $app
      #ObjectSpace.define_finalizer($app, proc {Qt4::Native.destroy_gui}) if $app
      if block_given?
         main = yield
         raise NoMainWidgetError unless main
         main.show
         Qt4::Native.start_event_loop
      end
   end

   module View

      class Builder
         def initialize
            @layout_parent = nil
            @widget_parent = nil
            @parents = []
            @layouts = []
            @level = 0
         end

         def window(*args, &block)
            method_missing(:main_window, *args, &block)
         end

         def method_missing(to_create, *args, &block)
            p "method missing for #{to_create}"
            obj = nil
            if instance_variables.include?("@#{to_create}")
               instance_variable_get("@#{to_create}")
            elsif @widget_parent && (@widget_parent.respond_to? to_create)
               return @widget_parent.send(to_create, *args, &block)
            else
               obj = create_widget!(to_create, *args)
            end

            if block
               @parents.push @widget_parent
               @widget_parent = obj unless (to_create.to_s=~/layout/) and obj
               p @widget_parent
               capture &block
               @widget_parent = @parents.pop
            end
            obj
         end

         def capture(&block)
            @level += 1
            @layouts.push @layout_parent
            item = instance_eval(&block)
            @layout_parent = @layouts.pop
            @level -= 1
            item
         end

         private
         def create_widget!(sym, *args)
            obj = QWidget.new(sym, @widget_parent)
            raise BadWidgetNameError unless obj
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

         def additional_methods_for(sym, obj)
            if sym.to_s =~ /grid_layout/
               @layout_parent = obj
               add_grid_layout_methods_to self
            elsif sym.to_s =~ /box_layout/
               @layout_parent = obj
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
         def row(*args, &block)
            @col_count = 0
            if block
               capture &block
            end
            @col_count = 0
            @row_count += 1
         end

         def item(arg = nil, row_span = 1, col_span = 1)
            grid(@layout_parent, :widget, arg, @row_count, @col_count, row_span, col_span) if arg
            @col_count += col_span
         end

         def layout(row_span = 1, col_span = 1, &block)
            old_parent= @widget_parent
            @widget_parent = nil
            arg = capture &block
            @widget_parent = old_parent
            grid(@layout_parent, :layout, arg, @row_count, @col_count, row_span, col_span)
            @row_count += 1
            @col_count += col_span
         end
      end #GridLayout

      module BoxLayout
         def box(arg = nil)
            if arg
               add_box(@layout_parent, :widget, arg)
            else
               add_box(@layout_parent, :stretch, nil)
            end
         end
      end

	  class Widget
	  end

   end #View
end #Gui
