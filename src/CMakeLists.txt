list(APPEND LIB_SRC
		app.cpp
		mainwindow.cpp
		aline_global.h
		editor.cpp
		mainwindow.ui
		)

add_library(${PROJECT_NAME} ${LIB_SRC})
#set_target_properties( ${PROJECT_NAME}
#	PROPERTIES
#	ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
#	LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
#	RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
#)
target_compile_definitions(${PROJECT_NAME}
  PRIVATE ALINE_LIBRARY)

QT5_USE_MODULES(${PROJECT_NAME} Core)
QT5_USE_MODULES(${PROJECT_NAME} Widgets)
QT5_USE_MODULES(${PROJECT_NAME} Gui)
