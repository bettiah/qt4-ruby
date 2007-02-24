   def show_event_filter()
       @lb = label(:focus_policy=>':strong_focus+wheel_focus',:mouse_tracking=>true)
	   @lb.set_focus
	   [:key_press, :mouse_button_press, :mouse_move, :wheel, :resize].each { |ev|
			@lb.event_filter ev, method(:show_event)
	   }
	   @lb
   end
   
   def show_event(ev)
       @lb.setp :text=>("<b>%s</b>" % ev)
   end

