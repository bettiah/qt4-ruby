require 'socket'
require 'obex'

include Net

class ObexPush < ObexServer
	def initialise
	end

	def do_get(req)
		@file = @file || File.open('put.txt')
		ObexContinue.new @file
	end

	def do_put(req)
		if req.final
			ObexOK.new
		else
			p "req not final"
			ObexContinue.new
		end
	end
end

port = 650

if ARGV[0]

p "opening connection .."
trans = TCPSocket.open('localhost', port)
p "opened"

o = Obex.new trans 
o.set_debug_output $stderr
o.start(:target=>"SYNCML-SYNC") {|session|
	p "max size is #{session.max_packet_size}"
	File.open("put.txt") {|file|
		res = session.put({:type=>"application/vnd.syncml-xml", 
				:length=>File.size(file), 
				:connection_id=>session.conn_id}, file)
		session.get(nil) {|d|}
	}

}

else

server = TCPServer.new('localhost', port)
while (session = server.accept)
	s = ObexPush.new session
	s.set_debug_output $stderr
	s.start
end

end
p "done"
