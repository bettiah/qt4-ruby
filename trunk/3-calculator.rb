require "qt"

include Qt4

$value1, $value2, $operation, $reset, $decimals = 0, nil, nil, nil, nil

def clear_display
  $value1 , $decimals = 0, nil
  display_number 0
end

def error_to_string(err)
  "#<#{err.to_s} >" 
end

def display_number(n)
	$float.setp(:text=>n.to_f.to_s)
	$real.setp(:text=>"kjhkjh")
end

words_clicked = Proc.new {
	message_box(:info, "#{$value1.to_f.to_s}")
	}

number_clicked = Proc.new {
	if $reset
		clear_display
		$reset = nil
	end
	clicked = sender.getp :text
	p "clicked #{clicked}"
	$value1 = $decimals ? ($value1 + clicked.to_i * (10 ** -$decimals+=1)) : (clicked.to_i + 10*$value1)
	display_number $value1
	}

back_clicked = Proc.new {
	$value1 = if $decimals 
				 then n = (10 ** $decimals-=1)
					   ((n * $value1)/n).to_i
				 else ($value1/10).to_i
				 end
	display_number $value1
	}

sign_clicked = Proc.new {
	$value1=(-$value1)
	display_number $value1
}

reci_clicked = Proc.new {
	$value1 = 1/$value1.to_f
	display_number $value1
}

point_clicked = Proc.new {
	$decimals = 0 unless $decimals
	}

clear_clicked = Proc.new {
	$value2 = nil
	clear_display
	}

def operate()
	res = $value2.send($operation.to_sym, $value1)
	p "result is #{res}"
	if res 
		$value2 = res
		display_number $value2
	end
end

operation_clicked = Proc.new {
	$operation = sender.getp :text
	$value2 ? operate : $value2 = $value1
	p "#{$operation}"
	$reset = 1
	}

equal_clicked = Proc.new {
	if $value2 
	   operate
	   $value1 = $value2
	   $value2 = nil
	   $reset = 1
	end
	}

#GUI
with_qt() {
    li=[]
	[:widget, :'line-edit', :label].each {|name| li.push Widget(name)}
    11.times do li.push(Widget(:'tool-button')) end 
	main, $float, $real, plus, minus, multiply, divide, reci, sign, point, clear, back, words, equal = li

    p "#{minus.class}"
    li.clear

    10.times do li.push(Widget(:'tool-button')) end
	zero, one, two, three, four, five, six, seven, eight, nine = li
	layout = Widget(:'grid-layout', main)

	[
	    [$real, 0, 0, 1, 5],
	    [$float, 1, 0, 1, 5],
	    [reci, 2, 0, 1, 1],
	    [divide, 2, 1, 1, 1],
	    [multiply, 2, 2, 1, 1],
	    [minus, 2, 3, 1, 1],
	    [clear, 2, 4, 1, 1],
	    [plus, 3, 3, 2, 1],
	    [back, 3, 4, 1, 1],
	    [words, 4, 4, 1, 1],
	    [sign, 5, 3, 1, 1],
	    [equal, 5, 4, 2, 1],
	    [zero, 6, 0, 1, 3],
	    [point, 6, 3, 1, 1]
	].each do |a, b, c, d, e|
		grid(layout, :widget, a, b, c, d, e)
	end

	n=0
	3.times do |r| 
		3.times do |c|
		grid(layout, :widget, li[n+=1], 5-r, c)
		end
	end

	[
		[plus, "+"],
		[minus, "-"],
		[multiply, "*"],
		[divide, "/"],
		[reci, "1/x"],
		[sign, "+-"],
		[point, "."],
		[clear, "AC"],
		[back, "<<"],
		[words, "blah"],
		[equal, "="]
	].each do |w, n|
		w.setp(:text=>n)
	end

	$real.setp(:'frame-shape'=>:box)
	$float.setp(:'read-only'=>true)
	$float.set_font(14)
	[$float, $real].each do |w| w.setp(:alignment=>:'align-right') end

	[plus ,minus ,multiply ,divide ,reci ,sign ,point ,clear ,back ,words ,equal].each { |btn|
		btn.setp(:'minimum-width'=>35, :'minimum-height'=>25)
		btn.allow_expanding
	}
	10.times do |n|
		btn = li[n]
		btn.setp(:text=>n.to_s, :'maximum-width'=>35, :'minimum-height'=>25)
		btn.allow_expanding
		btn.connect(:clicked, &number_clicked)
	end

	[plus, minus, multiply, divide].each do |bt|
		bt.connect(:clicked, &operation_clicked)
	end

	{
		clear	=>	clear_clicked, 
		back	=>	back_clicked, 
		sign	=>	sign_clicked,
		point	=>	point_clicked,
		reci	=>	reci_clicked,
		words	=>	words_clicked,
		equal	=>	equal_clicked
	}.each do |bt, act|
		bt.connect(:clicked, &act)
	end

    clear_display
    main.show
}

