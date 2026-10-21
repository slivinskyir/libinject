
# C++ Function Injection

## 0. The Problem

You have production code that depends on other functions, and you want to unit-test it without calling their real implementations.
For example, a function may download a file, access a database, or communicate with another service. These dependencies can make tests slow, unreliable, or difficult to reproduce.

```cpp
bool DownloadFile(const char* Path, char* Content)
{
    // Impl ... 
}

bool GetFile()
{
   if (DownloadFile("", 0))
   {
      // ...
   {
}
```
Function injection lets you replace a dependency with a simple lambda, so you can test your production logic in isolation without changing its behavior.


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


## 6. Example overview

Consider a function that downloads a file and a function that processes its result.

**Production code:**

```cpp
inject__ bool DownloadFile(const char* Path, char* Content)
{
    std::cout << "MAIN\n";
    return false;
}

file GetFile(const char* Path)
{
    file File = {};

    bool Downloaded = DownloadFile(Path, File.Content);
    if (Downloaded)
    {
        File.Exists = true;
        File.SizeBytes = strlen(File.Content);
    }

    return File;
}
```

**Test:**

```cpp
void Test_FileDownload_Error()
{
    INJECT_SET_LOCAL(
        DownloadFile,
        [](const char* Path, char* Content)
        {
            return false;
        });

    file File = GetFile("/test.txt");

    assert(File.Exists == false);
    assert(File.SizeBytes == 0);
}
```

The test replaces `DownloadFile` with a lambda that simulates a failed download. `GetFile` executes its normal production logic, allowing the test to verify the error-handling behavior without performing a real download.

## 7. Build and run

Generate the default implementations for all functions marked with `inject__`:

```bash
python3 inject.py . -o injected_functions.cpp
```

Compile the test executable using Clang++:

```bash
clang++ -O0 -std=c++17 -DINJECT_AVAILABLE \
    app.cpp \
    injected_functions.cpp \
    tests.cpp
```
