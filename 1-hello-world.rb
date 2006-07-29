require "qt"

include Qt4

with_qt() {
    lb = Widget(:label)
    p "#{lb.class} (\##{lb.object_id}): #{lb.to_s}" 
    lb.setp(:text => "<h1>Hello Baby!!")
    lb.show
}
