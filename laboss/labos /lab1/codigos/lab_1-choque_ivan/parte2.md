Part 2: TIVAWare SDK Installation
1. Explain the TIVAWare SDK installation on your computer step by step. Address the specific details. Did you have any issues during the installation? If so, how did you manage to solve them?

Installing the TivaWare SDK and flashing tools required adapting the original guide to a modern Ubuntu environment, as some compatibility issues and missing dependencies arose. The step-by-step process and solutions applied are detailed below:
Step 1: SDK Download The package corresponding to the board model was downloaded from the Texas Instruments website. The resulting file was SW-EK-TM4C1294XL-2.2.0.295.exe.

Step 2: Installing the ARM Compiler (Problem and Solution)
• Problem: When trying to add the repository suggested in the guide (sudo add-apt-repository ppa:team-gcc-arm-embedded/ppa), the Ubuntu system (Noble version) threw an error indicating that the repository did not have a secure release file. This is because the PPA is outdated for recent Linux versions.

• • Solution: The PPA was omitted, and the compiler was installed directly from the official Ubuntu repositories using the following command: `sudo apt-get install gcc-arm-none-eabi`

Step 3: Cloning the flashing tools (lm4tools) The Git version control system was installed, and the lm4tools repository was cloned: `sudo apt install gitgit clone https://github.com/utzig/lm4tools.git`

Step 4: Compiling lm4flash (Problems and Solutions) When accessing the lm4tools/lm4flash folder to compile the program, two sequential errors occurred:

• Problem 1: The `make` command was not found because it was not pre-installed on the system.

• Solution: It was installed with `sudo apt install make`.

• • Problem 2: Upon re-running `make`, the system indicated that the C compiler (cc) and the libusb-1.0 library were missing.

• Solution: The essential build tools and required USB libraries were installed using the command: `sudo apt install build-essential pkg-config libusb-1.0-0-dev`
• Once the dependencies were resolved, the `make` command compiled the C source code without errors.

Step 5: USB Installation and Permission Configuration The compiled program was copied to the system's binary directory, and the udev rules were configured to allow communication with the board via the USB port without requiring superuser privileges: `sudo cp lm4flash /usr/local/binecho 'ATTRS{idVendor}=="1cbe", ATTRS{idProduct}=="00fd", GROUP="users", MODE="0660"' |` sudo tee /etc/udev/rules.d/99-stellaris-launchpad.rules
Step 6: SDK Decompression Finally, a folder called Tivaware was created in the root directory, the file downloaded in Step 1 was moved there, and it was decompressed using the terminal. Afterward, the original installer was removed to free up space: mkdir Tivawarecd Tivawaremv ~/Downloads/SW-EK-TM4C1294XL-2.2.0.295.exe .unzip SW-EK-TM4C1294XL-2.2.0.295.exerm SW-EK-TM4C1294XL-2.2.0.295.exe

2. What is a make file? Why is it important? Explain its use.

A Makefile is a special text file that contains a set of directives and rules used by the make tool on Unix/Linux-based systems. Its main purpose is to automate the compilation and build process of a software project.

Why is it important? It's crucial because it saves time and minimizes human error. In large projects, manually compiling each source code file by typing long commands in the terminal (e.g., `gcc -Wall -I /usr/include...`) is inefficient. Furthermore, `make` is intelligent: if you modify a single file in your project, the Makefile tells the system to only recompile that modified file and not the entire project, drastically optimizing development time.

How is it used? It's used by defining "targets," "dependencies" (which files are needed to create that target), and "commands" (the exact compilation instructions). By simply running the word `make` in the terminal (as we did with `lm4tools`), the system reads the Makefile from that folder and automatically executes all the necessary commands in the correct order to transform the source code into an executable program.