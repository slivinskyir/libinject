
#!/usr/bin/env python3

import argparse
import re
from pathlib import Path


def mask_comments_and_strings(source: str) -> str:
    """Mask comments and literals while preserving offsets and newlines."""
    pattern = re.compile(
        r'//[^\n]*'
        r'|/\*.*?\*/'
        r'|R"([^ ()\\\t\r\n]{0,16})\(.*?\)\1"'
        r'|"(?:\\.|[^"\\])*"'
        r"|'(?:\\.|[^'\\])*'",
        re.DOTALL,
    )

    def replace(match):
        text = match.group(0)
        return ''.join('\n' if c == '\n' else ' ' for c in text)

    return pattern.sub(replace, source)


def extract_includes(source: str):
    """Extract include directives while preserving their original text."""
    masked = mask_comments_and_strings(source)
    pattern = re.compile(
        r'^[ \t]*#[ \t]*include\b[^\n]*',
        re.MULTILINE,
    )

    return [
        source[match.start():match.end()].strip()
        for match in pattern.finditer(masked)
    ]


def namespace_openings(masked: str):
    """Map namespace opening braces to their namespace components."""
    pattern = re.compile(
        r'\bnamespace\s+'
        r'((?:[A-Za-z_]\w*\s*::\s*)*[A-Za-z_]\w*)'
        r'\s*\{'
    )

    openings = {}

    for match in pattern.finditer(masked):
        name = re.sub(r'\s+', '', match.group(1))
        openings[match.end() - 1] = name.split('::')

    return openings


def namespace_at(masked: str, position: int, openings) -> list[str]:
    """Return named namespaces enclosing the specified position."""
    brace_stack = []

    for i, char in enumerate(masked[:position]):
        if char == '{':
            brace_stack.append(openings.get(i))
        elif char == '}' and brace_stack:
            brace_stack.pop()

    result = []

    for components in brace_stack:
        if components:
            result.extend(components)

    return result


def extract_functions(source: str):
    """Yield (namespace_components, function_definition) for inject__."""
    masked = mask_comments_and_strings(source)
    openings = namespace_openings(masked)
    keyword = re.compile(r'\binject__\b')

    for match in keyword.finditer(masked):
        start = match.end()

        while start < len(masked) and masked[start].isspace():
            start += 1

        parens = 0
        brackets = 0
        body_start = None

        for i in range(match.end(), len(masked)):
            c = masked[i]

            if c == '(':
                parens += 1
            elif c == ')':
                parens -= 1
            elif c == '[':
                brackets += 1
            elif c == ']':
                brackets -= 1
            elif c == ';' and parens == 0 and brackets == 0:
                break
            elif c == '{' and parens == 0 and brackets == 0:
                body_start = i
                break

        if body_start is None:
            continue

        depth = 0

        for i in range(body_start, len(masked)):
            if masked[i] == '{':
                depth += 1
            elif masked[i] == '}':
                depth -= 1

                if depth == 0:
                    yield (
                        namespace_at(masked, match.start(), openings),
                        source[start:i + 1].strip(),
                    )
                    break


def wrap_in_inject_namespace(
    namespace_components: list[str],
    function: str,
) -> str:
    """Wrap the function in _inject and preserve its original namespace."""
    lines = ['namespace _inject {']

    for namespace in namespace_components:
        lines.append(f'namespace {namespace} {{')

    lines.append(function)

    for _ in reversed(namespace_components):
        lines.append('}')

    lines.append('}')

    return '\n'.join(lines)


def main():
    parser = argparse.ArgumentParser(
        description="Extract marked function definitions and their includes."
    )
    parser.add_argument("root", type=Path, help="Directory to scan")
    parser.add_argument(
        "-o",
        "--output",
        type=Path,
        default=Path("injected_functions.cpp"),
        help="Output .cpp file",
    )

    args = parser.parse_args()

    root = args.root.resolve()
    output = args.output.resolve()

    includes = set()
    functions = []

    extensions = (".cpp", ".c", ".cc", ".h", ".hh", ".hpp")

    for path in sorted(
        path
        for path in root.rglob("*")
        if path.is_file()
        and path.suffix in extensions
        and path.resolve() != output
    ):
        source = path.read_text(encoding="utf-8")
        extracted = list(extract_functions(source))

        if not extracted:
            continue

        includes.update(extract_includes(source))

        for namespaces, function in extracted:
            functions.append(
                f"// Source: {path.relative_to(root)}\n"
                f"{wrap_in_inject_namespace(namespaces, function)}"
            )

    output.parent.mkdir(parents=True, exist_ok=True)

    parts = [
        "// Generated file. Do not edit manually.",
        "",
        *sorted(includes),
        "",
        *functions,
        "",
    ]

    output.write_text("\n".join(parts), encoding="utf-8")

    print(
        f"Extracted {len(functions)} functions and "
        f"{len(includes)} unique includes into {output}"
    )


if __name__ == "__main__":
    main()
