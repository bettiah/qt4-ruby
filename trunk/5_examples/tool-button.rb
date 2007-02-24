   def show_tool_button()
       widget do
	   tool_bar(:bar) {
	       @bar.add_widget tool_button(:text=>"Beside", 
					   :tool_button_style=>":text_beside_icon",
					   :icon_file=>"icons/icon.png")
	       @bar.add_widget tool_button(:text=>"Under", 
					   :tool_button_style=>":text_under_icon",
					   :icon_file=>"icons/icon.png")
	       @bar.add_widget tool_button(:icon_file=>"icons/icon.png")
	       @bar.add_widget tool_button(:text=>"Text Only", 
					   :checkable=>true, :checked=>true)
	   }
       end
   end

