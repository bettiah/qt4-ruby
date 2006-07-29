@class_files = []
def read_class_files
     File.open("../cqt.pro", "r") {|file|
         file.each_line { |line|
             line =~ /(.*\s)(.*)(\.h)/
             match = "#{$2}"
             @class_files << match unless ((match.length == 0) || (match !~ %r{/}))
         }
     }
end

def gen(file, frmt, fn, count=false, char="\t")
    File.open(format("include//%s", file), "w+") { |fl|
        n=0
        @class_files.each { |s|
            name = s.split("\/").last
            if(count)
                n+=1
                name = fn.call name
                fl << format(frmt, char, n, name)
            else
                fl << format(frmt, char, (fn.call name))
            end
        }
    }
end

object_name = lambda { |name| 
    name.tr "_", "-"
}

new_name = lambda { |name|
   s=""
   name.each("_") { |c|
       s << c.delete('_').capitalize
   }
   return s
}

include_name = lambda { |name|
    format "<%s.h>", name
}
    
read_class_files
gen("objects_names", "%s\"%s\",\n", object_name)
gen("objects_new", "%scase %3d: return new %s(w);\n", new_name, true)
gen("objects_include", "%sinclude %s\n", include_name, false, "#")
