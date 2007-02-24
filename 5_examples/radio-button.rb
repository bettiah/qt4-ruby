   def show_radio_button()
       widget do
	   group_box(:title=>'123') {
	       v_box_layout {
		       box radio_button(:text=>'one')
		       box radio_button(:text=>'two')
		       box radio_button(:text=>'three')
	   	}
	   }
       end
   end

