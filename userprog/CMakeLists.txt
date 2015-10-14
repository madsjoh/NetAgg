link_libraries(m)
ADD_EXECUTABLE(netagg netagg.c)

TARGET_LINK_LIBRARIES(netagg libnetagg)
