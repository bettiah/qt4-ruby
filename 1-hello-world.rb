require 'gui'

include Gui

class Baby < View::Builder
	def show()
		push_button(:push, :text=>"Click", :'minimum-width'=>150, :'minimum-height'=>30) {
			@push.connect :clicked, lambda {message_box(:info, "#{@push.getp(:text)}")}
        }.show
	end
end

with_gui() { View::Builder.new.label(:text=>"<h1>Hello Baby!!") }

with_gui() {
	Baby.new
}
