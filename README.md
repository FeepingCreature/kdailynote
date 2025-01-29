# KDailyNote

A simple diary that lives in your system tray.

Contents are autosaved at `~/.local/share/kdailynote/diary.md`.

## AI Notice

This project was created by Claude 3.5 Sonnet (Anthropic). Thanks Claude!

## Requirements

- Qt 6.4 or later
- KDE Frameworks 6.0 or later

## Building

```bash
cmake -B build -S .
make -C build
```

## Running

After building, run:
```bash
./build/bin/kdailynote
```

The application will appear in your system tray. Click the icon to open the editor.

## Usage

- Click the tray icon to show/hide the editor
- Use the toolbar buttons or keyboard shortcuts (Ctrl+B, Ctrl+I, Ctrl+U) for formatting
- Window hides on focus loss.
