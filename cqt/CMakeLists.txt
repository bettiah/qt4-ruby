project(qtnative) 

cmake_minimum_required(VERSION 2.4.3)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR})

find_package(Qt4 REQUIRED) 
include(${QT_USE_FILE})
add_definitions(${QT_DEFINITIONS})

find_package(Ruby REQUIRED) 

FIND_PACKAGE(SWIG REQUIRED)
INCLUDE(${SWIG_USE_FILE})
SET(CMAKE_MY_SWIG_FLAGS "-ruby -c++")

include_directories(${QT_INCLUDE_DIR} ${RUBY_INCLUDE_PATH} base extensions )

set(qtnative_SRCS
	   cqt.cpp 
       cqt_call.cpp 
	   rb/qt_wrap.cxx
	   base/action.cpp 
	   base/button_group.cpp 
	   base/calendar_widget.cpp 
	   base/check_box.cpp 
	   base/combo_box.cpp 
	   base/date_edit.cpp 
	   base/date_time_edit.cpp 
	   base/dial.cpp
	   base/dialog.cpp 
	   base/dir_model.cpp 
	   base/dock_widget.cpp 
	   base/double_spin_box.cpp 
	   base/error_message.cpp 
	   base/focus_frame.cpp 
	   base/font_combo_box.cpp 
	   base/frame.cpp 
	   base/graphics_view.cpp 
	   base/grid_layout.cpp 
	   base/group_box.cpp 
	   base/h_box_layout.cpp 
	   base/label.cpp 
	   base/l_c_d_number.cpp 
	   base/line_edit.cpp 
	   base/list_widget.cpp
	   base/list_view.cpp
	   base/main_window.cpp 
	   base/menu.cpp 
	   base/menu_bar.cpp
	   base/progress_bar.cpp 
	   base/progress_dialog.cpp 
	   base/push_button.cpp 
	   base/radio_button.cpp 
	   base/scroll_area.cpp 
	   base/scroll_bar.cpp 
	   base/size_grip.cpp 
	   base/slider.cpp 
	   base/spin_box.cpp 
	   base/splitter.cpp 
	   base/stacked_widget.cpp 
	   base/status_bar.cpp 
	   base/tab_bar.cpp 
	   base/table_widget.cpp 
	   base/tab_widget.cpp 
	   base/text_browser.cpp 
	   base/text_edit.cpp
	   base/time_edit.cpp 
	   base/tool_bar.cpp
	   base/tool_box.cpp 
	   base/tool_button.cpp 
	   base/tree_view.cpp 
	   base/tree_widget.cpp 
	   base/v_box_layout.cpp 
	   base/widget.cpp 
	   extensions/lisp_highlighter.cpp 
	   extensions/painter_widget.cpp 
	   extensions/wiggly_widget.cpp
)

set(qtnative_MOC_HDRS
	   cqt.h
	   base/action.h 
	   base/button_group.h 
	   base/calendar_widget.h 
	   base/check_box.h 
	   base/combo_box.h 
	   base/date_edit.h 
	   base/date_time_edit.h 
	   base/dial.h 
	   base/dialog.h 
	   base/dir_model.h 
	   base/dock_widget.h 
	   base/double_spin_box.h 
	   base/error_message.h 
	   base/focus_frame.h 
	   base/font_combo_box.h 
	   base/frame.h 
	   base/graphics_view.h 
	   base/grid_layout.h 
	   base/group_box.h 
	   base/h_box_layout.h 
	   base/label.h 
	   base/l_c_d_number.h 
	   base/line_edit.h 
	   base/list_widget.h 
	   base/list_view.h 
	   base/main_window.h 
	   base/menu.h 
	   base/menu_bar.h 
	   base/progress_bar.h 
	   base/progress_dialog.h 
	   base/push_button.h
	   base/radio_button.h
	   base/scroll_area.h
	   base/scroll_bar.h 
	   base/size_grip.h 
	   base/slider.h 
	   base/spin_box.h 
	   base/splitter.h 
	   base/stacked_widget.h 
	   base/status_bar.h 
	   base/tab_bar.h 
	   base/table_widget.h 
	   base/tab_widget.h 
	   base/text_browser.h
	   base/text_edit.h 
	   base/time_edit.h 
	   base/tool_bar.h 
	   base/tool_box.h 
	   base/tool_button.h 
	   base/tree_view.h 
	   base/tree_widget.h 
	   base/v_box_layout.h 
	   base/widget.h 
	   extensions/lisp_highlighter.h 
	   extensions/painter_widget.h 
	   extensions/wiggly_widget.h
)

message (${SWIG_EXECUTABLE})

ADD_CUSTOM_COMMAND(
   OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/rb/qt_wrap.cxx
   COMMAND ${SWIG_EXECUTABLE} -ruby -c++ ${CMAKE_CURRENT_SOURCE_DIR}/rb/qt.i 
   DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/rb/qt.i
   )

#qt4_automoc(${qtnative_SRCS})
QT4_WRAP_CPP(qtnative_MOC_SRCS ${qtnative_MOC_HDRS})

add_library(qtnative SHARED ${qtnative_SRCS} ${qtnative_MOC_SRCS})

target_link_libraries(qtnative ${QT_LIBRARIES} ${RUBY_LIBRARY})

set (qtnative_TARGET
    Native.so)

set_target_properties(qtnative PROPERTIES OUTPUT_NAME Native PREFIX "" SUFFIX .so)

