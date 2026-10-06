#!/usr/bin/env bash
set -euo pipefail

# ============================================================
# Apex 5 OLED – setup + diagnostic helper
# ============================================================

VENDOR="1038"
PRODUCT="161c"
SRC="sjb_oled.c"
BIN="apex_oled"
UDEV_RULE="/etc/udev/rules.d/99-steelseries-apex.rules"
PLAYER="ShittyJukeBox"

RED='\033[0;31m'
GRN='\033[0;32m'
YLW='\033[1;33m'
NC='\033[0m'

ok()   { echo -e "${GRN}[OK]${NC} $*"; }
warn() { echo -e "${YLW}[!!]${NC} $*"; }
err()  { echo -e "${RED}[ERR]${NC} $*"; }

# ---------- 1. Check source ----------
if [[ ! -f "$SRC" ]]; then
    err "Source file '$SRC' not found in $(pwd)"
    exit 1
fi
ok "Found $SRC"

if ! pkg-config --exists gio-2.0; then
    err "GLib/GIO development files and pkg-config are required for UTF-8 text support"
    exit 1
fi

# ---------- 2. Check playerctl ----------
if ! command -v playerctl &>/dev/null; then
    warn "playerctl not installed; skipping the optional metadata diagnostic"
else
    ok "playerctl present"
    echo "--- playerctl test ---"
    if playerctl -p "$PLAYER" metadata --format "{{artist}}||{{title}}" 2>/dev/null; then
        ok "Metadata readable from $PLAYER"
    else
        warn "Could not read metadata from $PLAYER (is it playing?)"
    fi
    echo
fi

# ---------- 3. Check device present ----------
if lsusb | grep -qi "${VENDOR}:${PRODUCT}"; then
    ok "Apex 5 detected (1038:161c)"
else
    err "Apex 5 not found in lsusb"
    lsusb | grep -i steelseries || true
    exit 1
fi

# ---------- 4. Ensure plugdev group exists ----------
echo
echo "=== Ensuring plugdev group exists ==="
if getent group plugdev >/dev/null 2>&1; then
    ok "Group 'plugdev' already exists"
else
    echo "Creating group 'plugdev'..."
    sudo groupadd plugdev
    ok "Created group 'plugdev'"
fi

# ---------- 5. Add user to plugdev ----------
if id -nG "$USER" | grep -qw plugdev; then
    ok "User $USER already in plugdev"
else
    echo "Adding $USER to plugdev..."
    sudo usermod -aG plugdev "$USER"
    warn "You must log out and log back in (or reboot) for group change to take effect"
fi

# ---------- 6. Install udev rule ----------
echo
echo "=== Installing udev rule (needs sudo) ==="
sudo tee "$UDEV_RULE" > /dev/null << EOF
# SteelSeries Apex 5 OLED
SUBSYSTEM=="usb", ATTRS{idVendor}=="${VENDOR}", ATTRS{idProduct}=="${PRODUCT}", MODE="0666", GROUP="plugdev"
KERNEL=="hidraw*", ATTRS{idVendor}=="${VENDOR}", ATTRS{idProduct}=="${PRODUCT}", MODE="0666", GROUP="plugdev"
SUBSYSTEM=="hidraw", ATTRS{idVendor}=="${VENDOR}", MODE="0666", GROUP="plugdev"
EOF
ok "Wrote $UDEV_RULE"

sudo udevadm control --reload-rules
sudo udevadm trigger
ok "udev rules reloaded"

# ---------- 7. Compile ----------
echo
echo "=== Compiling ==="
if pkg-config --exists hidapi-libusb 2>/dev/null; then
    cc "$SRC" -o "$BIN" $(pkg-config --cflags --libs hidapi-libusb gio-2.0)
else
    cc "$SRC" -o "$BIN" -lhidapi-libusb $(pkg-config --cflags --libs gio-2.0)
fi
ok "Built ./$BIN"

# ---------- 8. Permission check on hidraw ----------
echo
echo "=== hidraw nodes for SteelSeries ==="
found=0
for node in /dev/hidraw*; do
    [[ -e "$node" ]] || continue
    sys="/sys/class/hidraw/$(basename "$node")/device"
    if [[ -f "$sys/uevent" ]] && grep -qi "1038" "$sys/uevent" 2>/dev/null; then
        ls -l "$node"
        found=1
    fi
done
if [[ $found -eq 0 ]]; then
    warn "No matching hidraw node found yet – unplug/replug the keyboard"
fi
