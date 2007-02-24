   def show_push_button()
       widget do
	   v_box_layout {
	       box_layout h_box_layout {
		       box make_button(:left)
       		       box
    		   }
	   	box_layout h_box_layout {
	       	       box
		       box make_button(:center)
       		       box
    		   }
       	   	box_layout h_box_layout {
	       	       box
		       box make_button(:right)
    		   }
	   }
       end
   end

   def make_button(sym)
       btn = push_button(:text=>sym.to_s, :'minimum-width'=>150, :'minimum-height'=>30)
       btn.connect :clicked, lambda { message_box(:info, sender.getp(:text)) }
       btn
   end

