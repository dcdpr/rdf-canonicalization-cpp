# rdf-canonicalization-cpp

A C++ implementation of the [RDF Dataset Canonicalization](https://w3c.github.io/rdf-canon/spec/) specification.

## Building rdf-canonicalization-cpp

To build rdf-canonicalization-cpp, you will need:

* cmake
* g++, clang or Visual Studio (community edition)
* [jsonld-cpp](https://github.com/dcdpr/jsonld-cpp) installed locally

rdf-canonicalization-cpp uses a standard cmake build system:

```
mkdir build
cd build
cmake ..
make
```

If `jsonld-cpp` is not installed to a standard location, you will need to provide `cmake` with a prefix path:

```
cmake -DCMAKE_PREFIX_PATH=/tmp/jsonld-install ..
```

### Installing prerequisites

If the above build instructions don't work, you probably need to install some
prerequisites. For example, on a fresh Debian Bullseye system:

```
$ sudo apt install make cmake gcc g++
```

After that, make sure you can build and install `jsonld-cpp`. See its [documentation](https://github.com/dcdpr/jsonld-cpp) for more details.

Now you can try to build rdf-canonicalization-cpp again.

## Examples

There is a command line application included in rdf-canonicalization-cpp and a couple of example JSON-LD files to use.

```shell
> cat ../examples/ex01a.jsonld
```
```json
{
  "@context": {
    "name": "http://xmlns.com/foaf/0.1/name",
    "isKnownBy": { "@reverse": "http://xmlns.com/foaf/0.1/knows", "@container": "@index" }
  },
  "@id": "http://example.com/people/markus",
  "name": "Markus Lanthaler",
  "isKnownBy": {
    "Dave": {
      "@id": "http://example.com/people/dave",
      "name": "Dave Longley"
    },
    "Gregg": {
      "@id": "http://example.com/people/gregg",
      "name": "Gregg Kellogg"
    }
  }
}
```
```shell
> cat ../examples/ex01b.jsonld
```
```json
{ "@context": { "name": "http://xmlns.com/foaf/0.1/name", "isKnownBy": { "@reverse": "http://xmlns.com/foaf/0.1/knows", "@container": "@index" } }, "@id": "http://example.com/people/markus", "isKnownBy": { "Dave": { "@id": "http://example.com/people/dave", "name": "Dave Longley" }, "Gregg": { "@id": "http://example.com/people/gregg", "name": "Gregg Kellogg" } }, "name": "Markus Lanthaler" } 
```

### jsonld2canonrdf

`jsonld2canonrdf` can convert a JSON-LD document into normalized RDF data in NQuads format. This program first runs JSON-LD API's ['expansion'](https://www.w3.org/TR/json-ld-api/#expansion) algorithm on the input file, then converts it to RDF using JSON-LD API's ['RDF serialization'](https://www.w3.org/TR/json-ld-api/#rdf-serialization-deserialization), and will finally ['canonicalize'](https://w3c.github.io/rdf-canon/spec/#canon-algorithm) the result. The canonicalized RDF data will be output to the terminal.

```shell
> ./jsonld2canonrdf ../examples/ex01a.jsonld
```
```
<http://example.com/people/dave> <http://xmlns.com/foaf/0.1/knows> <http://example.com/people/markus> .
<http://example.com/people/dave> <http://xmlns.com/foaf/0.1/name> "Dave Longley" .
<http://example.com/people/gregg> <http://xmlns.com/foaf/0.1/knows> <http://example.com/people/markus> .
<http://example.com/people/gregg> <http://xmlns.com/foaf/0.1/name> "Gregg Kellogg" .
<http://example.com/people/markus> <http://xmlns.com/foaf/0.1/name> "Markus Lanthaler" .
```


One could use `jsonld2canonrdf` with the two example files included to show how the normalized RDF can be hashed to prove that the two documents are equivalent:

```shell
> ./jsonld2canonrdf ../examples/ex01a.jsonld | shasum -a 256
cb92d57d8b7abf09e4642338049bd9ee91a0ee0fe327e0a6061a8bbc2f4314bf  -
> ./jsonld2canonrdf ../examples/ex01b.jsonld | shasum -a 256
cb92d57d8b7abf09e4642338049bd9ee91a0ee0fe327e0a6061a8bbc2f4314bf  -
```


