# Contributing Guidelines

Thank you for your interest in contributing to **Qt Serial Communication**! We welcome bug reports, feature suggestions, and code contributions.

---

## Code of Conduct

Please maintain a respectful, constructive, and collaborative tone in all discussions, issues, and pull requests.

---

## How to Contribute

### 1. Reporting Issues

Before opening a new issue:
- Check existing issues to see if the problem or feature request has already been filed.
- When opening an issue, provide:
  - Operating system and Qt version.
  - Connected serial hardware or emulator setup.
  - Clear steps to reproduce the issue.
  - Expected vs. actual behavior.

### 2. Proposing Features

- Open an issue labeled `enhancement` or use the Feature Request template.
- Describe the use case and why the proposed feature benefits the project.

### 3. Code Contributions

#### Development Workflow

1. **Fork and Clone**:
   ```bash
   git clone https://github.com/<your-username>/qt-serial-communication.git
   cd qt-serial-communication
   ```

2. **Create a Feature Branch**:
   ```bash
   git checkout -b feature/my-new-feature
   ```

3. **Coding Standards**:
   - **Language**: Modern C++ (C++17 standard).
   - **Qt Conventions**: Follow standard Qt naming conventions:
     - PascalCase for class names (`SerialCommunication`).
     - camelCase for functions and methods (`connectPort()`, `sendCommand()`).
     - `m_` prefix for private member variables (`m_serial`, `m_config`).
     - Signals and slots should have descriptive names and clear parameters.
   - **Header Guards**: Use `#ifndef CLASSNAME_H` `#define CLASSNAME_H` `#endif`.
   - **Clean Architecture**: Keep the presentation layer (`src/ui`) decoupled from the serial engine (`src/communication`) and logging layer (`src/logging`).

4. **Verify Build**:
   Ensure the project builds cleanly without warnings:
   ```bash
   cmake -B build -S . -DCMAKE_BUILD_TYPE=Release
   cmake --build build
   ```

5. **Commit Message Guidelines**:
   Use [Conventional Commits](https://www.conventionalcommits.org/):
   - `feat: add support for custom flow control settings`
   - `fix: resolve crash on sudden USB unplug event`
   - `docs: update build instructions for Ubuntu`
   - `refactor: streamline terminal buffer formatting`

6. **Submit a Pull Request**:
   - Push your branch to GitHub.
   - Open a Pull Request against the `main` branch.
   - Describe your changes and reference any related issues.
