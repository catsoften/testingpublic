TPT Record Mod - v3.1415926535+v97.0
==========================

This mod replaces the vanilla tpt.record() lua function with a customizable recording GUI. The menu can be accessed by clicking the R button below the console button, or by using the R shortcut key.

Supported Formats:
* Gif
* WebP
* Old (same as original .ppm spam recordings)

Other Features:
* Selectable recording area
* Adjustable FPS
* Pixel scaling + zoom window imitation
* Buffering to ram or disk to improve game performance
* Multithreaded writing
* Pausing
* Built-in help menu
* Lua API for controlling all settings

For download links, screenshots, and other information see the [forum post](https://tpt.io/.323236).

Additional build instructions
===========================================================================

This mod requires libwebp. It can be downloaded [here](https://developers.google.com/speed/webp/download), and the contents of the lib/ directory (three .a or .lib files) should be extracted to a new directory named libwebp/ at the repository root (same location as README.md).

Additional libraries used
===========================================================================

* [msf_gif](https://github.com/notnullnotvoid/msf_gif)
* [libwebp](https://developers.google.com/speed/webp/download)

Additional controls
===========================================================================

| Key | Action           |
| --- | ---------------- |
| R   | Open record menu |

More Information
===========================================================================

This is a mod for [The Powder Toy](https://github.com/The-Powder-Toy/The-Powder-Toy). Go there for the full readme and documentation.
