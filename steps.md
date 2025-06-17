## Getting Started
1. Install MSYS (to install MinGW)
   ```https://www.msys2.org/```
   Then in your MSYS terminal, write:
   ```pacman -S --needed base-devel mingw-w64-ucrt-x86_64-toolchain```
   (If you get lost, follow [this](https://code.visualstudio.com/docs/cpp/config-mingw#_installing-the-mingww64-toolchain) guide)
3. Install Qt ( get aqt to not create an account)
   `aqt.exe install-qt windows desktop 6.10.0 win64_mingw -m all`
4. Install QtCreator
5. Configure the project (point to your gcc and qt installations)
