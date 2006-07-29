require "qt"

include Qt4

with_qt() { 
    btn = Widget(:'push-button')
    btn.setp({:text=>"Click", :'minimum-width'=>150, :'minimum-height'=>30 })
    btn.connect(:clicked) do
       text = btn.getp(:text)
       p "#{text}"
       message_box(:info, "#{text}")
    end
    btn.show
    }
