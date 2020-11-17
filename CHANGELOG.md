## v2.4.3
- Add window.SetTooltipFont. Since it's not permissable to call window.CreateTooltip more than once in a panel, this method has been added for changing the tooltip font without reloading the panel.

## v2.4.2
- Internal changes only, no new features.

## v2.4.1.2
- Fix `Thumbs` sample so [Last.fm](https://last.fm) artist art downloads work again.

## v2.4.1.1
- `utils.ListFolders` now has a `recursive` mode. Like `utils.ListFiles`, the default is `false`.

## v2.4.1
- Now requires `foobar2000` `v1.5` or later.
- All album art methods and `gdi.Image` / `gdi.LoadImageAsync` now have full `WebP` support.
- Add `plman.RemovePlaylists`. Accepts an array like `[1,3,4]`.
- Update docs to make it clear that only a single call to `window.CreateTooltip` is permitted per panel instance. Errors will now be thrown on any subsequent calls. Additionally, the `ITooltip` interface no longer has a `Dispose` method.
- `%fb2k_path%` is no longer expanded when used in the `PREPROCESSOR`. Use `%fb2k_profile_path%`.
- The default editor font is now `Consolas`. Existing users won't see this change unless they edit the font names under `File>Preferences>Tools>JScript Panel`. Choosing a new `Preset` or using `Reset page` will also use the new default.
- Remove `ListenBrainz` sample.
- Various `JS-Smooth` sample fixes.

## v2.4.0
- Fix crash when using `utils.CheckFont` on `foobar2000` `v1.6`. This affected many of the included scripts. Apologies for any inconvenience.
- Now requires `Windows 7` or later.

## v2.3.6.1 [released by kbuffington aka MordredKLB]
- Fix bug where `on_focus` was not always triggered.

## v2.3.6 [released by kbuffington aka MordredKLB]
- Add `IGdiBitmap` `InvertColours`.
- Add `clear_properties` option to `window.Reload`.

## v2.3.5
- Add `plman.GetPlaylistLockFilterMask`. It's recommended that you replace any usage of `plman.IsPlaylistLocked` with this as you can now determine the type of locks in place.
- Add `plman.GetPlaylistLockName`.
- Restore compatibility with `foo_popup_panels` when used from within `Default UI`. This was broken in `v2.3.4`.
- `fb.CheckClipboardContents` and `fb.GetClipboardContents` no longer require a `window_id` parameter. If supplied, it will be silently ignored.
- Minor sample fixes.

## v2.3.4
- The size and position of the `Properties Window` is now remembered. In addition, the size and position of the `Configuration Window` is now shared as a global setting rather than per panel instance like it was previously.
- `on_size` calls are now suppressed when the panel width or height is zero.
- Improve behaviour of `Pseudo Transparent` mode to be like older versions. Some changes were made during the `2.2.x` series that made panels not configured with the `Panel Stack Splitter` `Forced layout` option glitch more than they should.
- The project is now [MIT licensed](https://github.com/marc2k3/foo_jscript_panel/blob/develop/LICENSE.md).

## v2.3.3.1
- Add `utils.ReadUTF8`. It's preferable to use this when you know the file is `UTF8` encoded... such as ones written by `utils.WriteTextFile`. Continue to use `utils.ReadTextFile` if the files are `UCS2-LE`, `ANSI` or unknown.

## v2.3.3
- Fix rare crash with `utils.ReadTextFile`.

## v2.3.2
- Add `IMetadbHandleList` `OptimiseFileLayout`. Also takes an optional `minimise` argument. With `minimise` set to `false`, provides the functionality of `Utilities>Optimize file layout` or if `minimise` is `true` then `Utilities>Optimize file layout + minimize file size`. Unlike the context menu versions, there is no prompt.
- Properly fix `Return`/`Tab` key usage in the `Find`/`Replace` dialog.

## v2.3.1
- Fix a nasty editor bug that caused the component to crash while typing. Versions `2.3.0`-`2.3.0.2` were all affected so upgrading immediately is highly recommended!

## v2.3.0.2
- `Find`/`Replace` dialog fixes.
- Fix `Thumbs` sample so [Last.fm](https://last.fm) artist art downloads work again.
- Restore `Clear` button to the `Properties` dialog.

## v2.3.0.1
- Make `utils.ColourPicker` remember `Custom colours` for the lifetime of `foobar2000` being open.

## v2.3.0
- Drop support for `foobar2000` `v1.3.x`. Now requires `v1.4` or later.
- Add `utils.DateStringFromTimestamp` and `utils.TimestampFromDateString`.
- The `Properties` dialog has been rewritten and there is no longer any `Clear` or `Delete` buttons. Now you can multi-select using your mouse and combinations of `Ctrl`/`Shift` keys. There is a right click menu to `Select All`/`Select None`/`Invert selection` and `Remove`. `Ctrl+A` and the `Delete` keyboard shortcuts are also supported.
- The list view under `File>Preferences>Tools>JScript Panel` now supports inline editing from a single click on the value. You no longer double click items to open a new dialog.
- The following methods no longer support the previously optional `force` parameter.

```js
window.Repaint();
window.RepaintRect(x, y, w, h);
ITitleFormat Eval() // returns an empty string when not playing
```

- The following methods no longer support the previously optional `flags` parameter. All commands are ran if they exist. It no longer matters if they are hidden or not.

```js
fb.RunContextCommand(command);
fb.RunContextCommandWithMetadb(command, handle_or_handle_list);
```

- These previously optional parameters have been removed:

```
IContextMenuManager BuildMenu "max_id"
IMainMenuManager BuildMenu "count"
utils.CheckComponent "is_dll"
utils.WriteTextFile "write_bom" (writing a BOM is no longer supported)
```

- `plman.PlayingPlaylist` is now a read-only property.
- The `IPlaylistRecyclerManager` interface has been removed and replaced with fixed `plman` methods. All previous functionality remains. Look at the docs for `plman.RecyclerCount`.
- `fb.GetLibraryRelativePath` has been removed. Use `IMetadbHandleList` `GetLibraryRelativePaths` instead.
- `utils.FileTest` has been removed and replaced with the following 4 new methods. There is no replacement for `split` mode.

```js
utils.Chardet(filename)
utils.GetFileSize(filename)
utils.IsFile(filename)
utils.IsFolder(folder)
```

- The `Grab focus` checkbox in the [Configuration Window](https://github.com/marc2k3/foo_jscript_panel/wiki/Configuration-Window) and the `dragdrop` `PREPROCESSOR` have both been removed. Both features are disabled by default but are automatically enabled if relevant callbacks like `on_drag_drop` or `on_key_up` are present in the script.

## v2.2.2.4
- Fix bug introduced in `v2.2.2.1` which prevented images loaded with `gdi.Image` / `gdi.LoadImageAsync` being deleted/overwritten after `Dispose()` had been called.

## v2.2.2.3
- Fix `IMetadbHandleList` `MakeDifference` method.

## v2.2.2.2
- Fix broken descriptions for main menu items in the keyboard shortcut preferences. Thanks to [TheQwertiest](https://github.com/TheQwertiest) for spotting and providing the fix.

## v2.2.2.1
- Remove optional `force` argument from `fb.GetFocusItem` as it had no effect.
- Various sample fixes. Volume now scales at the same rate as the `Default UI` volume slider. Thumbs has been fixed to work with [Last.fm](https://last.fm) site update.
- Compiled with latest `foobar2000` `SDK`.

## v2.2.2
- Fix broken colour handling in `IGdiBitmap` `GetColourSchemeJSON`. The original code by [MordredKLB](https://github.com/kbuffington) was good but I broke it so apologies for that.
- `IGdiBitmap` `GetColourScheme` has been removed. See this page for how to use the better `JSON` method mentioned above.

https://github.com/marc2k3/foo_jscript_panel/wiki/Breaking-Changes

## v2.2.1.1
- Restore access to the [Configuration Window](https://github.com/marc2k3/foo_jscript_panel/wiki/Configuration-Window) via the `Columns UI` preferences layout tab.
- Fix various sample problems reported on the `foobar2000` forums such as main menu `Edit` items not always being available and some `JS-Smooth` `overflow` errors.

## v2.2.1
- Revert all timer related changes made in `v2.2.0.2`-`v2.2.0.3`. Although tested and working fine on my main machine, it caused crashes on `Windows XP` and `foobar2000` not to exit properly when running inside a `Windows 7` virtual machine. Apologies for any inconvenience!

## v2.2.0.3
- Fix potential freeze on shutdown caused by timer threads. Thanks to [TheQwertiest](https://github.com/TheQwertiest).
- Access to the [Configuration Window](https://github.com/marc2k3/foo_jscript_panel/wiki/Configuration-Window) via the `Layout` tab in the `Columns UI` Preferences has been blocked. The `Configure Panel...` button will now be grayed out. Always use the context menu on the panel itself.
- Some minor sample bugs have been fixed.

## v2.2.0.2
- Revert some changes made in `v2.2.0` which may have broken various scripts that made use of `plman` (Playlist Manager) methods/properties. It would typically be noticeable if you had no playlists on startup or removed all playlists.

## v2.2.0.1
- Fix issue introduced in `v2.2.0` where some colours with certain levels of alpha transparency were not handled properly.

## v2.2.0
- Remove `window.DlgCode`. The docs were wrong, behaviour was inconsistent between `Default UI` and `Columns UI` and it's just not needed. Support for all keys will be enabled if you check `Grab focus` in the [Configuration Window](https://github.com/marc2k3/foo_jscript_panel/wiki/Configuration-Window). All instances will need removing from scripts. If you want to support this and older components with the same script, do something like:

```js
if ('DlgCode' in window) { window.DlgCode = 4; }
```

- `fb.RunMainMenuCommand`, `fb.RunContextCommand`, `fb.RunContextCommandWithMetadb` have all been rewritten and must be supplied with the full path to their commands. Case is not important. You should use forward slashes with no spaces. eg: `fb.RunMainMenuCommand("Library/Search")`.
- All files imported to the [Configuration Window](https://github.com/marc2k3/foo_jscript_panel/wiki/Configuration-Window), Preferences>Tools or included in a script via the `import` `PREPROCESSOR` directive must be `UTF8` encoded (with or without `BOM`). `utils.ReadTextFile` is unaffected although it should have better `UTF8` detection than before.
- The default `write_bom` argument for `utils.WriteTextFile` when omitted is now `false`.
- The code that parses the `PREPROCESSOR` section has been rewritten from scratch. The only difference is that double quotes in the name/author no longer need to be escaped so they will be duplicated if you did this previously.
- `window.GetColourCUI` and `window.GetFontCUI` no longer accept the previously optional `client_guid` argument.
- Add menu toolbar to [Configuration Window](https://github.com/marc2k3/foo_jscript_panel/wiki/Configuration-Window) which provides quick access to all included samples and docs. You can now Import/Export scripts via the `File` menu. The `Reset` option now properly resets all settings.
- The preset colour schemes previously bundled inside the `Colour Schemes` folder can now be loaded directly from the main preferences under `Tools>JScript Panel`.
- Add `utils.ListFiles` and `utils.ListFolders`.
- Update `fb.CreateHandleList` to take an optional handle argument to create a list containing a single item.
- Add `IMetadbHandle` `GetAlbumArt` method which returns the image and path without the need for using `utils.GetAlbumArtAsync` and `on_get_album_art_done`.
- `utils.FileTest` can now query files over 4GB in size when in `s` mode.
- Various sample fixes.
- Lots of internal code refactoring and external dependency updates.
- If you browse the component folder and notice the `jscript.api` and `interface.api` files are missing, it's intentional. The contents are now included inside the component.

## v2.1.8
- Add `IMetadbHandleList` `RemoveAttachedImages` method.
- There are no longer any limitations managing attached images. Working with the playing file is now supported. Thanks to Peter for the new `SDK` release and the help I needed!

## v2.1.7.2
- Fix various sample bugs.
- Compile component with latest `foobar2000` `SDK`.

## v2.1.7.1
- Images loaded by `gdi.LoadImageAsync` are no longer locked by the process.
- Correctly bump `utils.Version` - it wasn't updated in `v2.1.7`.

## v2.1.7
- Add `IMetadbHandleList` `AttachImage` / `RemoveAttachedImage` methods. There are some limitations!!

## v2.1.6
- Add `IContextMenuManager` `InitContextPlaylist` method which shows playlist specific options not available when passing a handle list to `InitContext`.
- Update `JSPlaylist` and `JS Smooth Playlist` samples with the above method.

## v2.1.5.2
- `utils.InputBox` is now centred when you open it plus you can now detect when the user presses `Cancel`.
- Fix bug with `Autoplaylists` sample which prevent existing queries from being edited.
- `gr.GdiDrawText` no longer has any return value.

## v2.1.5.1
- Fix incorrect spelling on the `Cancel` button in `utils.InputBox`.

## v2.1.5
- Add `utils.InputBox` method.
- Update samples in `complete` folder with above method - input is no longer truncated at 254 characters.
- Remove `fromhook` argument from `on_metadb_changed` callback. From now on, it will always be `undefined`.
- Ensure `JSPlaylist` doesn't load the same font more than once. Previously it was using `gdi.Font` inside `on_paint` which is bad practice.

## v2.1.4
- Add `on_dsp_preset_changed` callback.
- Add `fb.GetDSPPresets` and `fb.SetDSPPreset` methods.
- Add `IMetadbHandleList` `GetLibraryRelativePaths` method.
- Add `IMetadbHandleList` `Convert` method which converts a handle list to an array of handles.

## v2.1.3
- Add `fb.GetOutputDevices` and `fb.SetOutputDevice` methods.
- Add `on_output_device_changed` callback.
- `JSPlaylist` / `JS Smooth Browser/Playlist`, make right click behaviour consistent with other playlist/library viewers. Holding `Shift` should now show hidden context menu items.
- Playback now restarts when changing replaygain mode with `fb.ReplaygainMode`.

## v2.1.2
- `foobar2000` `v1.3.x` is now supported. `v1.3.x` users must make sure they have the `Visual C++ 2017` redist installed: https://aka.ms/vs/15/release/vc_redist.x86.exe

## v2.1.1
- A new `IGdiBitmap` `GetColourSchemeJSON` method has been added. Thanks to [MordredKLB](https://github.com/kbuffington) for the contribution.
- Fix script error when dragging items on to `JSPlaylist` / `JS Smooth Playlist` with no active playlist.

## v2.1.0.2
- Fix crash when supplying `plman.IsAutoPlaylist`/`plman.IsPlaylistLocked` with an invalid `playlistIndex`.

## v2.1.0.1
- Add `plman.RemovePlaylistSwitch` method. Unlike `plman.RemovePlaylist`, this automatically sets another playlist as active if removing the active playlist.
- Update `JS Smooth Playlist Manager` and `JSPlaylist` to make use of the above method which should fix some previous buggy behaviour whem removing playlists.

## v2.1.0
- Requires `foobar2000` `v1.4` `Beta 8` or later.
- The drag/drop functionality has been completely rewritten by [TheQwertiest](https://github.com/TheQwertiest). This will break all existing scripts which allow dragging in files from external sources. The ability to drag handle lists from `JScript Panel` to other panels has been added. The included playlist samples have been updated so they're compatible but they do not make use of the new functionality.
- Add `on_replaygain_mode_changed` callback.
- The behaviour of `plman.AddLocations` with the `select` argument set to `true` has been slightly modified.

https://github.com/marc2k3/foo_jscript_panel/wiki/Drag-and-Drop

https://github.com/marc2k3/foo_jscript_panel/wiki/Breaking-Changes

## v2.0.6
- Add `fb.CopyHandleListToClipboard` method. Contents can then be pasted in other components or as files in `Windows Explorer`.
- Add `fb.CheckClipboardContents` / `fb.GetClipboardContents`. Contents can be handles copied to the clipboard in other components or a file selection from `Windows Explorer`.
- `JSPlaylist` has been updated for full clipboard functionality with updated context menu items and keyboard shortcut support (`CTRL+C`, `CTRL+X` and `CTRL+V`).

## v2.0.5
- Add `ITitleFormat` `EvalWithMetadbs` method that takes a handle list as an argument. Returns a VBArray.
- Add `plman.SortPlaylistsByName`.
- The `IMetadbHandleList` `OrderByRelativePath` method now takes subsong index in to account. Thanks to WilB for reporting.
- `plman.GetPlaybackQueueContents` has been restored after being removed in the `v2` cleanse. It's the only way to determine if a single playlist item has been queued more than once. Note that each `IPlaybackQueueItem` is read-only.
- The `Properties` dialog size and layout have been tweaked.

## v2.0.4
- New `plman.FindPlaylist` and `plman.FindOrCreatePlaylist` methods have been addded.
- `on_library_items_added`, `on_library_items_changed`, `on_library_items_removed` now return a handle list of affected items.
- Various doc tidy up/fixes. Some `plman` methods have been updated to return `-1` on failure.

## v2.0.3
- `utils.WriteTextFile` now takes an optional `write_bom` argument. It defaults to `true` if omitted. If `false`, then the resulting file should be `UTF8` without `BOM`.
- Tidy up docs and `IMenuObj` interface. See the [Breaking Changes](https://github.com/marc2k3/foo_jscript_panel/wiki/Breaking-Changes) page for details.

## v2.0.2
- Fix random component crash caused by `utils.WriteTextFile`.
- Fix `JS-Smooth` script image caching feature choking on some images. The breakage was introduced by me in `v2.0.1.2` as I attempted to update some old code. Thanks to always.beta for reporting.

## v2.0.1.2
- As part of the `v2` update, I accidentally broke the ability to drag tracks within `JSPlaylist`. This has now been fixed.

## v2.0.1.1
- Add usage notes to `Rating` and `Last.fm Lover` samples. `Rating` can now tag files or use the `JScript Panel` `Playback Stats` database in addition to using `foo_playcount` like it did before.
- A `Queue Viewer` script has been added to the `samples\complete` folder. It has a couple of basic options on the right click menu.

## v2.0.1
- Add `Last.fm Lover` sample. Imports all loved tracks from [Last.fm](https://last.fm) in to the new `JScript Panel` `Playback Stats` database. All loved tracks will have the value of `%JSP_LOVED%` set to `1` which is available in all components/search. You can then love/unlove tracks by clicking the heart icon. Obviously this requires a [Last.fm](https://last.fm) account. Use the right click menu to set your username and authorise the script.
- The timer mechanism behind `window.SetInterval` and `window.SetTimeout` has been completely rewritten by [TheQwertiest](https://github.com/TheQwertiest). Previously, it was limited to 16 instances and then it would fail causing random buggy behaviour in some advanced scripts.
- Fix `ListenBrainz` not clearing its cache after a successful submission. Thanks to zeremy for reporting.
- Fix `Thumbs` sample not deleting images.

## v2.0.0
- Requires `foobar2000` `v1.4`. It will not load with anything earlier.
- `fb.Trace` has been removed and replaced with a native `console.log` method. It takes multiple arguments as before.
- All callbacks/methods with `Color` in the name have been renamed with `Colour`. This will break most scripts!

```
on_colors_changed -> on_colours_changed
utils.ColorPicker -> utils.ColourPicker
utils.GetSysColor -> utils.GetSysColour
window.GetColorCUI -> window.GetColourCUI
window.GetColorDUI -> window.GetColourDUI
IGdiBitmap GetColorScheme -> GetColourScheme
```

- The `IPlaybackQueueItem` interface and `plman.CreatePlaybackQueueItem` method have been removed.
- `plman.GetPlaybackQueueContents` has been replaced with `plman.GetPlaybackQueueHandles` which returns a handle list. You can check the `Count` property so there is no longer any need for `plman.GetPlaybackQueueCount` and `plman.IsPlaybackQueueActive` which have been removed.
- `on_refresh_background_done` and `window.GetBackgroundImage` have both been removed.
- The `IGdiBitmap` `BoxBlur` method has been removed. Use `StackBlur` instead.
- `IContextMenuManager` `InitContext` only accepts a handle list as an argument.
- Anyone who has a `js_settings` folder in their `foobar2000` profile folder from using my previous samples should move the contents in to the `js_data` folder. Typically, this would be `autoplaylists.json`, `thumbs.ini`, `listenbrainz.ini`.
- Enable `Windows XP` support. Also, support for `Windows Vista`, `Windows 7` and `WINE` with `IE7`/`IE8` has been restored. `ES5` features are still supported for those with `IE9` or later. If your scripts crash after upgrading and making the changes outlined above, make sure to check the `Script Engine` setting in the [Configuration Window](https://github.com/marc2k3/foo_jscript_panel/wiki/Configuration-Window).
- Add `utils.WriteTextFile`. Always creates a `UTF8-BOM` file.
- Add `window.Name` property.
- Add a custom `Playback Stats` backend and `handle` methods for writing values.

https://github.com/marc2k3/foo_jscript_panel/wiki/Breaking-Changes

https://github.com/marc2k3/foo_jscript_panel/wiki/Configuration-Window

https://github.com/marc2k3/foo_jscript_panel/wiki/Playback-Stats

https://github.com/marc2k3/foo_jscript_panel/wiki/Samples

## v1.3.2.2
- Component now reports version on script error.
- `JSPlaylist` and `JS Smooth Playlist` have been unable to tag files since `v1.3.0` when `foo_playcount` was missing.

## v1.3.2.1
- Add `fb.AlwaysOnTop` property. Can be used to read or write the setting. A new `on_always_on_top_changed` callback has been added to detect when it changes via any other means such as using the menu, keyboard shortcuts, etc.

## v1.3.2
- Improved handling of `UTF8` files without `BOM`. This affects files loaded via the `PREPROCESSOR` section and the `Configuration Window>Tools>Import` button. They should now be treated the same as files with `BOM`.

## v1.3.1
- Fixed a bug where `plman.IsPlaylistItemSelected` didn't return a proper `boolean` value. Thanks to [TheQwertiest](https://github.com/TheQwertiest) for reporting.
- Fixed a bug where `plman.ShowAutoPlaylistUI` didn't always return the correct `boolean` value.
- `plman.MovePlaylistSelection` now returns a `boolean` value to indicate success or failure.
- Update docs for consistency with actual component behaviour. Some methods which returned useless values have been updated to return nothing.
- On failure, these now return `-1`.

```
IFileInfo InfoFind
IFileInfo MetaFind
IMetadbHandleList BSearch
IMetadbHandleList Find
plman.ActivePlaylist
plman.CreateAutoPlaylist
plman.PlayingPlaylist
```

- Remove the whole `IStyleTextRender` interface. Anything that uses `gdi.CreateStyleTextRender` or was based on `glow text sample.txt` will no longer work
- Remove these `IPlaylistRecyclerManager` properties/methods:

```
Id
FindById
RestoreById
```

## v1.3.0
- Support for `IE7`, `IE8` and `WINE` users has been dropped. Your computer must have `IE9` or later installed.
- The `UpdateFileInfoSimple` handle and handle list methods are now gone.
- A new `UpdateFileInfoFromJSON` method has been added. If anyone needs help with this, just ask.
- Add `fb.CreateHandleList` method for creating empty handle lists.

## v1.2.4
- Add `utils.ColorPicker` which is a popup dialog for selecting colours.
- Fix various bugs in `samples\complete` folder.
- `ListenBrainz` sample now has an on/off toggle on the menu.

## v1.2.3.3
- Fix typo in `MusicBrainz` sample. Thanks to zeremy for spotting.
- `ListenBrainz` now has caching/retry support. It's my first attempt at this so please report any issues! I also need reports of any code 400 errors from the server as the script will need updating before these tracks can be submitted successfully.
- Br3tt's `JS Smooth` scripts are now bundled with the component and the separate repository has been nuked.

## v1.2.3.2
- `ListenBrainz` sample - fix `musicbrainz track id` not being submitted from `m4a` files.

## v1.2.3.1
- Add `Last.fm Bio`, `Allmusic` and `ListenBrainz` samples. `Thumbs` can download images from [Last.fm](https://last.fm) again but only manually.
- Most samples are now DPI aware so text/tooltips should increase inline with other desktop elements. For existing users, each panel script should be re-imported from the `samples\complete` folder.

## v1.2.3
- Add `fb.ReplaygainMode`. Can be used to retrieve or set the value.
- Hide `JScript Panel` main menu items. Given they're meant for binding to to keyboard shortcuts/standard buttons, there is no need for them to be visible. Hold `Shift` if you really want to access them from the menu.
- Fix `IMetadbHandleList` `MakeUnion` method and update docs with examples for `MakeUnion`, `MakeDifference` and `MakeIntersection` methods.

## v1.2.2
- Add new `on_main_menu` callback. 10 main menu items have been created meaning you can now bind them to global keyboard shortcuts, standard toolbar buttons, etc and have them run your own custom code.

## v1.2.1
- Fix bug where the playing item handle was never sent to the `on_playback_edited` callback even though the docs said it would.
- Tidy up docs and auto-complete hints for `on_metadb_changed` and `on_selection_changed`.

## v1.2.0.1
- Minor code cleanups but no new component features for now.
- Add `Last.fm Similar Artists / Charts` and `MusicBrainz` samples.

## v1.2.0
- If your computer has `IE9` or later installed, newer `ECMAScript5` features are now available. Many thanks to [TheQwertiest](https://github.com/TheQwertiest) for discovering how to do this.
- The `Script engine` option has been removed from the [Configuration Window](https://github.com/marc2k3/foo_jscript_panel/wiki/Configuration-Window). The newer `Chakra` engine is always used if available but will fall back to `JScript` if not.
- Add `utils.Version` which reports the component version.
- Minor tweaks to [Configuration Window](https://github.com/marc2k3/foo_jscript_panel/wiki/Configuration-Window).
- Update `JSPlaylist` so it works with `Chakra` engine. Thanks to zeremy for reporting
- A previous `window.RepaintRect` bug reported by Wilb has now been fixed.
- Zap a few more compiler warnings.

## v1.1.7
- Remove trailing space in `Console` when using `fb.Trace`.
- `Text Reader` sample fix. Ensure `Open containing folder` works when specifiying a folder rather than a full path to a file.
- `Thumbs` sample fix. Addresses an issue where using `%profile%` in the `Custom folder` setting would fail if the profile path contained special title formatting characters like `%()[]'`.
- Remove `Delay load` from [Configuration Window](https://github.com/marc2k3/foo_jscript_panel/wiki/Configuration-Window).

## v1.1.6.2
- Fix bug with `plman.AddLocations` where the original order of the supplied `array` wasn't maintained. Also, the progress dialog should be hidden for short operations.

## v1.1.6.1
- Minor changes to `Console` logging on startup.
- The `complete` samples folder has 2 new scripts: `Thumbs` and `Text reader`. `Thumbs` allows the display/cycling of multiple images in a given folder. `Text reader` displays the contents of any plain text file. Title formatting can be used to specify the folder/file paths.

## v1.1.6
- Add `IGdiBitmap` `StackBlur` method.
- Update `ApplyMask` sample.

## v1.1.5.1
- Add `Open component folder` to default right click menu. This provides quick access to the docs and samples.

## v1.1.5
- `plman.IsPlaylistLocked(playlistIndex)`.
- `on_playlists_changed` is now called when a playlist's lock status changes through the use of components such as `foo_utils` or `foo_playlist_attributes`.
- Existing samples moved in to new `basic` subfolder.
- Add new `complete` samples folder containing some more feature complete samples.
- Add updated `JSPlaylist` for `JScript Panel`.

## v1.1.4.1
- Prevent old tooltips lingering after script error.

## v1.1.4
- Fix bug where keyboard shortcuts from the main `Preferences` were never processed in `Columns UI` when `JScript Panel` had focus.

## v1.1.3
- Add `IGdiBitmap` `SaveAs` method.

## v1.1.2
- Fix `on_mouse_wheel` not being called with certain trackpads.

## v1.1.1
- New `on_mouse_wheel_h` callback for left/right scrolling.

## v1.1.0
- `GetQueryItems` should be a `fb` method, not `plman`.
- Fix crash when using `GetQueryItems` with dates as part of the query.
- Update `Columns UI` `SDK`.

## v1.0.7
- Add `fb.GetLibraryRelativePath(handle)`.

## v1.0.6
- Add `IMetadbHandleList UpdateFileInfoSimple` method.

## v1.0.5
- Update `Columns UI` `SDK`.
- Tidy up docs/samples.

## v1.0.4
- Add `utils.MapString(text, lcid, flags)`.

## v1.0.3
- Revert previous `Columns UI` `SDK` update.

## v1.0.2
- Update `Columns UI` `SDK`.
- Hopefully removed all traces of `WSH Panel Mod` from sources and documentation!

## v1.0.1
- Update default script so you know a blank panel is `JScript Panel` as opposed to `WSH Panel Mod`.

## v1.0.0 (Compared to WSH Panel Mod 1.5.6)
- Given the name, it obviously doesn't support `VBScript`.
- Due to using updated library files, support for `Windows XP` has been dropped.
- Compiled with new `foobar2000` `SDK`. Requires `foobar2000` `v1.3` or above.
- Remove functions marked as obsolete 3+ years ago. Obviously there are replacements that been around even longer.

```
window.WatchMetadb
window.UnWatchMetadb
window.CreateTimerTimeout
window.CreateTimerInterval
window.KillTimer
UpdateFileInfo
```

- Remove all these `fb` methods because `plman` (Playlist Manager) methods also exist.

```
fb.CreatePlaylist
fb.RemovePlaylist
fb.RenamePlaylist
fb.DuplicatePlaylist
fb.MovePlaylist
fb.ActivePlaylist
fb.PlayingPlaylist
fb.PlaylistCount
fb.PlaylistItemCount
fb.GetPlaylistName
fb.PlaybackOrder
```

- Even though `plman` alternatives of these `fb` methods did not exist, I've removed them for consistency.

```
fb.CreateAutoPlaylist
fb.IsAutoPlaylist
fb.ShowAutoPlaylistUI
```

- Because of the above change, `plman.CreateAutoPlaylist`, `plman.IsAutoPlaylist` and `plman.ShowAutoPlaylistUI` have been created to replace them.
- `plman.GetPlaylistFocusItemHandle` has been removed because it's identical to `fb.GetFocusItem` which is more commonly used.
- `utils.GetAlbumArt` removed as the corresponding function has been removed from the `foobar2000` `SDK`. `utils.GetAlbumArtAsync`, `utils.GetAlbumArtEmbedded` and `utils.GetAlbumArtV2` are still present.
- `AppendMenuItem` no longer accepts `MF_POPUP` as a flag. Use `AppendTo` instead.
- Remove `on_tooltip_custom_paint` callback. `window.CreateTooltip` has been updated to accept custom font paramters.
- `Properties` dialog has a larger default size.
- `Safe mode` is no longer an option.
- Tidy up samples.
- Tidy up `PREPROCESSOR` handling. You no longer have to specify these when using the `on_metadb_changed` callback.

```js
// @feature "v1.4"
// @feature "watch-metadb"
```

- Due to the above change, `on_metadb_changed` will only receive a `handle list` and never a `handle` as the first argument. Also, `on_selection_changed` won't receive a `handle` argument.
- Add `fb.GetLibraryItems()` method. Returns a handle list of all items in library.
- Add `fb.IsLibraryEnabled()`.
- Add `fb.ShowLibrarySearchUI(query)` opens the `Library>Search` window populated with the query you set.
- Add `on_library_items_added` callback.
- Add `on_library_items_removed` callback.
- Add `on_library_items_changed` callback.
- Add `plman.AddLocations(playlistIndex, paths[, select])`.
- Add `plman.ClearPlaylist(playlistIndex)`. Use to clear a specified playlist. `fb.ClearPlaylist` still exists because it is just a shortcut to the `Edit` menu command which clears the active playlist.
- Add `plman.GetQueryItems(source_handlelist, query)` returns an unsorted handle list. Consider using `OrderByFormat`, etc on the result.
- Add `plman.UndoBackup(playlistIndex)`. If you call this before adding/removing/reordering playlist items, you will be able to use the undo/redo commands on the `Edit` menu.
- Add `CalcTotalDuration()` handle list method. returns the total in seconds.
- Add `CalcTotalSize()` handle list method. returns the total in bytes.
- Add `utils.FormatDuration(seconds)`. Returns a `string` like `3wk 2d 1:30:21`.
- Add `utils.FormatFileSize(bytes)`. Returns a `string` like `7.9 GB`.
- `window.CreateToolTip` now takes optional font `name`, `font size (px)` and `style` arguments. eg `window.CreateToolTip("Segoe UI", 32, 1)`. Defaults of `Segoe UI`, `12` and `0` are used if omitted.
- Default right click menu now has a `Reload` script option. This saves opening/closing the dialog when working on external files. In addition, a new `window.Reload` method has been added so you can force a panel reload with your own menus, buttons, etc.
- Script errors are now displayed in a popup window in addition to the `Console` like it was previously.
- `EstimateLineWrap` no longer leaves stray punctuation when wrapping text at end of line.
- `IMetadbHandle` `FileSize` now works with `JScript` engine. Previously, it only worked with `JScript9`.
