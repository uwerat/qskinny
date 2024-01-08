if(TARGET Qt::Svg)
    add_subdirectory(svg2qvg)
    qsk_update_package_config_file(QskTools)
endif()
