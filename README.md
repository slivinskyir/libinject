
# C++ Function Injection

## 1. Why use it?

Function injection makes it easy to test code that depends on external functions or other components without calling their real implementations.

- Simulate success and failure scenarios.
- Test error handling and edge cases.
- Replace function behavior with simple lambdas.
- Restore the original behavior automatically after the test scope ends.

## 2. Production code changes

Mark functions that should support injection with `inject__`:

```cpp
inject__ bool DownloadFile(const char* Path, char* Content)
{
    // Original implementation
    return false;
}
```

For normal production builds, `inject__` expands to nothing. When injection is enabled, GCC and Clang use weak symbols.

## 3. Generate function copies

Run the Python script to extract marked function implementations into a separate source file:

```bash
python3 inject.py . -o tests/base/injected_functions.cpp
```

The generated copies provide the default implementations used by the injection wrappers. Regenerate the file whenever the original implementations change.

## 4. Declare and define

Declare injectable functions in a header:

```cpp
INJECT_DECLARE(bool, DownloadFile,
    (const char* Path, char* Content));
```

Define the wrapper in a source file:

```cpp
INJECT_DEFINE(bool, DownloadFile,
    (const char* Path, char* Content),
    (Path, Content));
```

For functions inside a namespace, use `INJECT_DEFINE_NS`.

## 5. Test usage

Override a function locally using a lambda:

```cpp
void Test_FileDownload_Error()
{
	INJECT_SET_LOCAL(DownloadFile, [](const char *Path, char *Content) 
	{ 
		return false; 
	});

	file File = GetFile("/test.txt");
	assert(File.Exists == false);
	assert(File.SizeBytes == 0);
}
```

This lets tests control function behavior without modifying production logic.
