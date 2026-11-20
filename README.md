# C++ Semantic Vector Search Engine

A command-line semantic search engine built in C++ that indexes text
documents, generates embeddings locally through Ollama, and retrieves
relevant passages using cosine similarity.

The initial test corpus is Mary Shelley's *Frankenstein*.

## Overview

This project explores the core infrastructure behind semantic vector
search:

1.  Load text documents.
2.  Identify sections such as chapters and letters.
3.  Split sections into smaller passages called **chunks**.
4.  Generate an embedding for each chunk using Ollama.
5.  Store chunk content, metadata, and embeddings.
6.  Embed a user's search query.
7.  Compare the query embedding with stored embeddings using cosine
    similarity.
8.  Rank and display the most relevant passages.

The search engine, chunking logic, vector storage, similarity
calculations, and result ranking are implemented in C++. **Ollama is
used to generate embeddings; it does not perform the vector search or
ranking.**

## Goals

-   Build a working semantic search engine in C++.
-   Understand how text is transformed into numerical embeddings.
-   Implement cosine similarity and exact nearest-neighbor search.
-   Design clear, modular components with separate responsibilities.
-   Preserve document and chapter metadata so retrieved passages can be
    traced to their source.
-   Test retrieval quality using questions about *Frankenstein*.

## Technology

  -----------------------------------------------------------------------
  Technology                          Purpose
  ----------------------------------- -----------------------------------
  C++ (C++17 or later)                Core application and search
                                      infrastructure

  Ollama                              Local model serving and embedding
                                      generation

  `nomic-embed-text:v1.5`             Text embedding model

  libcurl                             HTTP communication with Ollama

  nlohmann/json                       JSON request and response handling

  C++ Standard Library                Strings, vectors, file I/O,
                                      sorting, and data structures
  -----------------------------------------------------------------------

### Prerequisites

Before building or running the project, install the `curl` and
`nlohmann-json` libraries. On Debian/Ubuntu, these can be installed with:

``` bash
sudo apt install curl libcurl4-openssl-dev nlohmann-json3-dev
```

### Embedding model

The initial model is `nomic-embed-text:v1.5`, accessed through Ollama's
local API.

Install/download the model:

``` bash
ollama pull nomic-embed-text:v1.5
```

The application will send document passages and search queries to
Ollama's embedding endpoint:

``` text
http://localhost:11434/api/embed
```

Use the model's task prefixes:

-   Document passages: `search_document:`
-   Search queries: `search_query:`

The model generates the numerical embedding vectors. The C++ application
stores and compares those vectors.

### Generated output

Running the application writes each chunk's ID and numeric embedding
vector to `output/frankenstein_embeddings.txt`. Each entry contains a
`Chunk ID` line followed by the vector; the original chunk text is omitted.

## Architecture

The application has two main workflows: **document indexing** and
**search/retrieval**.

### 1. Document indexing

``` text
TXT file
   |
   v
Document Loader
   |
   v
Document / Section Parser
   |
   v
Text Chunker
   |
   v
Ollama Embedding Client
   |
   v
Embedding Storage
```

-   **Document Loader:** Reads text files from the documents directory.
-   **Document / Section Parser:** Identifies document structure, such
    as chapter and letter headings. The initial parser will be tailored
    to the format of the *Frankenstein* text file.
-   **Text Chunker:** Divides each section into passages suitable for
    embedding.
-   **Ollama Embedding Client:** Sends batches of passages to Ollama and
    receives embedding vectors.
-   **Embedding Storage:** Associates each generated embedding with the
    chunk it represents.

### 2. Search and retrieval

``` text
User query
   |
   v
Ollama Embedding Client
   |
   v
Query embedding
   |
   v
Similarity Search
   |
   v
Ranked embedding matches
   |
   v
Chunk lookup
   |
   v
Display matching passages and source metadata
```

The query is embedded using the same model as the documents, but with
the `search_query:` prefix. The application calculates cosine similarity
between the query vector and every stored chunk vector, sorts the
results, and retrieves the corresponding chunk content.

The initial implementation uses **exact brute-force search**. Every
stored embedding is compared with the query. Approximate
nearest-neighbor indexes are outside the initial scope.

### 3. Chunking
Relationships:

``` text
Chapter
  ^
  | chapterId
  |
Chunk
  ^
  | chunkId
  |
Embedding
```

Each chunk records which chapter it belongs to. Each embedding records
which chunk it represents. A search result can therefore be traced from
an embedding to its chunk and then to its chapter.

These structures are an initial design, not a requirement to use these
exact fields. For example, the project may later need document IDs,
section types for Walton's letters, chunk indexes, or source filenames.

## Chunking Strategy

The initial chunking strategy is **paragraph-aware and section-aware**:

-   Detect chapter and letter boundaries in the input text.
-   Process each section independently so chunks do not cross section
    boundaries.
-   Use paragraph breaks as natural splitting points.
-   Combine neighboring paragraphs into chunks when a single paragraph
    is too short.
-   Split unusually long paragraphs at sentence boundaries when
    practical.
-   Preserve the original text and associate each chunk with its source
    section.

Initial values to experiment with:

  Setting                 Starting value
  -------------------- -----------------
  Target chunk size      About 400 words
  Overlap                 About 75 words
  Preferred boundary           Paragraph
  Fallback boundary             Sentence
  Section crossing                 Never

These are experimental starting values, not fixed rules. Paragraph-level
chunking can also be tested as an alternative. Retrieval quality should
guide the final choice.

## Similarity Search

The first version uses cosine similarity:

