## Cubes

simple falling blocks puzzle game using X11 windowing system

### Game Controles

*   `ArrowUp`: Rotate cube clockwise
*   `ArrowDown`: Speed up falling of cube
*   `ArrowLeft`: Move cube to the left
*   `ArrowRight`: Move cube to the right
*   `Ctrl`: Rotate cube counterclockwise
*   `P`: Pause game
*   `Space`: Drop the cube instantly

## Building

Navigate to the project directory and run the following command:

```bash
make
```

This command will compile the game. Ensure that the following dependencies are installed:

*   **GCC**: Required for compiling C code with strict compliance to `C99`. The build process leverages `-Wall`, `-Werror`, `-Wextra`, and `-Wpedantic` flags to enforce code quality and adherence to standards.
*   **Xlib**: Essential for interacting with the X11 windowing system. The game uses X11 for window management, event handling, and graphical rendering.
*   **FreeType/Xft**: Integrated for advanced font rendering. `Xft` provides anti-aliased text drawing using FreeType, critical for rendering game text. Ensure `freetype2` is correctly included in the compile path with `-I/usr/include/freetype2`.

### Build Configurations

*   **Release Build**: Optimized with `-O3`, stripped binary for reduced size. Command: `make release`. (default)
*   **Debug Build**: Includes debugging symbols and `DDEBUG` macro. Command: `make debug`.

### Cleaning Up

To remove build artifacts, run:

```bash
make clean
```

### Running the Game

After building, the game can be executed with:

```bash
make run
```
or 
```bash
./bin/Cubes
```

## License

This project is licensed under the GNU General Public License v3.0.

## Contributions

Contributions are welcome! If you find any issues or have suggestions for improvements, please open an issue or submit a pull request.