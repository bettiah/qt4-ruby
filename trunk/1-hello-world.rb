require 'gui'
include Gui

class Btn < PushButton
   def on_click
      message_box(:info, sender.getp(:text))
   end

   def initialize()
	   super(:text=>"Click", :'minimum-width'=>150, :'minimum-height'=>30) { |bt|
 	   		bt.connect :clicked, :on_click
	   }
   end
end

with_gui() { Label.new(:text=>"<h1>Hello Baby!!") }

with_gui() {
   Btn.new
}
