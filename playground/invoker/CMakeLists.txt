list(APPEND HEADERS Callback.h Invoker.h)
list(APPEND SOURCES Callback.cpp Invoker.cpp main.cpp)

qsk_example(invoker)
set_target_properties(invoker PROPERTIES AUTOMOC_MOC_OPTIONS --no-warnings)

