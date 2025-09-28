cmake_minimum_required(VERSION 3.16)
project(NaI_Spectrum_Na22 LANGUAGES CXX)

find_package(Geant4 REQUIRED ui_all vis_all)

add_executable(nai_spectrum
  src/main.cc
  src/DetectorConstruction.cc
  src/ActionInitialization.cc
  src/PrimaryGeneratorAction.cc
  src/RunAction.cc
  src/EventAction.cc
  src/SteppingAction.cc)

target_include_directories(nai_spectrum PRIVATE ${PROJECT_SOURCE_DIR}/include)

# conda-forge の Geant4 では ${Geant4_LIBRARIES} が正解
target_link_libraries(nai_spectrum PRIVATE ${Geant4_LIBRARIES})

set_property(TARGET nai_spectrum PROPERTY CXX_STANDARD 17)

include(${Geant4_USE_FILE})

