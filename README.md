<div align="center">

**グロートピア** *(Gurotopia)* : Lightweight & Maintained GTPS written in C/C++

[![](https://github.com/GT-api/GT.api/actions/workflows/make.yml/badge.svg)](https://github.com/GT-api/GT.api/actions/workflows/make.yml)
[![Dockerfile](https://github.com/gurotopia/Gurotopia/actions/workflows/docker.yml/badge.svg)](https://github.com/gurotopia/Gurotopia/actions/workflows/docker.yml)
[![](https://app.codacy.com/project/badge/Grade/fa8603d6ec2b4485b8e24817ef23ca21)](https://app.codacy.com/gh/gurotopia/Gurotopia/dashboard?utm_source=gh&utm_medium=referral&utm_content=&utm_campaign=Badge_grade)
[![](https://dcbadge.limes.pink/api/server/zzWHgzaF7J?style=flat)](https://discord.gg/zzWHgzaF7J)

</div>

***

# <img width="190" height="50" alt="Windows_logo_and_wordmark_-_2021 svg" src="https://github.com/user-attachments/assets/1385f762-2c56-465a-aa3b-901a431552bb" />

#### ![](https://raw.githubusercontent.com/microsoft/vscode-icons/main/icons/dark/archive.svg) 1. Install Dependencies
   - download [**MSYS2**](https://www.msys2.org/) and [**Visual Studio Code**](https://code.visualstudio.com/): install [C/C++ extension](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cpptools) for VSC (**Required**)
     
   - Locate your MSYS2 folder (e.g., `C:\msys64`), open `ucrt64.exe`, and run the following command:
     ```bash
     pacman -S --needed mingw-w64-ucrt-x86_64-{gcc,openssl,sqlite} make
     ```

#### ![](https://raw.githubusercontent.com/microsoft/vscode-icons/main/icons/dark/build.svg) 2. Compile
   - Open the project folder in Visual Studio Code.
   - Press **`Ctrl + Shift + B`** to start the build process.

#### ![](https://raw.githubusercontent.com/microsoft/vscode-icons/main/icons/dark/debug-alt-small.svg) 3. Run/Debug
   - After compiling, press **`F5`** to run the server with the debugger attached.

# <img src="https://github.com/user-attachments/assets/fecde323-04c5-4b82-a08d-badcb184be6a" width="30" /> Linux

#### ![](https://raw.githubusercontent.com/microsoft/vscode-icons/main/icons/dark/archive.svg) 1. Install Dependencies

- enter command associated with your distribution into the terminal to install nessesary tools.
   <details><summary><img width="18" height="18" src="https://github.com/user-attachments/assets/8359ba6e-a9b2-4500-893f-61eaf40e2478" /> Arch</summary>
   <p>
      
   ```bash
   sudo pacman -S base-devel openssl sqlite
   ```
   </p>
   </details> 
   <details><summary><img width="18" height="18" src="https://github.com/user-attachments/assets/742f35c4-3e69-450e-8095-9fabe9ecd0d8" /> Debian <img width="18" height="18" src="https://github.com/user-attachments/assets/46f0770e-f4ed-480b-851d-c90b05fae52f" /> Ubuntu</summary>
   <p>
      
   ```bash
   sudo apt-get update && sudo apt-get install build-essential libssl-dev openssl sqlite3
   ```
        
   </p>
   </details> 

#### ![](https://raw.githubusercontent.com/microsoft/vscode-icons/main/icons/dark/build.svg) 2. Compile
   - Navigate to the project's root directory in your terminal and run the `make` command:
     ```bash
     make -j$(nproc)
     ```

#### ![](https://raw.githubusercontent.com/microsoft/vscode-icons/main/icons/dark/debug-alt-small.svg) 3. Run
   - Execute the compiled binary located in the `main` directory:
     ```bash
     ./main.out
     ```

---

### ⚙️ How to Connect to Your Local Server

To test your server, you need to redirect the Growtopia game client on your computer to connect to your local machine instead of the official game servers. This is done by editing the `hosts` file on your system.

> [!WARNING]
> Editing the `hosts` file requires administrator privileges. Remember to revert these changes when you want to play on the official Growtopia servers again!

**Step 1: Open the `hosts` file with administrator rights.**

*   **Windows:**
    1.  Search for "Notepad" in the Start Menu, right-click it, and select "Run as administrator".
    2.  In Notepad, go to `File > Open`.
    3.  Navigate to `C:\Windows\System32\drivers\etc`.
    4.  Change the file type filter from "Text Documents (*.txt)" to "All Files (*.*)".
    5.  Select the `hosts` file and click "Open".

*   **Linux/macOS:**
    1.  Open a terminal.
    2.  Run the command: `sudo nano /etc/hosts` (or use another text editor like `vim` or `gedit`).

**Step 2: Add the server address lines.**

Add the following two lines to the very end of the `hosts` file. Do not change anything else.

```
127.0.0.1 www.growtopia1.com
127.0.0.1 www.growtopia2.com
```

**Step 3: Save the file and launch the game.**

Save your changes to the `hosts` file. Now, simply run Growtopia. The game will automatically connect to your local server running on your machine.

**How to Revert (to play on official servers):**

To play on the official servers again, simply open the `hosts` file (with administrator rights) and delete the two lines you added, or comment them out by adding a `#` symbol at the beginning of each line, like this:

```
# 127.0.0.1 www.growtopia1.com
# 127.0.0.1 www.growtopia2.com
```
