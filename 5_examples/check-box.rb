   def show_check_box()
       widget do
	   group_box(:title=>'Order') {
	       v_box_layout {
		       box make_button(:Pizza)
		       box make_button(:Coke)
		       box make_button(:Cake)
	   	}
	   }
       end
   end
   
   def make_button(sym)
       ch = check_box(:text=>sym.to_s)
       ch.connect :clicked, lambda { message_box :info, sender.getp(:text) }
       ch
   end

