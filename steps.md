## Getting Started
1. Install MSYS (to install MinGW)
   ```bash
   https://www.msys2.org/
   ```
   Then in your MSYS terminal, write:
   ```bash
   pacman -S --needed base-devel mingw-w64-ucrt-x86_64-toolchain
   ```
   (If you get lost, follow [this](https://code.visualstudio.com/docs/cpp/config-mingw#_installing-the-mingww64-toolchain) guide)
3. Install Qt ( get aqt to not create an account)
   `aqt.exe install-qt windows desktop 6.10.0 win64_mingw -m all`
4. Install QtCreator
5. Install GNUPLOT : in MSYS console :`pacman -S mingw-w64-x86_64-gnuplot`
5. Configure the project (point to your gcc and qt installations)
6. **Scrap that shit, use Makefiles like a real programmer**
7. Change line 44 and 45 IRM2D.cpp to <filesystem> only, and 
8. replace line 155 of irm_train_sgb.cpp and line 82  of IRM2D.cpp to an updated shuffle
   ```cpp
   std::random_device rd;
   std::mt19937 g(rd());
   std::shuffle(Dataset.begin(), Dataset.end(), g); // or std::shuffle(indexes.begin(), indexes.end(), g);
   ```

## Prise en main
1. Modify lines 16 or 18 in irm/irm_mlp_test.cpp to your file path
2. Compile tha shi (easy thanks to Makefiles)
   ```bash
   make
   ```
3. run `irm_mlp_test.exe best/irm2d_adam_20000.bin` 
