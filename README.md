# cryptoscript

A simple domain language designed to express classic cipher operations.

## Overview

**cryptoscript** is a domain-specific language (DSL) that provides an intuitive, expressive syntax for working with classical cryptography. It allows users to define and execute cipher operations without diving deep into implementation details, making it ideal for:

- Educational purposes in cryptography
- Quick prototyping of cipher algorithms
- Implementing classical encryption techniques (Caesar, Vigenère, Substitution, etc.)
- Demonstrating cipher mechanics and transformations

## Features

- **Simple Syntax**: Easy-to-understand language for defining cipher operations
- **Classical Ciphers**: Support for traditional encryption methods
- **Composable Operations**: Chain multiple cipher operations together
- **Educational Focus**: Learn cryptography concepts through practical implementation

## Installation

```bash
git clone https://github.com/mind-wrapper/cryptoscript.git
cd cryptoscript
```

## Quick Start

### Basic Usage

```
// Example cipher operation
cipher = caesar(shift: 3)
plaintext = "hello world"
ciphertext = cipher.encrypt(plaintext)
```

## Supported Ciphers

- **Caesar Cipher**: Character shift-based encryption
- **Vigenère Cipher**: Polyalphabetic substitution cipher
- **Substitution Cipher**: Character-to-character mapping
- **Atbash Cipher**: Letter reversal cipher
- **ROT13**: Rotate by 13 positions

## Syntax Reference

### Defining a Cipher

```
cipher = <cipher_type>(parameters)
```

### Encryption

```
result = cipher.encrypt(text)
```

### Decryption

```
original = cipher.decrypt(ciphertext)
```

### Chaining Operations

```
result = cipher1.encrypt(text) | cipher2.encrypt
```

## Examples

### Caesar Cipher

```
// Create a Caesar cipher with shift of 3
my_cipher = caesar(shift: 3)

// Encrypt a message
encrypted = my_cipher.encrypt("attack at dawn")
// Output: "dwwdfn dw gdzq"

// Decrypt a message
decrypted = my_cipher.decrypt("dwwdfn dw gdzq")
// Output: "attack at dawn"
```

### Vigenère Cipher

```
// Create a Vigenère cipher with a key
vig = vigenere(key: "SECRET")

encrypted = vig.encrypt("the quick brown fox")
decrypted = vig.decrypt(encrypted)
```

### ROT13

```
rot = rot13()
result = rot.encrypt("hello")
// Output: "uryyb"
```

## Language Specification

### Comments

```
// Single-line comment
/* Multi-line comment */
```

### Variables

```
key = "mykey"
text = "plaintext"
cipher = caesar(shift: 5)
```

### Operations

- **Encryption**: `cipher.encrypt(text)`
- **Decryption**: `cipher.decrypt(text)`
- **Analysis**: `cipher.analyze(text)` - Statistics about cipher
- **Key Generation**: `generate_key(length)` - Generate random key

## Architecture

```
src/
  ├── lexer/        Tokenizes input source code
  ├── parser/       Parses tokens into AST
  ├── evaluator/    Executes AST nodes
  ├── ciphers/      Classical cipher implementations
  └── builtins/     Built-in functions and utilities

tests/              Unit and integration tests
docs/               Extended documentation
examples/           Example cryptoscript programs
```

## Contributing

Contributions are welcome! Please feel free to submit pull requests or open issues for bugs and feature requests.

## License

This project is licensed under the GNU General Public License v3.0 - see the [LICENSE](LICENSE) file for details.

## Educational Use

This project is designed for educational purposes. While cryptoscript implements classical ciphers correctly, **do not use these ciphers for production security**. Classical ciphers are cryptographically broken and should only be used for learning and historical context.

## References

- [Classical Cryptography](https://en.wikipedia.org/wiki/Classical_cryptography)
- [Caesar Cipher](https://en.wikipedia.org/wiki/Caesar_cipher)
- [Vigenère Cipher](https://en.wikipedia.org/wiki/Vigen%C3%A8re_cipher)

## Roadmap

- [ ] Core language implementation
- [ ] Lexer and parser
- [ ] Basic cipher support (Caesar, ROT13)
- [ ] Extended cipher support (Vigenère, Substitution)
- [ ] REPL interactive console
- [ ] Standard library
- [ ] Comprehensive test suite
- [ ] Documentation and tutorials

## Support

For questions or issues, please open a GitHub issue on the [project repository](https://github.com/mind-wrapper/cryptoscript/issues).

---

Made with ❤️ for cryptography enthusiasts and learners.
