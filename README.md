# InazumaQuery

**InazumaQuery** is a performance-oriented C library designed for structured querying and predicate-based filtering of character attribute datasets. 

Developed to provide a high-efficiency alternative to manual data analysis, this library implements a flexible query engine using function pointers to allow for complex, user-defined filtering and sorting logic across multiple game databases.

> [!CAUTION]
> This library it's still in early development phase (pre-alpha). Bugs have to be expected.

## Table of contents

- [InazumaQuery](#inazumaquery)
  - [Table of contents](#table-of-contents)
  - [Features](#features)
  - [Supported platform](#supported-platform)
  - [Getting started](#getting-started)
  - [Project setup](#project-setup)
  - [Credits](#credits)

## Features

**Currently available features:**

- Filter and sort players and moves using their data and easily get the ones you are looking for
- Use the provided databases for IE1 , IE2 and IE3 located in the [`data/`](https://github.com/mengacpp/InazumaQuery/tree/main/data) folder

**Planned features:**

- Create built in compare functions to sort players by 'skills' rather than by statistic (e.g. sorting by general ability to shoot rather than by kick statistic).  
- Add secondary informations about players (team, recruitment method, recruitment location, skills)
- Allow queries to accept an offset parameter for pagination.

**Known bugs:**

- Some nicknames in the IE3 database and stats in the IE1 database (less than 5% I'd say) are subject to small decryption bugs that might influece accuracy
- Some players are duplicates, this is probably caused by the different versions of the same players depending on the game version

## Supported platform

Every platform is theroetically supported, as there are no platform specific dependencies, but only MacOS has been tested right now.

> [!NOTE]
> I will create issue templates in the near future so that you can point out bugs you've found.

## Getting started

Using the API is straight forward: [setup your project](#project-setup) to link to this library, and start using it.

You can learn how to use the API by reading the [examples](https://github.com/mengacpp/InazumaQuery/tree/main/examples) and the articles on the [wiki](https://github.com/mengacpp/InazumaQuery/wiki).

## Project setup

You can link to this library by adding it as a subdirectory in your `CMakeLists.txt`.

> [!NOTE]
> Theoretically you can just copy and paste the `InazumaQuery/` folder in your source directory and compile it as is, but i wouldn't recommend it since i haven't tested this method.

First of, create a git submodule in a directory of your project. I'd recommend placing it in a folder named `vendor/` or `third_party/`:

```shell
git submodule add --depth 1 https://github.com/mengacpp/InazumaQuery.git vendor/InazumaQuery
```

> [!TIP]
> If you prefer, you can just clone the repostory. The `--depth 1` command makes sure that you clone just what you need.

Now, in your `CMakeLists.txt`:

```cmake
# project(YourProject) ...
add_subdirectory(vendor/InazumaQuery)
target_link_libraries(YourProject PRIVATE InazumaQuery)
```

Before the `add_subdirectory()` call, there are some options that you can modify to customise the way the library gets built:

| Option name | Type | Default value | Description |
|------------------|------|------|-----------------|
| `INA_BUILD_SHARED` | `bool` | `TRUE` | Decide wether to build the library as static or shared. |

```cmake
set(INA_BUILD_SHARED FALSE) # Build a static library
add_subdirectory(vendor/InazumaQuery)
# ...
```

## Credits

- Player stats and data used in this project were sourced from [SwareJonge](https://github.com/SwareJonge)'s comprehensive [Inazuma Eleven databases](https://docs.google.com/spreadsheets/d/1qfanvDyPubSLyfcOMuXN9IbGtr7U1jr-5FRCf2R7FQA/edit?gid=469737450#gid=469737450). Huge thanks to their work!
