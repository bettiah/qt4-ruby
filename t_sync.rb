require 'net/protocol'
require 'obex'

include Net

begin
p "opening connection to ..."
trans = TCPSocket.open('localhost', 650) 
p "opened"

File.open("put.txt") {|output|
	File.open("get.txt", "w") {|write|
		o = Obex.new trans 
		o.set_debug_output $stderr
		o.start(:target=>"SYNCML-SYNC") {|session|
			res = session.put({:type=>"application/vnd.syncml-xml", 
				:length=>File.size(output), 
				:connection_id=>session.conn_id}, output)
			sleep 1
			session.get({:connection_id=>session.conn_id,
						:type=>"application/vnd.syncml-xml"}, write) if Net::ObexOK === res 
		}
	}
}
rescue 
p "#{$!}"
end



p "done"
