list(APPEND LIB_SRC
            aline_global.h
            model/editableitem.h
            model/editableitem.cpp
            model/editableitemmanager.h
            model/editableitemmanager.cpp
            model/editableitemfactory.h
            model/editableitemfactory.cpp
            model/editableitemmanagerfactory.h
            model/editableitemmanagerfactory.cpp
            model/labels/label.h
            model/labels/label.cpp
            model/labels/labelselectionforitemproxymodel.h
            model/labels/labelselectionforitemproxymodel.cpp
            model/labels/labelstree.h
            model/labels/labelstree.cpp
            model/labels/projectfilterbylabelproxymodel.h
            model/labels/projectfilterbylabelproxymodel.cpp
            model/exceptions/itemioexception.h
            model/exceptions/itemioexception.cpp
            model/exceptions/datasourcebusyexception.h
            model/exceptions/datasourcebusyexception.cpp
            model/viewmodel/externalitemreferencelistmodel.h
            model/viewmodel/externalitemreferencelistmodel.cpp
            model/viewmodel/editableitemfilteredconnectionproxymodel.h
            model/viewmodel/editableitemfilteredconnectionproxymodel.cpp
            view/mainwindow.h
            view/mainwindow.cpp
            view/mainwindow.ui
            view/labelitemsdockwidget.h
            view/labelitemsdockwidget.cpp
            view/labelitemsdockwidget.ui
            view/projectlabelsdockwidget.h
            view/projectlabelsdockwidget.cpp
            view/projectlabelsdockwidget.ui
            view/projecttreedockwidget.h
            view/projecttreedockwidget.cpp
            view/projecttreedockwidget.ui
            view/editor.h
            view/editor.cpp
            view/editorfactory.h
            view/editorfactory.cpp
            view/editableitemeditor.h
            view/editableitemeditor.cpp
            view/editableitemqmleditor.h
            view/editableitemqmleditor.cpp
            view/qmleditorfactory.h
            view/qmleditorfactory.cpp
            view/editableitemqmleditor.ui
            view/editorfactorymanager.h
            view/editorfactorymanager.cpp
            view/editableitemtypespecializedaction.h
            view/editableitemtypespecializedaction.cpp
            utils/stringutils.h
            utils/stringutils.cpp
            utils/jsonutils.h
            utils/jsonutils.cpp
            utils/view_model/indexrebasedproxymodel.h
            utils/view_model/indexrebasedproxymodel.cpp
            control/app.h
            control/app.cpp
            ressources/aline_ressources.qrc
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

install (TARGETS ${PROJECT_NAME} DESTINATION usr/lib)
