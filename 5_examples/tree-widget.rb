   def show_tree_widget()
       tr = tree_widget(:column_count=>2, :alternating_row_colors=>true)
       tr.set_header_labels "Name", "Depth"
       tr.add_item 0, "parent (with icon)"
       tr.set_icon_file 0, "icons/qt-logo.png"
       tr.add_items 1, "child" , '1'
       tr.add_items 2, "grandchild", '2'
       tr.add_items 0, "parent", 0
       tr.add_items 1, "child", 1
       tr.add_items 1, "sibling", 1
       tr.expand_all
       tr
   end

