#include "chunking_component.h"

#include <iostream>
#include <set>
#include <stdexcept>
#include <string>
#include <vector>

int fail(const std::string& message) {
  std::cerr << message << '\n';
  return 1;
}

int main() {
  Chapter firstChapter("", 1);
  Chapter secondChapter("", 1);
  const std::vector<Chunk> firstChunks =
      firstChapter.splitChunks("first paragraph\n\nsecond paragraph");
  const std::vector<Chunk> secondChunks =
      secondChapter.splitChunks("third paragraph");

  if (firstChunks.size() != 2 || secondChunks.size() != 1) {
    return fail("Unexpected number of chunks created");
  }

  std::set<int> ids;
  for (const Chunk& chunk : firstChunks) {
    if (!ids.insert(chunk.getId()).second) {
      return fail("Duplicate chunk ID across split calls");
    }
  }
  for (const Chunk& chunk : secondChunks) {
    if (!ids.insert(chunk.getId()).second) {
      return fail("Duplicate chunk ID across split calls");
    }
  }

  Chapter mutableChapter("", 1);
  mutableChapter.setNumber(7);
  mutableChapter.setChunks({Chunk("replacement chunk")});
  const std::vector<Chunk> replacementChunks = mutableChapter.getChunks();
  if (mutableChapter.getNumber() != 7 || replacementChunks.size() != 1 ||
      replacementChunks.front().getText() != "replacement chunk") {
    return fail("Chapter setters or getters returned unexpected values");
  }

  ChunkedComponent component(
      "sample",
      "Chapter 1\n\nfirst paragraph\n\nsecond paragraph\n\n"
      "Chapter 2\n\nthird paragraph");
  const std::vector<std::string> componentChunks = component.getChunks();
  const std::vector<int> componentChunkIds = component.getChunkIds();
  const std::vector<std::string> expectedChunks = {
      "first paragraph", "second paragraph", "third paragraph"};
  if (componentChunks != expectedChunks ||
      componentChunkIds.size() != componentChunks.size()) {
    return fail("Chapter headings were not excluded from the chunks");
  }

  for (std::size_t index = 0; index < componentChunkIds.size(); ++index) {
    const int expectedChapter = index < 2 ? 1 : 2;
    if (component.getChapterFromChunk(componentChunkIds[index]).getNumber() !=
        expectedChapter) {
      return fail("ID lookup returned the wrong chapter");
    }
  }

  bool missingChunkThrows = false;
  try {
    component.getChapterFromChunk(-1);
  } catch (const std::out_of_range&) {
    missingChunkThrows = true;
  }
  if (!missingChunkThrows) {
    return fail("Missing chunk ID lookup did not throw std::out_of_range");
  }
}
