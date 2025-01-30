# KDailyNote TODO

When you make a change that implements a TODO, remember to also change this file to check it off!

## MVP
- [x] Implement markdown save/load (translate to/from text segments)
- [x] Autosave
- [x] Format bar (MVP bold, italic, underline)
- [x] Add date header protection (prevent editing/deleting the timestamp itself)
- [x] Pretty up the date header
- [x] Implement proper window positioning relative to system tray
- [x] Close on focus loss.
- [x] Add README.md
- [x] Add LICENSE (GPL3)

## Improvements
- [x] Context menu for tray icon
- [x] Always scroll to bottom when opening
- [ ] Implement proper theming integration with KDE
- [ ] Make double sure we scroll the view to the bottom when opened
- [ ] Add visual feedback for current format state (B icon toggled when in bold mode, etc)
- [ ] Delete day when backspacing while textbox is empty
- [ ] Configurable global shortcut key
- [x] Add keyboard shortcuts for formatting (Ctrl+B, Ctrl+I, etc.)
- [ ] Recognize Markdown formatting commands as they are entered
    (for instance, *italic*<-format italic when * is pressed)
- [ ] Allow reversing shown day order
- [ ] Numbered lists, bullet lists
- [ ] Implement search functionality (Ctrl+F)
- [ ] Day summary/title
- [ ] Whatever needs doing to get KDailyNote as a kde app on make install

## Technical Debt
- [ ] Implement error handling for file operations (disk full, bad format)
- [ ] Add visual tests
- [ ] Review memory management
- [ ] Handle large files efficiently (virtual scroll)

## Nice to Have
- [ ] Multiple files
- [ ] Calendar view
