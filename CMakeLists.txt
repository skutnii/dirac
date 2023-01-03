cmake_minimum_required(VERSION 3.0)

project(Dirac)

set(SRC_LOC "${CMAKE_SOURCE_DIR}/src")
set(LIB_LOC "${CMAKE_SOURCE_DIR}/lib")

set(ALGEBRA_LOC "${SRC_LOC}/algebra")

add_executable(dirac "${SRC_LOC}/main.cpp"
					 "${SRC_LOC}/StringInput.cpp"
					 "${SRC_LOC}/Token.cpp"
					 "${SRC_LOC}/Symbolic.cpp"
					 "${SRC_LOC}/Operations.cpp"
					 "${SRC_LOC}/ExprPrinter.cpp"
					 "${SRC_LOC}/utils.cpp"
					 "${ALGEBRA_LOC}/LorentzInvariant.cpp"
					 "${ALGEBRA_LOC}/Rational.cpp")
					 
target_include_directories(dirac PUBLIC "${LIB_LOC}/eigen/Eigen"
										"${SRC_LOC}")

set_property(TARGET dirac PROPERTY CXX_STANDARD 20)