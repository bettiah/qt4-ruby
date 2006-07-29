require "qt"

include Qt4

def get_kwd(widget)
    widget.getp(:'current-text').delete!(':').to_sym
end

$show_font_dialog = Proc.new {
    fnt = font_dialog(@selected_font.getp(:text))
    @selected_font.setp :text=>fnt
}

$show_color_dialog = Proc.new {
    color = color_dialog(@selected_color.getp(:text))
    @selected_color.setp :text=>color
}

$heavy_calculation = Proc.new {
    max = 50
    progress_dialog("Calculating", max) { |set_progress|
        p 'setting progress'
        catch :progressCancelledException do 
            (1..max).each { |n| sleep(0.1); set_progress.call(n+1) }
        end
        message_box(:warning, "The calculation has been canceled!")
    }
}

def gui_dialog
    main = Widget :'group-box'
    Widget :'push-button', nil, :message, :input, :file, :font, :color, :error, :progress
    Widget :'combo-box', nil, :type_message, :type_input, :type_file
    Widget :'line-edit', nil, :selected_font, :selected_color, :error_text
    layout = Widget :'grid-layout', main

    [
	    [@message,         0, 0],
	    [@type_message,    0, 1],
	    [@font,            0, 2],
	    [@selected_font,   0, 3],
	    [@input,           1, 0],
	    [@type_input,      1, 1],
	    [@color,           1, 2],
	    [@selected_color,  1, 3],
	    [@file,            2, 0],
	    [@type_file,       2, 1],
	    [@error,           2, 2],
	    [@error_text,      2, 3]
	].each do |a, b, c|
	   grid(layout, :widget, a, b, c)
	end

    grid layout, :widget, @progress, 3,1,1,2

    4.times { |n| layout.slotcall :'set-column-stretch', n, 1 }

    [
        [:title,    "Dialogs"],
	    [:flat,     true]
    ].each do |prop, val|
        main.setp prop=>val
    end

    [
        [@message,      "message-box"],
	    [@input,        "input-dialog"],
	    [@file,         "file-dialog"],
	    [@font,         "font-dialog"],
	    [@color,        "color-dialog"],
	    [@error,        "error-message"],
	    [@error_text,   "My error message."],
	    [@progress,     "with-progress-dialog"]
    ].each do |w, val|
        w.setp :text=>val
    end

    [
        [@type_message, [":info", ":warning", ":error", ":question"]],
        [@type_input,   [":int", ":float", ":text"]],
        [@type_file,    [":open-file", ":save-file", ":dir"]]
    ].each do |obj, rst| 
        obj.slotcall :'add-items', rst.join('^')
    end

    [
        [@message, lambda { message_box get_kwd(@type_message), "Message Box" }],
        [@input, lambda { input_dialog get_kwd(@type_input), "Please Enter" }],
        [@file, lambda { file_dialog get_kwd(@type_file), "Name" }],
        [@font, $show_font_dialog],
        [@color, $show_color_dialog],
        [@error, lambda { error_message(@error_text.getp(:text)) }],
        [@progress, $heavy_calculation]
    ].each do |obj, prc|
        obj.connect :clicked, &prc
    end

    main
end

def populate(widget, obj, type)
	lst = obj.access_list type
	str=[]
	lst.split('^').each do |item|
		if item.length == 0
			str << "<br>" 
		elsif
			if item =~ /\:$/
				str << "<b>#{item}</b><br>"
			elsif item =~ /\s/
				str << "#{$`} <font color=#4040ff>#{$'}</font><br>"
            else str << "#{item}<br>"
			end
		end
	end #do
    widget.setp :html=>str.join
end

$populate_enums = Proc.new { |name|
	@enums.clear
	@enums.slotcall:'add-items', ($temp_object.enum_list name)
	}

$update_object_info = Proc.new { |name|
	$temp_object.destroy if $temp_object 
	$temp_object = Widget(name)
	populate(@properties, $temp_object, :property)
	populate(@signals, $temp_object, :signal)
	populate(@slots, $temp_object, :slot)

	@enum_properties.clear
	@enum_properties.slotcall :'add-items', $temp_object.enum_property_list
	$populate_enums.call(@enum_properties.getp(:'current-text'))
	}

def gui_object()
    main = Widget(:'group-box')
	hbox = Widget(:'h-box-layout')
	Widget(:label, nil, :lb_enum, :lb_properties, :lb_read_only, :lb_signals, :lb_slots)
	Widget(:'text-edit', nil, :properties, :signals, :slots)
	Widget(:'combo-box', nil, :objects, :enum_properties, :enums)
	layout = Widget :'grid-layout', main
	
	[
		[:widget,  @objects],
		[:stretch, nil],
		[:widget,  @lb_enum],
		[:widget,  @enum_properties],
		[:widget,  @enums]
	].each do |type,obj|
		box(hbox, type, obj) 
	end
	grid layout, :layout, hbox, 0, 0, 1, 3

	[
		[@lb_properties, 1, 0, 1, 1],
		[@lb_signals,    1, 1, 1, 1],
		[@lb_slots,      1, 2, 1, 1],
		[@properties,  2, 0, 1, 1],
		[@signals,     2, 1, 2, 1],
		[@slots,       2, 2, 2, 1],
		[@lb_read_only, 3, 0, 1, 1]
	].each do |a, b, c, d, e|
	   grid(layout, :widget, a, b, c, d, e)
	end

	[
		[:title, "Objects"],
		[:flat, true]
	].each do |prop, value|
		main.setp prop=>value
	end

	[
		[@lb_enum, "Enum properties"],
		[@lb_properties, <<-EOT
					<b><font color=#ff4020>Properties</font></b><br>
                    object.<b>getp</b> name <font color=#4040ff>:type</font><br>
					object.<b>setp</b> name value
					EOT
		],
		[@lb_signals, <<-EOT
			<b><font color=#ff4020>Signals</font></b><br>
			<div align=right>from-object.<b>connect</b> signal<br>
			from-object.<b>connect</b> signal</div>
			EOT
		],
		[@lb_slots, <<-EOT
			<b><font color=#ff4020>Slots</font></b><br>
			to-object slot<br>
			lisp-slot 
			EOT
		],
		[@lb_read_only, "[r] ... read only"]
	].each do |obj, value|
		obj.setp :text=>value
	end

	[@enum_properties, @enums].each do |obj|
		#setp obj, :'size-adjust-policy'=>:'adjust-to-contents'
		obj.setp :'size-adjust-policy'=>:'adjust-to-contents'
	end

	[@properties, @signals, @slots].each do |obj|
		 obj.setp :'read-only'=>true
	end

	@objects.slotcall :'add-items', object_list
    
    $temp_object = nil    
	$update_object_info.call( @objects.getp(:'current-text') )

	[
		[@objects, $update_object_info],
		[@enum_properties, $populate_enums]
	].each do |widget, proc|
		widget.connect(:'activated', &proc)
	end

	main
end

with_qt() {
    obj = gui_object
    dlg = gui_dialog

    main = Widget :widget
    layout = Widget :'v-box-layout', main
    box layout, :widget, dlg
    box layout, :widget, obj

    main.setp :size=>[750, 550]
    main.show
}
