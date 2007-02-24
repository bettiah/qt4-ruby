   def show_table_widget()
       rows = 100
       cols = 3
       tab = table_widget(:row_count=>rows, :column_count=>cols, :alternating_row_colors=>true)
       rows.times {|r|
           cols.times{|c|
               tab.set_text r, c, "#{r+c}"
           }
       }
       tab.set_horizontal_header_labels "A", "B", "C"
       tab.set_text_color 0, 1, "tomato"
       tab.set_background_color 0, 1, "khaki"
       tab.set_icon_file 0, 1, 'icons/qt-logo.png'
       tab.set_text 0, 0, "left"
       tab.set_text 0, 1, "center"
       tab.set_font 0, 1, ":bold t"
       tab.set_text 0, 2, "right"
       tab.set_text_alignment 0, 1, ':align-center'
       tab.set_text_alignment 0, 2, ':align-right'

       tab.connect :item_value_changed, lambda {|r, c, txt|
           @main.show_status_message("Cell changed %s, (row %d), (col %d)" % [txt, r+1, c+1])
       }

       tab
   end


