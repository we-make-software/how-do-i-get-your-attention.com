# Memory.h

This header file defines a standard interface for memory management that can be included in various projects. By including **`wms.h`**, it establishes a connection to the broader **`wms`** module, ensuring seamless integration with other components of the project.

The use of `#pragma once` prevents multiple inclusions of the header during compilation, ensuring efficiency and avoiding potential redefinition errors. The file provides the necessary external declaration of the function `waitForMemory`, allowing other modules to request memory allocation in a standardized way.

As a flexible and reusable header, `Memory.h.md` serves as a foundational part of the memory management system in any project where dynamic memory allocation is needed.