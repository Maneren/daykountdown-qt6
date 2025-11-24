set(daykountdown_SRCS
    main.cpp
    kountdownmodel.cpp
    importexport.cpp
    resources.qrc
    )

add_executable(daykountdown ${daykountdown_SRCS})

ecm_add_app_icon(DAYKOUNTDOWN_ICON ICONS ${CMAKE_SOURCE_DIR}/org.kde.daykountdown.png)
target_sources(daykountdown PRIVATE ${DAYKOUNTDOWN_ICON})
target_link_libraries(daykountdown 
	Qt6::Quick
	Qt6::QuickControls2
	Qt6::Qml
	Qt6::Gui
	Qt6::Sql
	Qt6::Svg
	KF6::Kirigami2
	KF6::I18n
	KF6::CoreAddons
	KF6::ConfigCore
	KF6::ConfigGui
	KF6::WidgetsAddons
	KF6::KIOCore)
kconfig_add_kcfg_files(daykountdown GENERATE_MOC daykountdownconfig.kcfgc)

if(ANDROID)
    kirigami_package_breeze_icons(ICONS
        "help-about"
        "document-open"
        "document-save"
        "edit-clear"
        "list-add"
        "configure"
    )
endif()

install(TARGETS daykountdown ${KF6_INSTALL_TARGETS_DEFAULT_ARGS})
