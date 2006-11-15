module WbXml
    Global = {
	    0=>:switch_page,
	    1=>:end,
	    2=>:entity,
	    3=>:str_i,
	    4=>:literal,
	    40=>:ext_l_0,
	    41=>:ext_l_1,
	    42=>:ext_l_2,
	    43=>:pi,
	    44=>:literal_c,
	    80=>:ext_t_0,
	    81=>:ext_t_1,
	    82=>:ext_t_2,
	    83=>:str_t,
	    84=>:literal_a,
    }

    module Parser
	def _u_int_8
		p "parsed #{@parsed}"
		num = @buffer[@parsed]
		@parsed += 1
		num	
	end

	def _byte
		p "parsed #{@parsed}"
		@parsed += 1
		@buffer[@parsed, @parsed+=1]
	end
	
	def _mb_u_int32
		num=0
		@buffer[@parsed, 5].each_byte { |ch|
			p "parsed #{@parsed}"
			@parsed += 1
			num = num<<7 | (ch&0x7f) 
			break if (ch&0x80)==0
		}
		num
	end
    end

    class Document
	include Parser
	def initialize(buffer)
		@buffer = buffer
		@parsed = 0
		@version = _u_int_8
		@publicid = _publicid
		@charset = _mb_u_int32
		@strtbl = _strtbl
		@root = get_element
	end
	attr_reader :version, :publicid, :charset, :strtbl, :root

	def _publicid
		if @buffer[@parsed] == 0
			@parsed += 1
		end
		_mb_u_int32
	end

	def _strtbl
		len = _mb_u_int32
		data = ""
		len.times {data << _byte}
		[len, data]
	end

	def get_element
		@root = Element.new(@buffer, @parsed)
	end

	def inspect
		[@version, @publicid, @charset, @strtbl, @root]
	end
    end

    class Element
	include Parser
	def initialize(data, parsed)
		@buffer = data
		@parsed = parsed
		@tag = _u_int_8
		@has_attribute = (@tag & 0x80) == 1 ? true : false
		@has_content = (@tag & 0x40) == 1 ? true : false
		@tag = @tag & 0x3f
	end
	attr_reader :has_attribute, :has_content, :tag

	def get_attribute
		return nil unless @has_attribute
		start = _u_int8
		raise AttributeError
	end

	def get_content
	end

	def get_page
		_u_int8
	end

	def inspect
		[@tag, @has_attribute, @has_content]
	end
    end

end


buffer = <<EOF
�j mlq�1.2r�
SyncML/1.2e�13[�1nW�http://192.168.0.1:5865/SyncgW�IMEI:392511177527475Z L�10000  kFK�1O�201TnW�
./ContactsgW�./C:Contacts.cdbZ EJ� O�20060915T142609Z  FK�2O�201TnW�./NotesgW�./c:Notepad.datZ EJ� O�20060915T142609Z  _K�3Z S�#application/vnd.syncml-devinf+wbxml  TgW�
./devinf12OÊ4�Tj Je�1.2Q�UnknownU�UnknownO� ^� S60_3_0_200548P� I�IMEI:392511177527475K�phone(*)G]�./C:Contacts.cdbL�ContactsR�8ZF�text/x-vcardd�2.1bF�text/x-vcardd�2.1EF�text/x-vcardd�2.1kX�BEGINH� c�VCARDL�BeginkX�ENDH� c�VCARDL�EndkX�VERSIONH� c�2.1L�VersionkX�REVH� L�RevisionkX�NH� L�NamekX�ADRH� L�AddresslW�TYPEH� c�HOMEc�WORKL�TypekX�TELH� L�Telephone numberlW�TYPEH� c�HOMEc�WORKc�CELLc�PAGERc�FAXc�VIDEOL�TypekX�FNH� L�FullNamekX�EMAILH� L�Email addresslW�TYPEH� c�HOMEc�WORKL�TypekX�URLH� L�URL addresslW�TYPEH� c�HOMEc�WORKL�TypekX�NOTEH� L�NotekX�TITLEH� L�TitlekX�ORGH� L�OrganisationkX�PHOTOH� L�PhotokX�BDAYH� L�BirthdaykX�SOUNDH� L�SoundkX�X-WV-IDH� L�Wireless Village IdkX�X-EPOCSECONDNAMEH� L�NicknamekX�X-SIPH� L�SIP protocollW�TYPEH� c�POCc�SWISL�Type_`�1`�2`�3`�4`�5`�6`�7G]�./c:Notepad.datL�NotesR�8ZF�
text/plaind�1.0bF�
text/plaind�1.0EF�
text/plaind�1.0kX�BEGINH� c�NOTEL�BeginkX�ENDH� c�NOTEL�EndkX�VERSIONH� c�VERSIONL�VersionkX�IDH� L�IDkX�LAST-MODIFIEDH� L�Last ModifiedkX�MEMOH� L�MEMO_`�1`�2`�3`�4`�5`�6`�7SK�4Z S�#application/vnd.syncml-devinf+wbxml  TnW�
./devinf12
EOF

include WbXml
doc = Document.new(buffer)
p doc.version
p doc.publicid
p doc.charset
p doc.strtbl
p doc.root.tag
p doc.root.has_attribute
p doc.root.has_content
cnt = 0
while 1
	ele = doc.get_element
	p "#{ele.times'  '}, %d %d %d ", ele.tag, ele.has_attribute, ele.has_content
	p ele.get_attribute if ele.has_attribute
	p ele.get_content if ele.has_content
	if Global[ele.tag] == :end
		cnt -= 1
	elsif Global[ele.tag] == :switch_page
		code = ele.get_page
 		p "switching to #{code}"
	end
end
