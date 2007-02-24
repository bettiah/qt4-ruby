   def show_graphics_view()
       widget do
           v_box_layout {
   			   box push_button(:action, :text=>"Action!")
   			   box graphics_view(:view, :drag_mode=>':scroll-hand-drag') {
				   graphics_line_item :line, :line=>'(0 0 0 100)', :pos=>'(0 0)'
				   graphics_rect_item :rect, :rect=>'(0 0 100 100)', :pos=>'(10 0)'
				   graphics_ellipse_item :ellipse, :rect=>'(0 0 100 100)', :pos=>'(120 0)'
				   graphics_polygon_item :polygon, :polygon=>'(0 0 100 100 0 100)', :pos=>'(230 0)'
				   graphics_text_item :html, :html=>"<h3>Items</h3><ul><li>one<li>two<li>three",
                         :pos=>'(340 0)'
	  			   graphics_pixmap_item :pixmap, :pixmap_file=>"icons/onlisp.png", :pos=>'(440 20)'
			   }
		   }
		   view_items.each {|item|
			   item.setp :selectable=>true, :movable=>true
		   }
		   @action.connect :clicked, method(:do_action)
       end
   end
  
   def view_items
	   [@line, @rect, @ellipse, @polygon, @html, @pixmap]
   end

   def do_action
	   view_items.each { |item|
		   item.setp :pen=>":color (#{rnd_color}) :width 2", 
			   :brush=>":color (#{rnd_color}) \"light\""
   		   item.move_by rnd_num, rnd_num
		   item.rotate rnd_num(5)
		   item.scale rnd_factor, rnd_factor
	   }
   end

   def rnd_color
	   ["coral", "green", "steelblue"].at rand(3)
   end

   def rnd_num(f=1)
	   f * 2-rand(5)
   end

   def rnd_factor
	   (2/10 * (rand 2)) - 11/10
   end

