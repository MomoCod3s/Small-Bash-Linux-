# Mini Bash
A tiny bash-like shell implemented in C++.

> **WSL required (Windows only):** This project must be built and run inside **Windows Subsystem for Linux (WSL 2)**. macOS/Linux users can use their native shell environment.

---

## Table of Contents
- [Prerequisites](#prerequisites)
- [Install WSL (Windows 10/11)](#install-wsl-windows-1011)
- [Linux dependencies](#linux-dependencies)
- [Getting the code](#getting-the-code)
- [Build](#build)
- [Run](#run)
- [Configuration](#configuration)
- [Troubleshooting](#troubleshooting)
- [Development tips](#development-tips)
- [License](#license)

---

## Prerequisites
- **Windows 10 (21H2+) or Windows 11** with **WSL 2**  
  - A Linux distro (e.g. **Ubuntu 22.04+**) installed in WSL
- Or: **macOS/Linux** with a recent compiler toolchain
- **C++17+** compiler (g++) and **CMake** (or Make)
- ~200 MB free space

---

## Install WSL (Windows 10/11)
Open **PowerShell as Administrator** and run:

```powershell
wsl --install
wsl --set-default-version 2
wsl --install -d Ubuntu

## Build 
g++ minishell.cpp -o minishell.exe

## Run
./minishell

