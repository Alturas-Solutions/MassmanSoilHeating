cmake_minimum_required (VERSION 3.8)

project (massmanhmv VERSION 1.0 LANGUAGES CXX)

IF(MSVC)
	add_compile_definitions( _CRT_SECURE_NO_WARNINGS)
#	target_compile_options(${PROJECT_NAME} /fp:except)
ENDIF(MSVC)

set(HEADER_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(HEADERS 
	${HEADER_DIR}/BMSoil.h
	${HEADER_DIR}/Crank.h
	${HEADER_DIR}/HTAA.h
	${HEADER_DIR}/Matrix.h
	${HEADER_DIR}/SCN_Arrays.h
	)

add_library(${PROJECT_NAME}  STATIC
	${HEADERS}
	src/AccumulateHMV.cpp
	src/BM_Util.cpp
	src/BoundarydBFD.cpp
	src/BoundaryIR.cpp
	src/BoundaryIR_T1.cpp
	src/BoundaryLHB17dBFD.cpp
	src/BoundaryU.cpp
	src/BoundaryUBFD.cpp
	src/calAwa.cpp
	src/calAwaP.cpp
	src/calConCoef2.cpp
	src/calConCoef5.cpp
	src/calcondry.cpp
	src/calconHMVl.cpp
	src/calconHMVll.cpp
	src/calconRAD.cpp
	src/calcpaHMVNR.cpp
	src/calcp0HMV.cpp
	src/calcpaHMVNR.cpp
	src/calcsHMV.cpp
	src/calcsHMVnT.cpp
	src/caldelHMV.cpp
	src/calden2HMV.cpp
	src/calDENVEL.cpp
	src/caldiffHMVNRa.cpp
	src/caldiseq.cpp
	src/caldryvis.cpp
	src/calEBCN.cpp
	src/calepssurfHMV.cpp
	src/caleta4.cpp
	src/calgascomb.cpp
	src/calhydrauKF.cpp
	src/calhydrauVA.cpp
	src/calhydrauWRC.cpp
	src/calmaxdenwHMV.cpp
	src/calmaxmulaHMV.cpp
	src/calmaxmulaWHMV.cpp
	src/calmulaHMV.cpp
	src/calmulaWHMV.cpp
	src/calparx.cpp
	src/calprofP.cpp
	src/calpsinProf.cpp
	src/calQHCN.cpp
	src/calrhev.cpp
	src/calstefan1NR.cpp
	src/calsurfdT.cpp
	src/calSw.cpp
	src/caltempkHMV.cpp
	src/calthetaCSr.cpp
	src/calthetaFYr.cpp
	src/caluHMV.cpp
	src/calvaporHMV.cpp
	src/calVsourceGNRa.cpp
	src/calxhiv1.cpp
	src/CrankNicolson.cpp
	src/densatHMV.cpp
	src/densatmaxHMV.cpp
	src/Dry_Air_Constants_HMV.cpp
	src/EBcoef.cpp
	src/EBcoef-Save.cpp
	src/GenThomas.cpp
	src/Glob_Data.cpp
	src/global.cpp
	src/harmean.cpp
	src/HTAA.cpp
	src/Liquid_Water_Constants.cpp
	src/Liquid_Water_Constants_2.cpp
	src/LMS.cpp
	src/LoadQuincy1.cpp
	src/Matrix.cpp
	src/Model_Param.cpp
	src/Model_Switch_HMV.cpp
	src/Physical_Constants_HMV.cpp
	src/PSINi.cpp
	src/PsinTmpnT.cpp
	src/Quincy1G.cpp
	src/rhoveqHMV.cpp
	src/SCN_Arrays.cpp
	src/Soil_Model_Data_Files_HMV.cpp
	src/Soil_Time_Depth_Param_HMV.cpp
	src/SolveHMV.cpp
	src/TryMatix.cpp
	src/vapdiffEHMV.cpp
	src/vaporTempdiff.cpp
	src/Vmult5.cpp
	src/Water_Vapor_Constants.cpp
	src/Water_Vapor_Constants_0.cpp
	src/Water_Vapor_Constants_2.cpp
	src/WesternUS01.cpp
	src/Xnew.cpp
	)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include  )

install(TARGETS ${PROJECT_NAME} DESTINATION "${lib_dest}")
