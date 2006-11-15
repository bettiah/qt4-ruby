
class ServiceRecord
    AttIds =  {0=>"ServiceRecordHandle", 
            1=>"ServiceClassIDList",
            2=>"ServiceRecordState",
            3=>"ServiceID",
            4=>"ProtocolDescriptorList",
            5=>"BrowseGroupList",
            6=>"LanguageBaseAttributeIDList",
            7=>"ServiceInfoTimeToLive",
            8=>"ServiceAvailability",
            9=>"BluetoothProfileDescriptorList",
            10=>"DocumentationURL",
            11=>"ClientExecutableURL",
            12=>"IconURL",
			512=>"VersionNumberList",
			513=>"ServiceDatabaseState"
            }
    
SDP_SERVER_CLASS = 0x1000
BROWSE_GRP_DESC_CLASS = 0x1001
PUBLIC_BROWSE_GROUP = 0x1002
SERIAL_PORT_CLASS = 0x1101
LAN_ACCESS_CLASS = 0x1102
DIALUP_NET_CLASS = 0x1103
IRMC_SYNC_CLASS = 0x1104
OBEX_OBJPUSH_CLASS = 0x1105
OBEX_FILETRANS_CLASS = 0x1106
IRMC_SYNC_CMD_CLASS = 0x1107
HEADSET_CLASS = 0x1108
CORDLESS_TELEPHONY_CLASS = 0x1109
AUDIO_SOURCE_CLASS = 0x110a
AUDIO_SINK_CLASS = 0x110b
AV_REMOTE_TARGET_CLASS = 0x110c
ADVANCED_AUDIO_CLASS = 0x110d
AV_REMOTE_CLASS = 0x110e
VIDEO_CONF_CLASS = 0x110f
INTERCOM_CLASS = 0x1110
FAX_CLASS = 0x1111
HEADSET_AGW_CLASS = 0x1112
WAP_CLASS = 0x1113
WAP_CLIENT_CLASS = 0x1114
PANU_CLASS = 0x1115
NAP_CLASS = 0x1116
GN_CLASS = 0x1117
DIRECT_PRINTING_CLASS = 0x1118
REFERENCE_PRINTING_CLASS = 0x1119
IMAGING_CLASS = 0x111a
IMAGING_RESPONDER_CLASS = 0x111b
IMAGING_ARCHIVE_CLASS = 0x111c
IMAGING_REFOBJS_CLASS = 0x111d
HANDSFREE_CLASS = 0x111e
HANDSFREE_AGW_CLASS = 0x111f
DIRECT_PRT_REFOBJS_CLASS = 0x1120
REFLECTED_UI_CLASS = 0x1121
BASIC_PRINTING_CLASS = 0x1122
PRINTING_STATUS_CLASS = 0x1123
HID_CLASS = 0x1124
HCR_CLASS = 0x1125
HCR_PRINT_CLASS = 0x1126
HCR_SCAN_CLASS = 0x1127
CIP_CLASS = 0x1128
VIDEO_CONF_GW_CLASS = 0x1129
UDI_MT_CLASS = 0x112a
UDI_TA_CLASS = 0x112b
AV_CLASS = 0x112c
SAP_CLASS = 0x112d
PNP_INFO_CLASS = 0x1200
GENERIC_NETWORKING_CLASS = 0x1201
GENERIC_FILETRANS_CLASS = 0x1202
GENERIC_AUDIO_CLASS = 0x1203
GENERIC_TELEPHONY_CLASS = 0x1204
UPNP_CLASS = 0x1205
UPNP_IP_CLASS = 0x1206
UPNP_PAN_CLASS = 0x1300
UPNP_LAP_CLASS = 0x1301
UPNP_L2CAP_CLASS = 0x1302
VIDEO_SOURCE_CLASS = 0x1303
VIDEO_SINK_CLASS = 0x1304

	def ServiceRecord.parseDe(blob)
		bin = blob.slice!(0)
		typ = bin >> 3
        idx = bin & 7

		size_of_data= case idx
			  		  when 0 then 1 unless typ == 0
			  		  when 1 then 2
			  		  when 2 then 4
			  		  when 3 then 8
					  when 4 then 16
					  when 5 then blob.slice!(0)
					  when 6 then blob.slice!(0, 2).unpack('n')[0]
					  when 7 then blob.slice!(0, 4).unpack('N')[0]
					  end
		#p "typ=#{typ}, data_size=#{size_of_data}, left=#{blob.length}, idx = #{idx}"
		yield typ, size_of_data, blob
		blob.slice!(0, size_of_data) unless typ == 0
	end

	def parse(blob, &block)
		count = 0
		while true
			#p "******ITER****** #{count+=1}, #{blob.length}"
			break if blob.length == 0
			ServiceRecord.parseDe(blob) { |typ, size_of_data, blob|
				case typ
				when 0 #special case
				when 1, 2 then
                    case size_of_data
                    when 1
						yield @attribute, "#{blob[0].to_i}"
                    when 2 #for ??
						typ==1 ? (@attribute = blob.unpack('n')[0]) : (yield @attribute, "#{blob.unpack('n')[0]}")
                    when 4
						yield @attribute, "#{blob.unpack('N')[0]}"
					end # case
				when 3 then #UID
					case size_of_data
					when 2 then 
						yield @attribute, "#{blob.unpack('n')[0]}"
					when 4
					when 16
					end #case size_of_data
				when 4, 8 then #string, url
					yield @attribute, blob[0, size_of_data]
				when 7,6
					inner = blob[0, size_of_data]
					case @attribute
					when nil, 1, 5, 9
						parse(inner, &block)
                    when 4 #ProtocolDescriptorList
                        pdl = inner[0, size_of_data]
		                while true
			            break if pdl.length == 0
                        ServiceRecord.parseDe(pdl) { |typ, size_of_data, inner|
                            if (typ == 6)
                                desc = inner[0, size_of_data]
                                while 1
                                    break if desc.length == 0
                                    ServiceRecord.parseDe(desc) { |typ, size_of_data, inner|
                                        if (typ==3) 
                                            yield @attribute, "#{inner.unpack('n')[0]}"
                                        elsif (typ==1) 
                                            yield @attribute, "port #{inner[0].to_i}"
                                        end
                                    }
                                end
                            end
						}
                        end
					when 6 #LanguageBaseAttributeIDList
						ServiceRecord.parseDe(inner) { |typ, size_of_data, inner|
							yield @attribute, "lang #{inner[0, size_of_data]}"
						}
						ServiceRecord.parseDe(inner) { |typ, size_of_data, inner|
							yield @attribute, "enc #{inner.unpack('n')[0]}"
						}
						ServiceRecord.parseDe(inner) { |typ, size_of_data, inner|
							id_base = inner.unpack('n')[0]
							AttIds[id_base] = "ServiceName"
							AttIds[id_base+1] = "ServiceDescription"
							AttIds[id_base+2] = "ProviderName"
							yield @attribute, "id_base #{inner.unpack('n')[0]}"
						}
					when 9 #BluetoothProfileDescriptorList
						ServiceRecord.parseDe(inner) { |typ, size_of_data, inner|
							yield @attribute, "UUID = #{inner[0, size_of_data]}"
						}
						ServiceRecord.parseDe(inner) { |typ, size_of_data, inner|
							yield @attribute, "version = #{inner.unpack('n')[0]}"
						}
					when 512 #VersionNumberList
						ServiceRecord.parseDe(inner) { |typ, size_of_data, inner|
							yield @attribute, "version = #{inner.unpack('n')[0]}"
						}
					end #case attribute
				end #case typ
			} #do
		end #while
	end
end

if __FILE__ == $0
    arr = "5\263\t\000\000\n\000\000\000\000\t\000\0015\003\031\020\000\t\000\002\n\000\000\000\000\t\000\0045\0055\003\031\001\000\t\000\0055\003\031\020\000\t\000\0065\t\ten\t\000j\t\001\000\t\000\a\n\000\000\004\260\t\000\010\010\377\t\001\000%\nSDP Server\t\001\001%6Provides local service information forremote devices.\t\001\002%\fSymbian Ltd.\t\002\0005\006\t\001\000\t\001\001\t\002\001\n\000\000\000\021"
    arr2 = "5m\t\000\000\n\000\001\000\005\t\000\0015\003\031\021\005\t\000\002\n\000\000\000\a\t\000\0045\0215\003\031\001\0005\005\031\000\003\010\t5\003\031\000\010\t\000\0055\003\031\020\002\t\000\0065\t\tEN\t\000j\t\001\000\t\000\t5\0105\006\031\021\005\t\001\000\t\001\000%\020OBEX Object Push\t\003\0035\002\010\377"
    ServiceRecord.new.parse(arr2) { |attid, val|
    p "attribute #{attid}, #{ServiceRecord::AttIds[attid]} = #{val}"
    }
end
