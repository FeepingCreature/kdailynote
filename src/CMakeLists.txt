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
