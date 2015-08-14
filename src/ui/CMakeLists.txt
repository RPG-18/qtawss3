set(SRC ObjectTree.cpp
        Settings.cpp
        MainWindow.cpp
        KeyDelegate.cpp
        ObjectTreeModel.cpp
        SettingsDialog.cpp
        ErrorDialog.cpp
        FolderDialog.cpp
)

set(HEADERS ObjectTree.h
            Settings.h
            MainWindow.h
            KeyDelegate.h
            ObjectTreeModel.h
            SettingsDialog.h
            ErrorDialog.h
            FolderDialog.h
)

set(LIBS Qt5::Core
         Qt5::Widgets
         Qt5::Concurrent
)

set(UIC MainWindow.ui
        ErrorDialog.ui
        FolderDialog.ui
        SettingsDialog.ui
)
qt5_wrap_ui(UICS ${UIC})

include_directories(../core)

add_library(gui STATIC ${SRC} ${HEADERS} ${UICS})
target_link_libraries(gui ${LIBS})

add_executable(qtawss3 main.cpp ${HEADERS})
target_link_libraries(qtawss3 gui core)