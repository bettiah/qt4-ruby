require 'gui'
include Gui

class Symbol
	def to_proc
		proc { |obj, *args| obj.send(self, *args) }
	end
end

class Baby < View::Builder
	def on_click
		sender{|obj| message_box(:info, obj.getp(:text))}
	end

	def show()
		push_button(:text=>"Click", :'minimum-width'=>150, :'minimum-height'=>30) {
			connect :clicked, method(:on_click)
		}.show
	end
end

with_gui() { View::Builder.new.label(:text=>"<h1>Hello Baby!!") }

with_gui() {
	Baby.new
}
