# Copyright 2017-present Yeolar

#execute_process(COMMAND
#  ${PROJECT_SOURCE_DIR}/_build/crystal/tools/crystal-recordgen
#    -conf ${PROJECT_SOURCE_DIR}/crystal/record/test/record.cson
#    -outputdir ${PROJECT_SOURCE_DIR}/crystal/record/test/
#    -addid)

test_sources(
  AllocMaskTest.cpp
  RecordConfigTest.cpp
  RecordTest.cpp
)
