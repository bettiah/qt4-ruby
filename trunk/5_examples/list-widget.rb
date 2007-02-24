   def show_list_widget()
       widget do
           li = list_widget
           li.add_items("Apples", "Oranges", "Bananas")
           li.connect :current_text_changed, lambda {|txt| message_box :info, 'got '+txt}
           h_box_layout {
	        box li
                box
	   }
       end
   end
  
