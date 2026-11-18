file(MAKE_DIRECTORY "${OUTPUT_DIR}/documents")
file(COPY "${SOURCE_DIR}/documents/frankenstein.txt"
     DESTINATION "${OUTPUT_DIR}/documents")

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
  message(FATAL_ERROR "assignment unexpectedly wrote chunks to the terminal")
endif()

set(output_file "${OUTPUT_DIR}/frankenstein_chunks.txt")
if(NOT EXISTS "${output_file}")
  message(FATAL_ERROR "assignment did not create frankenstein_chunks.txt")
endif()
file(READ "${output_file}" output)

if(NOT output MATCHES "Chunk ID: 1")
  message(FATAL_ERROR "output file does not contain chunk IDs and text")
endif()

if(output MATCHES "Chunk ID: [0-9]+\nChapter [0-9]+")
  message(FATAL_ERROR "output file contains a chapter heading as a chunk")
endif()

if(NOT output MATCHES "lost in darkness and distance\\.")
  message(FATAL_ERROR "output file does not contain the end of the book")
endif()

string(REGEX MATCHALL "Chunk ID: [0-9]+" chunk_headers "${output}")
list(LENGTH chunk_headers chunk_count)
list(GET chunk_headers -1 last_header)
string(REGEX REPLACE "Chunk ID: " "" last_id "${last_header}")
if(NOT last_id EQUAL chunk_count)
  message(FATAL_ERROR "output file does not contain every chunk")
endif()
