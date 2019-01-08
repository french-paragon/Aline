list(APPEND LIB_SRC
	    aline_global.h
		model/editableitem.cpp
		model/editableitemmanager.cpp
		model/editableitemfactory.cpp
		model/labels/label.cpp
		model/labels/labelselectionforitemproxymodel.cpp
		model/labels/labelstree.cpp
		model/labels/projectfilterbylabelproxymodel.cpp
		view/mainwindow.cpp
		view/labelitemsdockwidget.cpp
		view/projectlabelsdockwidget.cpp
		view/projecttreedockwidget.cpp
		view/editor.cpp
		view/editorfactory.cpp
		view/editableitemeditor.cpp
		view/editableitemqmleditor.cpp
		view/editableitemqmleditor.ui
		view/editorfactorymanager.cpp
		view/editableitemtypespecializedaction.cpp
		view/mainwindow.ui
		view/labelitemsdockwidget.ui
		view/projectlabelsdockwidget.ui
		view/projecttreedockwidget.ui
		utils/stringutils.cpp
		utils/jsonutils.cpp
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

target_link_libraries(${PROJECT_NAME} Qt5::Core)
target_link_libraries(${PROJECT_NAME} Qt5::Widgets)
target_link_libraries(${PROJECT_NAME} Qt5::Gui)
target_link_libraries(${PROJECT_NAME} Qt5::Qml)
target_link_libraries(${PROJECT_NAME} Qt5::QuickWidgets)