\[ `\operatorname{cosine}`{=tex}(A,B)=
`\frac{A\cdot B}{\|A\|\|B\|}`{=tex} \]

Where:

-   \(A\) is the user's query embedding.
-   \(B\) is a stored chunk embedding.
-   (A `\cdot `{=tex}B) is the dot product.
-   (\|A\|) and (\|B\|) are vector magnitudes.

The application calculates a score for each stored chunk embedding and
sorts the results from highest to lowest.

A similarity score is a measure of vector similarity, **not a
probability that a passage is correct**.

## Planned Project Structure

One possible organization:

``` text
VectorSearchEngine/
├── documents/
│   └── frankenstein.txt
├── include/
│   ├── Chapter.h
│   ├── Chunk.h
│   ├── Embedding.h
│   ├── DocumentLoader.h
│   ├── DocumentParser.h
│   ├── TextChunker.h
│   ├── OllamaClient.h
│   ├── VectorStorage.h
│   ├── VectorOperations.h
│   └── SearchEngine.h
├── src/
│   ├── main.cpp
│   ├── DocumentLoader.cpp
│   ├── DocumentParser.cpp
│   ├── TextChunker.cpp
│   ├── OllamaClient.cpp
│   ├── VectorStorage.cpp
│   ├── VectorOperations.cpp
│   └── SearchEngine.cpp
├── tests/
├── Makefile
└── README.md
```

The final structure may be simplified or adjusted during implementation.
Avoid adding a class or file unless it has a clear responsibility.

## Implementation Milestones

### Milestone 1 --- Document loading

-   [ ] Read a TXT file into memory.
-   [ ] Handle file-open and read errors.
-   [ ] Inspect the actual *Frankenstein* file format and encoding.

### Milestone 2 --- Document parsing and chunking

-   [ ] Identify the beginning and end of the novel, excluding Project
    Gutenberg boilerplate.
-   [ ] Detect letters and chapter headings.
-   [ ] Separate sections into paragraphs.
-   [ ] Implement paragraph-aware chunk generation.
-   [ ] Preserve section metadata for every chunk.
-   [ ] Print chunks for manual inspection.

### Milestone 3 --- Ollama integration

-   [ ] Install and run Ollama.
-   [ ] Pull `nomic-embed-text:v1.5`.
-   [ ] Make an HTTP request to Ollama's embedding endpoint.
-   [ ] Parse the JSON response.
-   [ ] Generate embeddings for document chunks in batches.
-   [ ] Handle unavailable Ollama service, HTTP errors, and malformed
    responses.

### Milestone 4 --- Vector storage and similarity

-   [ ] Associate each embedding with its chunk ID.
-   [ ] Store embeddings in memory.
-   [ ] Implement dot product.
-   [ ] Implement vector magnitude.
-   [ ] Implement cosine similarity.
-   [ ] Handle mismatched dimensions and zero-magnitude vectors.

### Milestone 5 --- Search and results

-   [ ] Accept a query from the user.
-   [ ] Generate a query embedding with the `search_query:` prefix.
-   [ ] Compare it against all stored chunk embeddings.
-   [ ] Sort matches by similarity score.
-   [ ] Retrieve the matching chunk and its source section.
-   [ ] Display the top results with passage text and metadata.

### Milestone 6 --- Testing and documentation

-   [ ] Test with several questions about *Frankenstein*.
-   [ ] Inspect whether the top results actually contain relevant
    passages.
-   [ ] Test empty input, missing files, short documents, and service
    failures.
-   [ ] Document build instructions and runtime requirements.

## Example Search Questions

Use questions that test different kinds of semantic retrieval:

-   What does Victor create?
-   Why does the creature become angry?
-   How does the creature learn to communicate?
-   What happens when the creature approaches the De Lacey family?
-   How does Victor react to his creation?
-   Why does the creature seek revenge?

The search engine returns passages that are semantically similar to the
query. It does not generate a natural-language answer in the initial
version.

## Scope

### Initial scope

-   C++ command-line application.
-   TXT document input.
-   Automatic section detection and chunking.
-   Local embeddings generated through Ollama.
-   In-memory vector storage.
-   Exact cosine-similarity search.
-   Ranked passage retrieval with source metadata.

### Possible future extensions

-   Persist and reload the vector index.
-   Support multiple documents and formats.
-   Add configurable chunking strategies.
-   Compare paragraph-level and multi-paragraph chunking.
-   Add hybrid search combining keyword search (such as TF-IDF) with
    embeddings.
-   Add approximate nearest-neighbor search for larger collections.
-   Add a local language model to build a retrieval-augmented generation
    (RAG) workflow.

## Important Design Principles

-   Keep chunking, embedding generation, storage, similarity, and
    retrieval responsibilities separate.
-   Do not implement a second tokenizer for Nomic; the embedding model
    handles its own tokenization. The application can use word counts or
    another simple measurement to estimate chunk size.
-   Store source text and metadata alongside the embedding association
    so search results can be displayed and traced.
-   Do not assume vector position equals chunk ID. Use explicit IDs to
    associate records.
-   Start with in-memory storage and exact search. Add persistence or
    approximate search only after the basic engine works.
-   Treat similarity scores as ranking signals, not confidence
    percentages.

## References

-   [Ollama documentation](https://docs.ollama.com/)
-   [Ollama Embeddings API](https://docs.ollama.com/api/embed)
-   [Nomic Embed](https://www.nomic.ai/)
-   [Project Gutenberg --- Frankenstein; or, The Modern
    Prometheus](https://www.gutenberg.org/ebooks/84)
-   [Github Frankenstein](https://github.com/asweigart/codebreaker/blob/master/frankenstein.txt)

