
class Clock < PainterWidget
    def initialize(parent)
        super(parent)
        set_fixed_size '(200 200)'
        event_filter :resize, method(:show_clock)
        event_filter :timer, method(:repaint_clock)
        event_filter :paint, method(:draw_clock)
  
        start_timer(1000) 
    end

    def show_clock(ev)
        p 'resize'
        translate(0, 0)
        w = getp(:width)
        h = getp(:height)
        side = w > h ? h : w
        
        translate(w/2, h/2)
        scale(side/200.0, side/200.0)
    end

    def repaint_clock(ev)
        clear
    end

    def draw_clock(ev)
        hour_hand = "7 8 -7 8 0 -40"
        minute_hand = "7 8 -7 8 0 -70"
        second_hand = "2 8 -2 8 0 -90"
        
        time = Time.now       

        12.times {
            draw_line '88 0 96 0'
            rotate 30
        }

        60.times { |n|
            draw_line '92 0 96 0' unless n%5 == 0
            rotate 6
        }
        
        save
        set_pen ':style :no-pen'
        set_brush ':color (green)'
        rotate(30.0 * ((time.hour + time.min / 60.0)))
        draw_polygon hour_hand
        restore

        save
        set_pen ':style :no-pen'
        set_brush ':color (steelblue) :alpha 191'
        rotate(6.0 * (time.min + time.sec / 60.0))
        draw_polygon minute_hand
        restore
        
        save
        set_pen ':style :no-pen'
        set_brush ':color (red)'
        rotate(6.0 * time.sec)
        draw_polygon second_hand
        restore
    end
end

def show_painter_widget
    Clock.new @parent
end
