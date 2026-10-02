# Third-party licenses and notices

This file identifies third-party software used by ShittyJukeBox (`sjb`), its song importer, and its optional OLED helper. The project's own license is recorded in [LICENSE.md](LICENSE.md). That license does not replace the licenses of its dependencies or grant rights to music, lyrics, or artwork.

The inventory is based on [Makefile](Makefile), [build.sh](build.sh), [oled_support/build.sh](oled_support/build.sh), and the source files, reviewed on 2026-10-02. Libraries are obtained from the build system's installed packages; their source trees are not vendored here. Versions and enabled features are not pinned, so a particular binary's dependency notices must match the packages used to build it.

## Main player: `sjb`

| Component | Use in this project | License and upstream reference |
| --- | --- | --- |
| FFmpeg: `libavformat`, `libavcodec`, `libavutil`, `libswresample`, `libswscale` | Audio input, decoding, resampling, downloads, and cover decoding/scaling | LGPL-2.1-or-later by default; optional features can change the applicable license. See [FFmpeg licensing](https://ffmpeg.org/legal.html) and the build-specific note below. |
| SDL 2 | Audio output | Zlib. [SDL license](https://www.libsdl.org/license.php). |
| SQLite 3 | Catalogs and playlists; also used by `jukebox-add` | Public domain dedication. [SQLite copyright statement](https://www.sqlite.org/copyright.html). |
| Lua 5.4 | Loading `config/theme.lua` | MIT. [Lua license](https://www.lua.org/license.html). |
| GLib, GObject, GIO | MPRIS/D-Bus integration and local-library utilities | LGPL-2.1-or-later, with additional notices for individual source files. [GLib license inventory](https://github.com/GNOME/glib/tree/main/LICENSES) and [COPYING](https://github.com/GNOME/glib/blob/main/COPYING). |

The Arch dependency path in `build.sh` installs `sdl2-compat`, an SDL 2 compatibility layer backed by SDL 3. Both use the Zlib license; preserve their separate notices when distributing those packages. See [sdl2-compat's license](https://github.com/libsdl-org/sdl2-compat/blob/main/LICENSE.txt) and [SDL 3's license](https://github.com/libsdl-org/SDL/blob/main/LICENSE.txt).

### FFmpeg build-specific licensing

FFmpeg's enabled components determine its effective license. GPL components enabled with `--enable-gpl`, version-3 components, and `--enable-nonfree` builds require separate attention; do not describe every FFmpeg build as LGPL-only. The FFmpeg executable in the development environment reported GPL version 3 or later during this review. This observation is not a license declaration for every installation of `sjb`. See [FFmpeg's upstream licensing explanation](https://ffmpeg.org/legal.html) and [LICENSE.md](https://github.com/FFmpeg/FFmpeg/blob/master/LICENSE.md).

For a binary release, inspect the actual linked FFmpeg libraries and their configuration. `ffmpeg -L` and `ffmpeg -buildconf` describe the installed executable and are useful when it comes from the same build as the libraries. Library APIs such as `avformat_license()` and `avformat_configuration()` describe the loaded library itself. Preserve the corresponding source-file notices and the applicable license texts from that build.

## Song importer: `jukebox-add`

| Component | Use in this project | License and upstream reference |
| --- | --- | --- |
| libcurl | HTTP requests in the song importer and Genius integration | curl license (SPDX identifier `curl`). [Copyright and permission notice](https://curl.se/docs/copyright.html). |
| libxml2 | Parsing imported HTML | MIT, with source-file-specific copyright notices. [Upstream Copyright](https://github.com/GNOME/libxml2/blob/master/Copyright). |
| SQLite 3 | Writing the catalog | Public domain; see the SQLite entry above. |

## Optional OLED helper: `apex_oled`

This helper is built separately by `oled_support/build.sh`; it is not linked into `sjb`.

| Component | Use in this project | License and upstream reference |
| --- | --- | --- |
| HIDAPI (`hidapi-libusb`) | Communicating with the keyboard's HID interface | Choice of GPL-3.0, a BSD-style license, or the original HIDAPI license. [Upstream license choices](https://github.com/libusb/hidapi/blob/master/LICENSE.txt). The BSD-style notice is reproduced below. |
| libusb | USB backend used by `hidapi-libusb` | LGPL-2.1-or-later. [Source notice](https://github.com/libusb/libusb/blob/master/libusb/libusb.h) and [COPYING](https://github.com/libusb/libusb/blob/master/COPYING). |
| playerctl | External command invoked to read MPRIS metadata | LGPL version 3 license text in upstream [COPYING](https://github.com/altdesktop/playerctl/blob/master/COPYING). This is a separate executable, not copied into the helper. |

## Included permissive license notices

The following notices were transcribed from installed upstream headers or package license files during the review. Copyright years can differ between releases; preserve the exact notices supplied with any version you redistribute. These excerpts do not replace additional notices in those distributions.

### SDL 2 — Zlib

Source: SDL 2 public header `SDL.h`; [upstream license](https://www.libsdl.org/license.php).

```text
Simple DirectMedia Layer
Copyright (C) 1997-2026 Sam Lantinga <slouken@libsdl.org>

This software is provided 'as-is', without any express or implied
warranty.  In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not
   claim that you wrote the original software. If you use this software
   in a product, an acknowledgment in the product documentation would be
   appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be
   misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
```

### Lua — MIT

Source: Lua 5.4 public header `lua.h`; [upstream license](https://www.lua.org/license.html).

```text
Copyright (C) 1994-2026 Lua.org, PUC-Rio.

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
```

### libcurl — curl license

Source: Installed `curl/COPYING`; [upstream notice](https://curl.se/docs/copyright.html).

```text
COPYRIGHT AND PERMISSION NOTICE

Copyright (c) 1996 - 2026, Daniel Stenberg, <daniel@haxx.se>, and many
contributors, see the THANKS file.

All rights reserved.

Permission to use, copy, modify, and distribute this software for any purpose
with or without fee is hereby granted, provided that the above copyright
notice and this permission notice appear in all copies.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT OF THIRD PARTY RIGHTS. IN
NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE
OR OTHER DEALINGS IN THE SOFTWARE.

Except as contained in this notice, the name of a copyright holder shall not
be used in advertising or otherwise to promote the sale, use or other dealings
in this Software without prior written authorization of the copyright holder.
```

### libxml2 — MIT

Source: Installed `libxml2/Copyright`; [upstream notice](https://github.com/GNOME/libxml2/blob/master/Copyright).

```text
Except where otherwise noted in the source code (e.g. the files dict.c and
list.c, which are covered by a similar licence but with different Copyright
notices) all the files are:

 Copyright (C) 1998-2012 Daniel Veillard.  All Rights Reserved.
 Copyright (C) The Libxml2 Contributors.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is fur-
nished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FIT-
NESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
```

### HIDAPI — BSD-style license option

Source: Installed `hidapi/LICENSE-bsd.txt`; [upstream notice](https://github.com/libusb/hidapi/blob/master/LICENSE-bsd.txt).

```text
Copyright (c) 2010, Alan Ott, Signal 11 Software
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice,
      this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of Signal 11 Software nor the names of its
      contributors may be used to endorse or promote products derived from
      this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.
```

## Copyleft license texts and binary distributions

The complete standard texts are available from the Free Software Foundation:

- [GNU LGPL version 2.1](https://www.gnu.org/licenses/old-licenses/lgpl-2.1.html).
- [GNU GPL version 2](https://www.gnu.org/licenses/old-licenses/gpl-2.0.html).
- [GNU LGPL version 3](https://www.gnu.org/licenses/lgpl-3.0.html).
- [GNU GPL version 3](https://www.gnu.org/licenses/gpl-3.0.html).

This dependency inventory is not a complete notice bundle for every possible binary build. When redistributing dependency binaries, include their applicable license texts, copyright notices, and corresponding source or source-access arrangements required by those licenses. A link to an upstream homepage alone does not replace those requirements. Preserve file-specific notices and account for libraries enabled by the selected builds, including TLS, codec, compression, and platform backends.

The build also uses platform C/math/thread libraries and external development tools. Their implementations and licenses depend on the host system; this repository does not supply copies of them.

## Embedded material and attribution status

- **OLED bitmap font:** the `font[95][5]` table in [oled_support/sjb_oled.c](oled_support/sjb_oled.c) has no recorded upstream source, author, or license in that file. Its provenance remains unverified. No third-party license is assigned to it here; its origin and permission need to be established before claiming a complete attribution inventory for the OLED helper.
- **FFT code:** [src/fft.h](src/fft.h) identifies the FFT as the author's own reused implementation. No separate third-party copyright notice was found in `src/fft.c` or `src/fft.h`; this document does not attribute it to an unrelated FFT library.
- **Terminal graphics:** [src/terminal_handler.h](src/terminal_handler.h) credits fastfetch as inspiration for the Kitty graphics approach. That comment identifies inspiration, not a specific copied source file. It is not evidence for assigning a fastfetch license to this implementation.

## Music, lyrics, artwork, and external services

Audio recordings, compositions, lyrics, album artwork, and other third-party content referenced by or stored in the catalog retain their respective rights. The project's software license and the library licenses above do not grant permission to redistribute that content.

Google Drive and Genius are external services used by links or importer code, not bundled software libraries. Their service terms and any rights in retrieved content are separate from this software dependency inventory.
