# WeMakeSoftware.h

This header provides a simplified interface for kernel module development on Linux. It includes essential tools and macros that streamline kernel operations and improve readability.

### Key Macros:
- **Export(symbol)**: A shorthand for `EXPORT_SYMBOL(symbol)`, making it easier to export symbols from the module for use in other kernel modules.
  
- **Info(fmt, ...)**: A simplified logging macro that prints informational messages with the function name prefixed. The output is logged using `printk` at the `INFO` level.
  
- **Warning(fmt, ...)**: A logging macro that prints warning messages, including the current function name. The output is logged at the `WARNING` level.
  
- **Error(fmt, ...)**: A logging macro for error messages, also including the function name in the log. The output is logged at the `ERROR` level.
  
- **GetFunction(name)**: A macro that wraps around `kallsyms_lookup_name`, allowing you to retrieve the address of a kernel symbol by name. It casts the result to an integer.
  
- **Malloc(size, flags)**: A simplified wrapper for `kmalloc`, making memory allocation easier by calling `kmalloc(size, flags)`.

### Metadata:
The `MODULE_METADATA()` macro is used to define the basic metadata for the kernel module, including:
- **GPL License**
- **Description**: "A Simple Kernel Module"
- **Author**: Pirasath Luxchumykanthan