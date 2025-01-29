add_executable(kdailynote
    main.cpp
    diarywindow.cpp
    diaryeditor.cpp
)

target_link_libraries(kdailynote
    Qt::Core
    Qt::Widgets
    KF6::I18n
    KF6::CoreAddons
    KF6::TextWidgets
)

install(TARGETS kdailynote ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})

# Tests
if(BUILD_TESTING)
    add_executable(testdiaryeditor
        tests/testdiaryeditor.cpp
        diaryeditor.cpp
    )
    target_link_libraries(testdiaryeditor
        Qt::Core
        Qt::Widgets
        Qt6::Test
        KF6::TextWidgets
    )
    add_test(NAME testdiaryeditor COMMAND testdiaryeditor)
endif()
