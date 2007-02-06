
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
    File.open(format("../gen/include/%s", file), "w+") { |fl|
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

def make_gen_pro
    File.open("gen.pro", "w+") { |f|
        f << format("
VPATH = ../modules
VPATH += ../gen
#include(../modules/bt.pro)
TEMPLATE = app
INCLUDEPATH = ../base ../extensions ../gen/include
OBJECTS_DIR = tmp
MOC_DIR = tmp
TARGET = gen
HEADERS += ")
        @class_files.each { |cl| f << format("\\\n    ../%s.h", cl)}
        f << format("\n\nSOURCES += gen.cpp")
        @class_files.each { |cl| f << format("\\\n    ../%s.cpp", cl)}
        f << format("\n\nDESTDIR = ./\nTARGET = gen\n")
    }
end

read_class_files
make_gen_pro
gen("objects_names", "%s\"%s\",\n", object_name)
gen("objects_new", "%scase %3d: return new %s(parent);\n", new_name, true)
gen("objects_include", "%sinclude %s\n", include_name, false, "#")

def create_api
	api_lines = []
	File.open("../cqt.cpp", "r") {|fl|
		fl.each_line {|line|
			if not line =~ /define/ then
				api_lines << $' if line =~ /CQT_EXPORT /
			end
		}
	}
	File.open("../cqt_api.h", "w+") {|fl|
		fl << format(
"
#ifndef __CQT_API_H__
#define __CQT_API_H__

#ifdef _MSC_VER
#define CQT_EXPORT //so that we dont get these exports
#endif

#ifdef __cplusplus
extern \"C\" {
#endif")
		fl << "\n"
		api_lines.each {|line|
			line.chomp!
			line = line.gsub(/EXPORT/, 'IMPORT')
			line << ";\n"
			fl << line
		}

		fl << format(
"
#ifdef __cplusplus
} // end extern \"C\"
#endif

#endif //__CQT_API_H__

")
	}
end

create_api
