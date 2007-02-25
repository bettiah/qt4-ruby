require 'gui'
include Gui


class Mainwindow < View::Builder

    def show
        window(:main) {
            @main.add_status_bar
            @main.set_central_widget text_edit(:text)
            @main.set_menu_bar(menu_bar(:mbar) do
                @mbar.add_menu(menu(:menu1, :title=>'File'))
                @mbar.add_menu(menu(:menu2, :title=>'Help'))
            end)
            bar = tool_bar

            copy = action :text=>'copy', :icon_file=>"icons/copy.png", :shortcut=>"Ctrl+C", :status_tip=>"Copy to clipboard"
            cut = action :text=>'cut', :icon_file=>"icons/cut.png", :shortcut=>"Ctrl+X", :status_tip=>"Cut to clipboard"
            paste  = action :text=>'paste', :icon_file=>"icons/paste.png", :shortcut=>"Ctrl+V", :status_tip=>"Paste from clipboard"

            [copy, cut, paste].each  do |act|
                @menu1.add_action act
                bar.add_action act
                act.connect_qt(:triggered, @text, act.getp(:text))
            end

            @main.add_tool_bar bar
            abt = action :text=>'about'
            abt.connect(:triggered, lambda {message_box :info, "A <b>main-window</b> example."})
            @menu2.add_action abt
        }.show
    end
    
end

with_gui() {
   Mainwindow.new
}
