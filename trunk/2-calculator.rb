require 'gui'
include Gui

class Calculator < View::Builder

   $value1, $value2, $operation, $reset, $decimals = 0, nil, nil, false, nil

   def clear_display
      $value1 , $decimals = 0, nil
      display_number 0
   end

   def display_number(n)
      @float.setp(:text=>n.to_f.to_s)
      txt = "%s %s %s" % [$value2.to_f.to_s, $operation, ($reset==true ? nil : $value1).to_f.to_s]
      @real.setp(:text=>"#{txt}")
   end

   def number_clicked
      if $reset
         clear_display
         $reset = false
      end
      clicked = nil
      clicked = sender.getp :text
      $value1 = $decimals \
      ? ($value1 + clicked.to_i * (10 ** -$decimals+=1)) \
      : (clicked.to_i + 10*$value1)
      display_number $value1
   end

   def back_clicked
      $value1 = if $decimals
      then n = (10 ** $decimals-=1)
         ((n * $value1)/n).to_i
      else ($value1/10).to_i
      end
      display_number $value1
   end

   def sign_clicked
      $value1=(-$value1)
      display_number $value1
   end

   def reci_clicked
      $value1 = 1/$value1.to_f
      display_number $value1
   end

   def point_clicked
      $decimals = 0 unless $decimals
   end

   def clear_clicked
      $value2 = nil
      clear_display
   end

   def operate
      res = $value2.send($operation.to_sym, $value1)
      p "result is #{res}"
      if res
         $value2 = res
         display_number $value2
      end
   end

   def operation_clicked
      $operation = sender.getp :text 
      $value2 ? operate : $value2 = $value1
      p "#{$operation}"
      $reset = true
   end

   def equal_clicked
      if $value2
         operate
         $value1 = $value2
         $value2 = nil
         $reset = true
         $operation = nil
      end
   end

   #GUI
   def do_button(name, oper)
      but = tool_button(:'minimum-width'=>35, :'minimum-height'=>25, 'size-policy'=>':expanding', :text=>name)

      case oper
      when Symbol
         but.connect :clicked, method(oper)
      when Proc, Method
         but.connect :clicked, oper
      end

      but.setp :shortcut=>case name
  		 when "=" then "Return"
	  	 when "<<" then "Backspace"
	  	 when "AC" then "Delete"
	  	 when "blah" then "B"
	  	 when "1/x" then "R"
	  	 when "+-" then "S"
	  	 else name
	  	 end
	  but
	end

def show
   widget {
      grid_layout do
         row {
            item label(:real, :'frame-shape'=>:box, :alignment=>:'align-right'), :row_span=>1, :col_span=>5
         }
         row {
            item line_edit(:float, :'read-only'=>true, :font=>14, :alignment=>:'align-right'), :row_span=>1, :col_span=>5
         }
         row {
            item do_button("1/x", :reci_clicked)
            item do_button("/", :operation_clicked)
            item do_button("*", :operation_clicked)
            item do_button("-", :operation_clicked)
            item do_button("AC", :clear_clicked)
         }
         row {
            item do_button('1', :number_clicked)
            item do_button('2', :number_clicked)
            item do_button('3', :number_clicked)
            item do_button("+", :operation_clicked), :row_span=>2
            item do_button("<<", :back_clicked)
         }
         row {
            item do_button('4', :number_clicked)
            item do_button('5', :number_clicked)
            item do_button('6', :number_clicked)
            item
            item do_button("=", :equal_clicked), :row_span=>2
         }
         row {
            item do_button('7', :number_clicked)
            item do_button('8', :number_clicked)
            item do_button('9', :number_clicked)
            item do_button("+-", :sign_clicked)
         }
         row {
            item do_button('0', :number_clicked), :row_span=>1, :col_span=>3
            item do_button(".", :point_clicked)
            item do_button("blah", lambda {message_box(:info, "#{$value1.to_f.to_s}")})
         }
      end
      clear_display
   }.show
end

end #class

with_gui() {
	Calculator.new
}
