cmake_minimum_required(VERSION 3.0)

project(Dirac)

set(SRC_LOC "${CMAKE_SOURCE_DIR}/src")
set(LIB_LOC "${CMAKE_SOURCE_DIR}/lib")

set(ALGEBRA_LOC "${SRC_LOC}/algebra")

set(LIB_SOURCES "${SRC_LOC}/StringInput.cpp"
			    "${SRC_LOC}/Token.cpp"
				"${SRC_LOC}/Operations.cpp"
				"${SRC_LOC}/ExprPrinter.cpp"
				"${ALGEBRA_LOC}/LorentzInvariant.cpp"
				"${ALGEBRA_LOC}/Rational.cpp"
				"${ALGEBRA_LOC}/Permutations.cpp"
				"${ALGEBRA_LOC}/Gamma.cpp")
				
add_library(dirac_common STATIC ${LIB_SOURCES})

target_include_directories(dirac_common PUBLIC "${LIB_LOC}/eigen/Eigen"
											   "${SRC_LOC}")

set_property(TARGET dirac_common PROPERTY CXX_STANDARD 20)

add_executable(dirac "${SRC_LOC}/main.cpp"
					 "${SRC_LOC}/utils.cpp"
					 "${SRC_LOC}/App.cpp")
					 
target_link_libraries(dirac dirac_common)
					 
target_include_directories(dirac PUBLIC "${LIB_LOC}/eigen/Eigen"
										"${SRC_LOC}")

set_property(TARGET dirac PROPERTY CXX_STANDARD 20)

set(FIERZ_GEN "${CMAKE_SOURCE_DIR}/examples/fierz_gen")

set(FIERZ_GEN_SRC "${FIERZ_GEN}/main.cpp"
			      "${FIERZ_GEN}/Expression.cpp"
			      "${FIERZ_GEN}/algorithms.cpp"
			      "${FIERZ_GEN}/Printer.cpp")

add_executable(fierz_gen ${FIERZ_GEN_SRC})

target_link_libraries(fierz_gen dirac_common)
					 
target_include_directories(fierz_gen PUBLIC "${LIB_LOC}/eigen/Eigen"
										"${SRC_LOC}")

set_property(TARGET fierz_gen PROPERTY CXX_STANDARD 20)
										