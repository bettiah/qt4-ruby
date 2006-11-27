require 'gui'
include Gui

class Symbol
   def to_proc
      proc { |obj, *args| obj.send(self, *args) }
   end
end

class Btn < PushButton
   def on_click
      message_box(:info, sender.getp(:text))
   end

   def initialize()
	   super
	   self.setp(:text=>"Click", :'minimum-width'=>150, :'minimum-height'=>30)
 	   self.connect :clicked, :on_click
   end
end

with_gui() { Label.new(:text=>"<h1>Hello Baby!!") }

with_gui() {
   Btn.new
}
