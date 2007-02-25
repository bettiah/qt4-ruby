require 'gui'
include Gui

def ostats(last_stat = nil)
 stats = Hash.new(0)
 ObjectSpace.each_object {|o| stats[o.class] += 1}

 stats.sort {|(k1,v1),(k2,v2)| v2 <=> v1}.each do |k,v|
   printf "%-30s  %10d", k, v
   printf " | delta %10d", (v - last_stat[k]) if last_stat
   puts
 end

 stats
end

class Browser < View::Builder

    def initialize()
        super
        @stats = nil
    end
    
   def get_kwd(widget)
      widget.getp(:current_text).delete!(':').to_sym
   end

   def show_font_dialog
      fnt = font_dialog(@selected_font.getp(:text))
      @selected_font.setp :text=>fnt
   end

   def show_color_dialog
      color = color_dialog(@selected_color.getp(:text))
      @selected_color.setp :text=>color if color
   end

   def heavy_calculation
      max = 50
      show_progress_dialog("Calculating", max) { |set_progress|
         catch :progressCancelledException do
            (1..max).each { |n| sleep(0.1); set_progress.call(n+1) }
         end
         message_box(:warning, "The calculation has been canceled!")
      }
   end

   def toolbar_add(bar, *objects)
     objects.each do |ob|
         bar.add_widget ob
         bar.add_separator
     end
   end

   def add_gui_dialog(main)
       tool_bar(:bar1) do
           toolbar_add(@bar1, push_button(:message), combo_box(:type_message), 
                       push_button(:input), combo_box(:type_input), push_button(:file), 
                       combo_box(:type_file), push_button(:progress))
       end
       
       tool_bar(:bar2) do
           toolbar_add(@bar2, push_button(:font), line_edit(:selected_font), 
                       push_button(:color), line_edit(:selected_color), push_button(:error), 
                       line_edit(:error_text))
       end

       main.add_tool_bar @bar1
       main.add_tool_bar @bar2
       main.insert_tool_bar_break @bar2

         [
            [@message,      "message-box"],
            [@input,        "input-dialog"],
            [@file,         "file-dialog"],
            [@font,         "font-dialog"],
            [@color,        "color-dialog"],
            [@error,        "error-message"],
            [@error_text,   "My error message."],
            [@progress,     "with-progress-dialog"]
         ].each do |w, val|
            w.setp :text=>val
         end

         [
            [@type_message, [":info", ":warning", ":error", ":question"]],
            [@type_input,   [":int", ":float", ":text"]],
            [@type_file,    [":open-file", ":save-file", ":dir"]]
         ].each do |obj, rst|
            obj.add_items rst.join('^')
         end

         [
            [@message, lambda { message_box get_kwd(@type_message), "Message Box" }],
            [@input, lambda { input_dialog get_kwd(@type_input), "Please Enter" }],
            [@file, lambda { file_dialog get_kwd(@type_file), "Name" }],
            [@font, method(:show_font_dialog)],
            [@color, method(:show_color_dialog)],
            [@error, lambda { show_error_message(@error_text.getp(:text)) }],
            [@progress, method(:heavy_calculation)]
         ].each do |obj, prc|
            obj.connect :clicked, prc
         end
   end

   def populate(widget, obj, type)
      lst = (obj.access_list type).split('^')
      lst.map! do |item|
         i = if item.length == 0
            "<br>"
             else
            if item =~ /\:$/
               "<b>#{item}</b><br>"
            elsif item =~ /\s/
               "#{$`} <font color=#4040ff>#{$'}</font><br>"
            else
                "#{item}<br>"
            end
         end
         i
      end #do
      widget.setp :html=>lst.join
   end

      def populate_enums(name)
         @enums.clear
         @enums.add_items $temp_object.enum_list(name)
      end

      def update_object_info(name)
         $temp_object.destroy if $temp_object
         $temp_object = Qt4::QWidget.new name
         populate(@properties, $temp_object, :property)
         populate(@signals, $temp_object, :signal)
         populate(@slots, $temp_object, :slot)

         @enum_properties.clear
         @enum_properties.add_items $temp_object.enum_property_list
         populate_enums(@enum_properties.getp(:current_text))
      end

      def gui_object()
         group_box(:title=>'Objects', :flat=>true) do
            grid_layout do
               row {
                  layout(:row_span=>1, :col_span=>3) { h_box_layout do
                     box combo_box(:objects)
                     box label(:text=>"  Enum")
                     box combo_box(:enum_properties, :size_adjust_policy=>':adjust-to-contents')
                     box combo_box(:enums, :size_adjust_policy=>':adjust-to-contents')
                     box
                     box label(:text=>'Events')
                     box combo_box(:events)
                  end}
               }
               row {
                  item label(:text=>"<b><font color=#ff4020>Properties</font></b><br>\
                  object.<b>getp</b> name<br>\
                  object.<b>setp</b> name=>value")
                  item label(:text=>"<b><font color=#ff4020>Signals</font></b><br>\
                  from-object.<b>connect</b> signal<br>\
                  from-object.<b>connect</b> signal")
                  item label(:text=>"<b><font color=#ff4020>Slots</font></b><br>\
                  to-object slot<br>\
                  slot")
               }
               row {
                  item text_edit(:properties)
                  item text_edit(:signals), :row_span=>2
                  item text_edit(:slots), :row_span=>2
               }
               row { item label(:text => "[r] ... read only") }
            end

            [@properties, @signals, @slots].each do |obj|
               obj.setp :'read-only'=>true
            end

            @objects.add_items Gui::Widgets
            @events.add_items Qt4::Native.event_list

            update_object_info @objects.getp(:'current-text')

            [
               [@objects, :update_object_info],
               [@enum_properties, :populate_enums]
            ].each do |widget, meth|
               widget.connect(:activated, method(meth))
            end

         end #group_box
      end

      def show
         window(:main, :size=>'800 600') {
             @main.set_central_widget gui_object
             add_gui_dialog @main
         }.show
      end

   end # class

   with_gui() {
      Browser.new
   }
