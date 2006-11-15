require "qt"
require "sdp"

include Qt4


with_qt() {
    main = Widget(:'tool-button')
    main.setp(:text=>'start')

    locator = nil
    
    nomore = Proc.new { |err|
        p "no more devices, err = #{err}"
        locator.terminate
        stop_gui
    }
    
    on_found_device = Proc.new { |name, addr|
        p "Remote BT Service name : " << name
        p "address = #{addr}"
        if name =~ /Akr/
            host = Widget(:'bt-remotehost')
            host.setp("remote-address"=>addr)
            host.setp("remote-name"=>name)

            on_request_complete = Proc.new { |err|
                p "no more services, err = #{err}"
                locator.send('find-next')
            }

            on_received_packet = Proc.new { |packet|
                p packet
                nomore.call 0
            }

            on_found_service = Proc.new { |name, comment, port, blob|
                ServiceRecord.new.parse(blob) { |attid, val|
    p "attribute #{attid}, #{ServiceRecord::AttIds[attid]} = #{val}"
                    if attid == ServiceRecord::AttIds.index("ServiceClassIDList")
                        if val.to_i == ServiceRecord::OBEX_OBJPUSH_CLASS
                            service = Widget(:'bt-service')
                            service.setp('service-name'=>'My Object Push')
                            service.setp('service-id'=>ServiceRecord::OBEX_OBJPUSH_CLASS)
                            service.connect('received-packet', &on_received_packet)
                            #service.send('start-receive')
                        end
                    end
                }
                host.send('find-next')
            }

            host.connect('found-service', &on_found_service)
            host.connect('request-complete', &on_request_complete)
            host.send('get-services')
        else locator.send('find-next')
        end
    }

    on_clicked = Proc.new {
        locator = Widget(:'bt-locator')
        locator.start
        locator.connect(:'found-device', &on_found_device)
        locator.connect(:'request-complete', &nomore)
        locator.send('start-lookup')
    }

    main.connect :clicked, &on_clicked
    main.show
}
