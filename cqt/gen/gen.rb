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
