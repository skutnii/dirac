cmake_minimum_required(VERSION 3.0)

project(Dirac)

set(SRC_LOC "${CMAKE_SOURCE_DIR}/src")

add_executable(dirac "${SRC_LOC}/main.cpp"
					 "${SRC_LOC}/eval.cpp"
					 "${SRC_LOC}/StringInput.cpp"
					 "${SRC_LOC}/Token.cpp"
					 "${SRC_LOC}/Compiler.cpp")

set_property(TARGET dirac PROPERTY CXX_STANDARD 20)