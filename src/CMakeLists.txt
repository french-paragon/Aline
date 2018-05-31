list(APPEND LIB_SRC
	    aline_global.h
		model/editableitem.cpp
		model/editableitemmanager.cpp
		model/editableitemfactory.cpp
		view/mainwindow.cpp
		view/editor.cpp
		view/editorfactory.cpp
		view/editableitemeditor.cpp
		view/editorfactorymanager.cpp
		view/mainwindow.ui
		utils/stringutils.cpp
		control/app.cpp
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
