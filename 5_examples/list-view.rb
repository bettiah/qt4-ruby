   def show_list_view()
       widget do
		   l = ["Apples", "Oranges", "Bananas"]
           li = list_view
           li.set_as_model l
		   li.model.set_observer Ob.new(l)
           h_box_layout {
	        box li
                box
	   }
       end
   end

   class Ob
	   def initialize(mod)
		   @mod = mod
	   end

	   def update_view(row, col)
		   p "updated to #{@mod.inspect} row #{row} column #{col}"
	   end
   end
