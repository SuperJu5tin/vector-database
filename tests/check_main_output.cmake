file(MAKE_DIRECTORY "${OUTPUT_DIR}/documents")
file(MAKE_DIRECTORY "${OUTPUT_DIR}/output")
file(WRITE "${OUTPUT_DIR}/documents/frankenstein.txt"
     "Chapter 1\nFirst test paragraph.\n\nSecond test paragraph.\n")

execute_process(
  COMMAND "${ASSIGNMENT_EXECUTABLE}"
  WORKING_DIRECTORY "${OUTPUT_DIR}"
  RESULT_VARIABLE result
  OUTPUT_VARIABLE terminal_output
  ERROR_VARIABLE error
)

if(NOT result STREQUAL "0")
  message(FATAL_ERROR "assignment failed: ${error}")
endif()

if(NOT terminal_output STREQUAL "")
  message(FATAL_ERROR "assignment unexpectedly printed to the console")
endif()

set(embeddings_file "${OUTPUT_DIR}/output/frankenstein_embeddings.txt")
if(NOT EXISTS "${embeddings_file}")
  message(FATAL_ERROR "assignment did not create frankenstein_embeddings.txt")
endif()
file(READ "${embeddings_file}" embeddings_output)

if(NOT embeddings_output MATCHES "Chunk ID: 1")
  message(FATAL_ERROR "embeddings file is missing the first chunk ID")
endif()

if(NOT embeddings_output MATCHES "Chunk ID: 2")
  message(FATAL_ERROR "embeddings file is missing the second chunk ID")
endif()

if(NOT embeddings_output MATCHES "\\[[ \t\r\n]*[-0-9]")
  message(FATAL_ERROR "embeddings file does not contain numeric vector values")
endif()

if(embeddings_output MATCHES "First test paragraph|Second test paragraph")
  message(FATAL_ERROR "embeddings file unexpectedly contains chunk text")
endif()
