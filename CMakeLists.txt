cmake_minimum_required(VERSION 3.1)
enable_language(C)
set(CMAKE_C_STANDARD 11) # C++11...
set(CMAKE_C_STANDARD_REQUIRED ON) #...is required...
set(CMAKE_C_EXTENSIONS OFF) #...without compiler extensions like gnu++11

## Set our project name
project(romaji2kana_sample)

set(romaji2kana_src 
	romaji2kana/romaji2kana.c
    romaji2kana/Source.c
)
if(MSVC)
  # Force to always compile with W4
  if(CMAKE_C_FLAGS MATCHES "/W[0-4]")
    string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
  else()
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /W4")
  endif()
elseif(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUC)
  # Update if necessary
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Wno-long-long -pedantic")
endif()
## Define the executable
add_executable(romaji2kana_sample ${romaji2kana_src})
