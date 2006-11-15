require 'net/protocol'
require 'iconv'

module Net
    
	Revision = %q$Revision: 0.1 $.split[1]
    ObexVersion = '1.1'
	MAX_SIZE = 0x400

  class ObexBadResponse < StandardError; end
  class ObexHeaderSyntaxError < StandardError; end

  class Obex < Protocol

    def initialize(sock)
      @curr_obex_version = ObexVersion
      @keep_alive = false
      @started = false
  	  @socket = BufferedIO.new(sock)
  	  @socket.read_timeout = @read_timeout
      @debug_output = nil
	  @max_packet_size = MAX_SIZE
	  @conn_id = 0
    end

    def inspect
      "#<#{self.class} #{@socket} open=#{started?}>"
    end

    def set_debug_output(output)
      warn 'Net::Obex#set_debug_output called after Obex started' if started?
      @debug_output = output
	  @socket.debug_output = @debug_output
    end

    def started?
      @started
    end

    attr_accessor :keep_alive
    attr_reader   :conn_id
	attr_reader   :max_packet_size
	attr_reader   :flags

    def start(initheader = nil)
  		raise IOError, 'Obex session already opened' if @started
  		if block_given?
	  		begin
	  			do_start(initheader)
	  			yield(self)
			rescue Exception => ex
				D "#{ex.class}: #{ex.message}\n\t#{ex.backtrace[0]}"
			ensure
	  			finish
			end
  		else 
  			do_start(initheader)
  		end
  		self
	end

    def do_start(initheader = nil)
	  begin
		  D "Connecting to #{@socket}"
		  req = Connect.new initheader
		  res = request req
  		  if ObexOK === res
			@conn_id = res[:connection_id] || @conn_id
      		@started = true
  		  else 			  
			raise IOError
  		  end
		  res
	  rescue
		  D 'Failed Starting obex session'
		  raise 
	  end
    end
    private :do_start

    def finish
	  begin
		  request(Disconnect.new()) if @started
	  ensure
		  @started = false
	      do_finish
	  end
    end

    def do_finish
      @socket.close if @socket and not @socket.closed?
      @socket = nil
    end
    private :do_finish

    public
	
	def get(initheader, dest = nil, &block)
	  do_start unless started?
      raise IOError, 'Obex session not yet started' unless started?
      res = nil
      request(Get.new(initheader)) {|r|
        r.read_body dest, &block
        res = r
      }
      res
    end

    def put(initheader, source = nil)
	  do_start unless started?
      raise IOError, 'Obex session not yet started' unless started?
	  pt = Put.new(initheader)
	  pt.instance_variable_set(:@max_packet_size, @max_packet_size)
	  request(pt, source)
    end

    def request(req, src_dest = nil)
	  res = nil
        begin
  			D "Sending request #{req.inspect} to #{@socket}"
			req.exec @socket, src_dest
			res = ObexResponse.read_new(@socket)
			res.method = req.method
			check_response res, req
			res.read_header @socket
  			D "Received Response #{res.inspect}"
			yield res if block_given? 
        end while ObexContinue === res
      res
    end

    private

	def check_response(res, req)
		if Connect === req
			@curr_obex_version, @flags, max  = res.read_connect(@socket)
			@max_packet_size = max < MAX_SIZE ? max : MAX_SIZE
		end
	end
	
    def D(msg)
      return unless @debug_output
      @debug_output << msg
      @debug_output << "\n"
    end

  end # Obex

  ObexSession = Obex

  class ObexServer
	  def initialize(sock)
		  @socket = BufferedIO.new(sock)
		  @started = false
	  end

  	  def set_debug_output(output)
		  @debug_output = output
		  @socket.debug_output = @debug_output
  	  end

	  def start
		  raise IOError, 'Obex session already opened' if @started
		  begin
			  run
		  end while @started
		  @socket.close
	  end
	 
	  def do_connect(req)
  		  @started = true
		  ObexOK.new
	  end

	  def do_disconnect(req)
  		  @started = false
		  ObexOK.new
	  end
   
	  private
	  def run
		  begin
			  req = ObexRequest.read_new @socket
			  check_request req
			  req.read_header @socket
			  D "handling request #{req.inspect} from #{@socket}"
			  service req
		  rescue ObexError => ex
			  res = ex.response
		  rescue ObexFatalError => ex
			  D "#{ex.class}: #{ex.message}"
			  res = ex.response
		  rescue EOFError => ex
			  D "#{ex.class}: #{ex.message}\n\t#{ex.backtrace[0]}"
			  @started = false
			  break
		  rescue StandardError => ex
			  D "#{ex.class}: #{ex.message}\n\t#{ex.backtrace[0]}"
			  res = ObexInternalServerError.new
			  @started = false
		  ensure
			  res.method = req.method
			  res.final = req.final
  			  D "Sending Response #{res.inspect}"
	  		  res.instance_variable_set(:@max_packet_size, @max_packet_size)
			  res.exec(@socket)
		  end
	  end

	  def service(req)
        method_name = "do_" + req.method.to_s.split(/::/).last.downcase
        if respond_to?(method_name)
  			res = __send__(method_name, req)
			if res
				raise res.error!
			else
				raise ObexInternalServerError.new.error!
			end
        else
          ObexNotImplemented.new.error! "unsupported method `#{method_name}'."
        end
	  end

	  def check_request(req)
		  if Connect == req.class
			  @curr_obex_version, @flags, max  = req.read_connect(@socket) 
			  @max_packet_size = max < MAX_SIZE ? max : MAX_SIZE
		  end
	  end

	  def D(msg)
		  return unless @debug_output
		  @debug_output << msg
		  @debug_output << "\n"
  	  end
  end

  module ObexHeader
    Headers = {
        :connection_id=>0xCB,
        :count=>0xC0,
        :name=>0x01,
        :type=>0x42,
        :length=>0xC3,
        :timestamp=>0x44,
        :timestamp=>0xC4,
        :description=>0x05,
        :target=>0x46,
        :http=>0x47,
        :who=>0x4A,
        :parameters=>0x4C,
        :auth_challenge=>0x4D,
        :auth_response=>0x4E,
        :object_class=>0x4F,
        :creator_id=>0xCF,
        :wan_uuid=>0x50,
        #:object_class=>0x51,
        :session_parameters=>0x52,
        :session_sequence_number=>0x93,
        :body=>0x48,
        :eob=>0x49
    }

	def initialize_obex_header(initheader)
      @header = {}
      return unless initheader
      initheader.each do |key, value|
        warn "net/obex: warning: duplicated Obex header: #{key}" if key?(key) and $VERBOSE
        @header[key] = value
      end
    end

    def size
      @header.size
    end

    def [](key)
      @header[key]
    end

    def []=(key, val)
      @header[key] = val
    end

    def fetch(key, *args, &block)
      @header.fetch(key, *args, &block)
    end

    def each_header(&block)
      @header.each(&block)
    end

    alias each each_header

    def each_key(&block) 
      @header.each_key(&block)
    end

    def each_value(&block)
      @header.each_value(&block)
    end

    def delete(key)
      @header.delete(key)
    end

    def key?(key)
      @header.key?(key)
    end

    def to_hash
      @header.dup
    end

	def sorted
		@header.sort {|a, b|
		  if a[0] == :connection_id
			  -1
		  elsif b[0] == :connection_id
			  1
		  elsif (a[0] == :body) || (a[0] == :eob)
			  1
		  elsif (b[0] == :body) || (b[0] == :eob)
			  -1
		  else 
			  0
		  end
  		}
	end

    def encoded_each
	  sorted.each {|key, value|
  		  yield coded(key, value)
	  }
    end

    def coded( k, data )
	  hi = Headers[k]
      case hi >> 6
        when 0
            package = [hi, data.length*2+3, data+"0"]
            return package.pack("Cna*")
        when 1
            package = [hi, data.length+3, data]
            return package.pack("Cna*")
        when 2
            package = [hi, data]
            return package.pack("CC")
        when 3
            package = [hi, data]
            return package.pack("Ci")
        end #case
    end
    private :coded

	def decode(data)
        while true
            hval, length, quant = parse_head(data)
            @header[Headers.index(hval)] = quant
            data.slice!(0..length-1)
            break if data.length == 0
        end
    end

    def parse_head(data)
        case data.unpack("C")[0] >> 6
        when 0, 1
            byte, length = data.unpack("Cn")
            quantity = data[3,length-3].unpack("a*")[0]
        when 2
            byte, quantity = data.unpack("CC")
            length = 2
        when 3
            byte, quantity = data.unpack("CI")
            length = 5
        end
        return [byte, length, quantity]
    end
	private:parse_head

    def content_length
      @header[:length] or nil
    end

  end #ObexHeader

  module ObexUtils
    
    def f_connect(header)
        # "v1, no flags, max"
        fop([0x10, 0x00, MAX_SIZE, header].pack("CCna*"))
    end

	def read_connect(sock)
		raise IOError, "wrong connect req of length #{@message_length}" if @message_length < 4
		@message_length -= 4
        sock.read(4).unpack('CCn')
 	end

	def read_header(sock)
		decode sock.read(@message_length) if @message_length > 0
	end

    def send_(sock)
      header = ""
      encoded_each do |h|
        header << h
      end
	  sock.write(Connect == @method ? f_connect(header) : fop(header))
    end

    def read_body(dest = nil, &block)
      if @read
        raise IOError, "#{self.class}\#read_body called twice" if dest or block
        return @body
      end
      to = procdest(dest, block)
      read_body_0 to
      @body = to
      @read = true
      @body
    end

    private

    def procdest(dest, block)
      raise ArgumentError, 'both arg and block given for HTTP method' \
          if dest and block
      if block
        ReadAdapter.new(block)
      else
        dest || ''
      end
    end

	  def read_body_0(proc_dest)
		  data = @header[:eob]
		  if data
			  @final = true
		  else data = @header[:data]
		  end
		  proc_dest << data
	  end

  end #ObexUtils

	class ObexGenericRequest
		include ObexHeader
		include ObexUtils
	end

	class ObexRequest < ObexGenericRequest
		def initialize(initheader = nil, len = 0)
	  		super len, initheader
		end
  	end

	class Connect < ObexRequest
	end

	class Disconnect < ObexRequest
	end

	class Get < ObexRequest
    end

    class Put < ObexRequest
    end

	class Chdir < ObexRequest
	end

	class Abort < ObexRequest
	end
	
	class ObexUnknownRequest < ObexRequest
	end


	class ObexGenericRequest
	  Opcodes = {
		  0x80 => Connect,
 		  0x81 => Disconnect,
		  0x02 => Put,
		  0x03 => Get,
		  0x82 => Put,
		  0x83 => Get,
  		  0x85 => Chdir,
  		  0xff => Abort
	  }
	  
    class << self
		def read_new(sock)
			m, len = read_req(sock)
			req = request_class(m).new nil, len-3
			req.final = is_final?(m)
			p "Request Final? #{req.final}"
			req
		end

		private
		
		def request_class(code)
			Opcodes[code] or ObexUnknownRequest
		end

		def read_req(sock)
			sock.read(3).unpack('Cn')
		end
	
  		def is_final?(m)
  			(m & 0x80) > 0 ? true : false
  		end
	end
	
    def initialize(message_length, initheader = nil)
		@method = self.class
  		@message_length = message_length
		@final = true
  		initialize_obex_header initheader
    end

    attr_reader :method
    attr_reader :message_length
	attr_reader :header
	attr_accessor :final

    def inspect
      "\#<#{self.class} final=>#{@final} >" 
    end

    def fop(data)
		code = Opcodes.index(@method)
		p "Final? #{@final}"
		if @final
			code = code | 0x80 
		else
			code = code & 0x7f
		end	
        [code, data.length+3, data].pack("Cna*")
    end

    def exec(sock, src_dest=nil) 
      if src_dest
        send_request_with_body sock, src_dest
      else
        send_ sock
      end
	  @header.clear
    end

	private

    def send_request_with_body(sock, src)
      data = src.read @max_packet_size
	  if data.length < @max_packet_size
		  @final = true
		  @header[:eob] = data
	  else 
		  @final = false
		  @header[:body] = data
	  end
      send_ sock
    end
  end #ObexGenericRequest

  # Obex exception class.
  # You must use its subclasses.
  module ObexExceptions
    def initialize(msg, res)   #:nodoc:
      super msg
      @response = res
    end
    attr_reader :response
    alias data response    #:nodoc: obsolete
  end
  class ObexError < ProtocolError
    include ObexExceptions
  end
  class ObexRetriableError < ProtoRetriableError
    include ObexExceptions
  end
  class ObexServerException < ProtoServerError
    # We cannot use the name "ObexServerError", it is the name of the response.
    include ObexExceptions
  end
  class ObexFatalError < ProtoFatalError
    include ObexExceptions
  end

  class ObexResponse		#redefined later
    def ObexResponse.exception_type  
      self::EXCEPTION_TYPE
    end
  end  

  class ObexUnknownResponse < ObexResponse
    EXCEPTION_TYPE = ObexError
  end
  class ObexInformation < ObexResponse           # 1xx
    EXCEPTION_TYPE = ObexError
  end
  class ObexSuccess < ObexResponse               # 2xx
    EXCEPTION_TYPE = ObexError
  end
  class ObexRedirection < ObexResponse           # 3xx
    EXCEPTION_TYPE = ObexRetriableError
  end
  class ObexClientError < ObexResponse           # 4xx
    EXCEPTION_TYPE = ObexServerException 
  end
  class ObexServerError < ObexResponse           # 5xx
    EXCEPTION_TYPE = ObexFatalError    
  end

  class ObexContinue < ObexInformation           # 100
  end
  
  class ObexOK < ObexSuccess                            # 200
  end
  class ObexCreated < ObexSuccess                       # 201
  end
  class ObexAccepted < ObexSuccess                      # 202
  end
  class ObexNonAuthoritativeInformation < ObexSuccess   # 203
  end
  class ObexNoContent < ObexSuccess                     # 204
  end
  class ObexResetContent < ObexSuccess                  # 205
  end
  class ObexPartialContent < ObexSuccess                # 206
  end

  class ObexMultipleChoice < ObexRedirection     # 300
  end
  class ObexMovedPermanently < ObexRedirection   # 301
  end
  class ObexFound < ObexRedirection              # 302
  end
  ObexMovedTemporarily = ObexFound
  class ObexSeeOther < ObexRedirection           # 303
  end
  class ObexNotModified < ObexRedirection        # 304
  end
  class ObexUseProxy < ObexRedirection           # 305
  end

  class ObexBadRequest < ObexClientError                    # 400
  end
  class ObexUnauthorized < ObexClientError                  # 401
  end
  class ObexPaymentRequired < ObexClientError               # 401
  end
  class ObexForbidden < ObexClientError                     # 403
  end
  class ObexNotFound < ObexClientError                      # 404
  end
  class ObexMethodNotAllowed < ObexClientError              # 405
  end
  class ObexNotAcceptable < ObexClientError                 # 406
  end
  class ObexProxyAuthenticationRequired < ObexClientError   # 407
  end
  class ObexRequestTimeOut < ObexClientError                # 408
  end
  class ObexConflict < ObexClientError                      # 409
  end
  class ObexGone < ObexClientError                          # 410
  end
  class ObexLengthRequired < ObexClientError                # 411
  end
  class ObexPreconditionFailed < ObexClientError            # 412
  end
  class ObexRequestEntityTooLarge < ObexClientError         # 413
  end
  class ObexRequestURITooLong < ObexClientError             # 414
  end
  ObexRequestURITooLarge = ObexRequestURITooLong
  class ObexUnsupportedMediaType < ObexClientError          # 415
  end

  class ObexInternalServerError < ObexServerError   # 500
  end
  class ObexNotImplemented < ObexServerError        # 501
  end
  class ObexBadGateway < ObexServerError            # 502
  end
  class ObexServiceUnavailable < ObexServerError    # 503
  end
  class ObexGatewayTimeOut < ObexServerError        # 504
  end
  class ObexVersionNotSupported < ObexServerError   # 505
  end


  class ObexResponse   # redefine
    CODE_CLASS_TO_OBJ = {
      1 => ObexInformation,
      2 => ObexSuccess,
      3 => ObexRedirection,
      4 => ObexClientError,
      5 => ObexServerError
    }
    CODE_TO_OBJ = {
      100 => ObexContinue,

      200 => ObexOK,
      201 => ObexCreated,
      202 => ObexAccepted,
      203 => ObexNonAuthoritativeInformation,
      204 => ObexNoContent,
      205 => ObexResetContent,
      206 => ObexPartialContent,

      300 => ObexMultipleChoice,
      301 => ObexMovedPermanently,
      302 => ObexFound,
      303 => ObexSeeOther,
      304 => ObexNotModified,
      305 => ObexUseProxy,

      400 => ObexBadRequest,
      401 => ObexUnauthorized,
      402 => ObexPaymentRequired,
      403 => ObexForbidden,
      404 => ObexNotFound,
      405 => ObexMethodNotAllowed,
      406 => ObexNotAcceptable,
      407 => ObexProxyAuthenticationRequired,
      408 => ObexRequestTimeOut,
      409 => ObexConflict,
      410 => ObexGone,
      411 => ObexLengthRequired,
      412 => ObexPreconditionFailed,
      413 => ObexRequestEntityTooLarge,
      414 => ObexRequestURITooLong,
      415 => ObexUnsupportedMediaType,

      500 => ObexInternalServerError,
      501 => ObexNotImplemented,
      502 => ObexBadGateway,
      503 => ObexServiceUnavailable,
      504 => ObexGatewayTimeOut,
      505 => ObexVersionNotSupported
    }

    include ObexHeader
	include ObexUtils

    class << self
      def read_new(sock)
        code, rest = read_resp(sock)
        res = response_class(code).new(nil, nil, rest)
		res.final = is_final?(code)
		res
      end

      private

      def read_resp(sock)
        code, len = sock.read(3).unpack('Cn')
		[from_obex(code), len-3]
      end

      def response_class(code)
        CODE_TO_OBJ[code] or
        CODE_CLASS_TO_OBJ[code/100] or
        ObexUnknownResponse
      end

	  def from_obex(code)
		  ((code & 0x70)>>4)*100 + (code & 0x0f)
	  end
	
	  def is_final?(m)
  		  (m & 0x7f) > 0 ? true : false
  	  end
    end

    public 

    def initialize(src = nil, initheader = nil,  message_length = 0)
		@method = 0
		@res = self.class
		@src = src
  		@message_length = message_length
  		@final = true
  		@body = nil
  		@read = false
  		initialize_obex_header initheader
	end

	attr_accessor	:method
	attr_accessor	:final
    attr_accessor	:message_length
    attr_reader		:read

	# For outgoing responses
	def fop(data)
        [obex_code, data.length+3, data].pack("Cna*")
	end

	def obex_code()
		num = CODE_TO_OBJ.index(@res)
		ret = ((num/100)<<4) + num%100
		#ret = (num/100)<<4 + num%100 # Does not work for some reason
		@final ? ret | 0x80 : ret
	end

	def exec(sock)
      if @src
        send_response_with_body sock
      else
        send_ sock
      end
	  @header.clear
	end

	def send_response_with_body(sock)
      data = @src.read @max_packet_size
	  if data.length < @max_packet_size
		  @header[:eob] = data
		  @res = ObexOK
		  @src.close
	  else 
		  @final = false
		  @header[:body] = data
	  end
      send_ sock
	end
	#

	def inspect
      "#<#{self.class} for #{@method} final=>#{final}>"
    end
   
    def error!(msg = nil)
      raise error_type().new(msg ? msg : inspect, self)
    end

    def error_type
      self.class::EXCEPTION_TYPE
    end

    def response
      self
    end

    private

  end #ObexResponse

end #Module

