## v2.5.6
- Fix crash with `utils.GetAlbumArtAsync` when it's asked to query art from radio streams. Update docs to clarify only `IMetadbHandle` `GetAlbumArt` and `utils.GetAlbumArtV2` can retrieve that type of art.
- Fix `track info + seekbar + buttons` sample to properly update on stream title/art changes. Note this requires replacing the existing text in any existing panel by using the `Samples` menu.

## v2.5.5.2
- Fix double click action in `Album Art` sample.

## v2.5.5.1
- The `on_playback_dynamic_info_track` callback now receives a `type` as the first argument. It will have a value of `0` for stream title updates and a value of `1` for stream album art updates. 

## v2.5.5
- The included `Album art` sample now displays album art (front cover only) from radio streams. Requires `foobar2000` `v1.6.6` or above. Tested with [RadioParadise](https://radioparadise.com/listen/stream-links) as per this thread: https://hydrogenaud.io/index.php?topic=120855.0

## v2.5.4
- Fix bug where the `cookie` returned by `gdi.LoadImageAsync` wasn't always unique.
- A recent website update broke the `Allmusic` sample. This is now fixed.

## v2.5.3
- Add `IMetadbHandle` `ShowAlbumArtViewer` which uses the new internal viewer added in foobar2000 `v1.6.2`. You will see a popup text message if you try and use it on earlier versions. Check the docs for options.
- The included `Album art` sample now has 3 choices available for the `double click` action. Use the right click menu to choose.
  * Open using external viewer
  * Open using new internal viewer mentioned above
  * Open containing folder

## v2.5.2
- Add `IMetadbHandleList` `SaveAs` method which saves using the native `foobar2000` `fpl` format. The docs also give an example of how to save an `m3u8` playlist using `utils.WriteTextFile`.
- `IUiSelectionHolder` `SetSelection` now takes an optional `type` argument.
- Fix `utils.ReadUTF8` so it now strips the `BOM` if present. `utils.ReadTextFile` already did this. Although its presence didn't affect display, it could cause `JSON.parse` to fail. It never affected included samples because files are always written without `BOM`.

## v2.5.1
- Add `IMetadbHandleList` `Randomise`.
- Add `IMetadbHandleList` `RemoveDuplicatesByFormat`.
- Update `JS-Smooth Browser` / `JS-Smooth Playlist`.
  * The context menu no longer has a `Settings` option. Right click the `Album count` or `Playlist name` in the top right corner to access the same settings.
  * The disk cache is always enabled and now supports embedded art. Start up should be faster once the cache has been generated.
  * `JS-Smooth Browser` has had the browse by `Genre` option removed.

## v2.5.0.2
- Fix editor bug that caused crashes if `foo_wave_seekbar` was installed.

## v2.5.0.1
- Fix `utils.Glob` bug introduced in `v2.4.3.1` where each item in the returned array had no separator between the filename and parent folder.

## v2.5.0
- The internal mechanism for saving per panel settings has changed. All scripts/settings from previous versions will be kept as you upgrade but the changes mean you can no longer downgrade to earlier versions without losing scripts embedded in each panel. Exporting your current layout before upgrading is highly recommended. Then if the worst happens, you can always re-import your theme with an earlier version.
- The `Album Art` sample now has a menu option to customise the double click action. It can either open the image in the default viewer as before or it can open the containing folder.
- Add `utils.ReplaceIllegalChars(str[, modern])`. If `modern` is set to `true`, characters are replaced with similar loooking unicode characters that are valid for use in file paths. If `false`, legacy character replacements are used. The behaviour should match the same advanced `Preferences` that are available for `File Operations` and the `Converter` built-in to `foobar2000` `v1.6` and later.
- Add `IMetadbHandleList` `RemoveDuplicates` method. The original order of the handle list is preserved.
- Remove `IMetadbHandleList` `MakeUnion` and `Sort` methods. `MakeDifference`, `MakeIntersection` and `BSearch`  now perform the required sorting internally so any calls to `Sort` before using them must be removed. More details can be found on the `Breaking Changes` wiki page.
- (Beta.3) Fix bug in `IMetadbHandleList` `BSearch` introduced in `Beta.1`.
- (Beta.4) Properly fix `IMetadbHandleList` `BSearch`. The attempted fix in `Beta.3` was incorrect because it made a copy of the handle list internally which meant you couldn't remove the handle by the id of the result. Now the original order is not preserved which matches the behaviour of older versions when you had to manually `Sort()` first.

https://github.com/marc2k3/foo_jscript_panel/wiki/Breaking-Changes

## v2.4.3.1
- Fix bug introduced in `v.2.4.3` which caused a crash on startup when `foo_whatsnew` is present. Apologies for any inconvenience.
- The `Preferences` for the `Editor Properties` have been reset to default. If you used one of the built in themes, you'll need to select it again. Any manual changes will need to be re-applied. `Properties` that used placeholders like `$(style.default)` are no longer supported so you'd have to duplicate those values manually.

## v2.4.3
- Add `window.SetTooltipFont`. Since it's not permitted to call `window.CreateTooltip` more than once in a panel, this method has been added for changing the tooltip font without reloading the panel.

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
- The project is now [MIT licensed](https://github.com/marc2k3/foo_jscript_panel/blob/v2.5.x/LICENSE.md).

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
utils.WriteTextFile "write_bom" (writing a `BOM` is no longer supported)
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
