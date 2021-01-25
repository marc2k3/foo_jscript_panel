var fso = new ActiveXObject("Scripting.FileSystemObject");
var CACHE_FOLDER = fb.ProfilePath + "js_smooth_cache\\";
if (!fso.FolderExists(CACHE_FOLDER)) fso.CreateFolder(CACHE_FOLDER);

function reset_cover_timers() {
	if (timers.coverDone) {
		window.ClearTimeout(timers.coverDone);
		timers.coverDone = false;
	}
}

function generate_filename(cachekey, art_id) {
	var prefix = art_id == 4 ? "artist" : "front";
	return CACHE_FOLDER + prefix + cachekey + ".jpg";
}

function get_art(metadb, albumIndex, art_id) {
	var img = gdi.Image(generate_filename(brw.groups[albumIndex].cachekey, art_id));
	if (img) return img;

	if (brw.groups[albumIndex].load_requested == 0) {
		brw.groups[albumIndex].load_requested = 1;
		window.SetTimeout(function () {
			utils.GetAlbumArtAsync(window.ID, metadb, art_id, false, false, false);
		}, 10);
	}
	return img;
}

function on_get_album_art_done(metadb, art_id, image, image_path) {
	var tot = brw.groups.length;
	for (var i = 0; i < tot; i++) {
		if (brw.groups[i].metadb && brw.groups[i].metadb.Compare(metadb)) {
			if (image) {
				var s = Math.min(200 / image.Width, 200 / image.Height);
				var w = Math.floor(image.Width * s);
				var h = Math.floor(image.Height * s);
				image = image.Resize(w, h, 2);
				image.SaveAs(generate_filename(brw.groups[i].cachekey, art_id));
				brw.groups[i].cover_img = image;
			} else {
				brw.groups[i].cover_img = images.noart;
			}
			brw.repaint();
			break;
		}
	}
}

function drawImage(gr, img, src_x, src_y, src_w, src_h, auto_fill, border, alpha) {
	if (!img || !src_w || !src_h) {
		return;
	}
	gr.SetInterpolationMode(7);
	if (auto_fill) {
		if (img.Width / img.Height < src_w / src_h) {
			var dst_w = img.Width;
			var dst_h = Math.round(src_h * img.Width / src_w);
			var dst_x = 0;
			var dst_y = Math.round((img.Height - dst_h) / 4);
		} else {
			var dst_w = Math.round(src_w * img.Height / src_h);
			var dst_h = img.Height;
			var dst_x = Math.round((img.Width - dst_w) / 2);
			var dst_y = 0;
		}
		gr.DrawImage(img, src_x, src_y, src_w, src_h, dst_x + 3, dst_y + 3, dst_w - 6, dst_h - 6, 0, alpha || 255);
	} else {
		var s = Math.min(src_w / img.Width, src_h / img.Height);
		var w = Math.floor(img.Width * s);
		var h = Math.floor(img.Height * s);
		src_x += Math.round((src_w - w) / 2);
		src_y += src_h - h;
		src_w = w;
		src_h = h;
		var dst_x = 0;
		var dst_y = 0;
		var dst_w = img.Width;
		var dst_h = img.Height;
		gr.DrawImage(img, src_x, src_y, src_w, src_h, dst_x, dst_y, dst_w, dst_h, 0, alpha || 255);
	}
	if (border) {
		gr.DrawRect(src_x, src_y, src_w - 1, src_h - 1, 1, border);
	}
}

// *****************************************************************************************************************************************
// Common functions & flags by Br3tt aka Falstaff (c)2013-2015
// *****************************************************************************************************************************************

//=================================================// General declarations
SM_CXVSCROLL = 2;
SM_CYHSCROLL = 3;

DLGC_WANTARROWS = 0x0001; /* Control wants arrow keys         */
DLGC_WANTTAB = 0x0002; /* Control wants tab keys           */
DLGC_WANTALLKEYS = 0x0004; /* Control wants all keys           */
DLGC_WANTMESSAGE = 0x0004; /* Pass message to control          */
DLGC_HASSETSEL = 0x0008; /* Understands EM_SETSEL message    */
DLGC_DEFPUSHBUTTON = 0x0010; /* Default pushbutton               */
DLGC_UNDEFPUSHBUTTON = 0x0020; /* Non-default pushbutton           */
DLGC_RADIOBUTTON = 0x0040; /* Radio button                     */
DLGC_WANTCHARS = 0x0080; /* Want WM_CHAR messages            */
DLGC_STATIC = 0x0100; /* Static item: don't include       */
DLGC_BUTTON = 0x2000; /* Button item: can be checked      */

// Used in utils.Glob()
// For more information, see: http://msdn.microsoft.com/en-us/library/ee332330%28VS.85%29.aspx
FILE_ATTRIBUTE_READONLY = 0x00000001;
FILE_ATTRIBUTE_HIDDEN = 0x00000002;
FILE_ATTRIBUTE_SYSTEM = 0x00000004;
FILE_ATTRIBUTE_DIRECTORY = 0x00000010;
FILE_ATTRIBUTE_ARCHIVE = 0x00000020;
//FILE_ATTRIBUTE_DEVICE            = 0x00000040; // do not use
FILE_ATTRIBUTE_NORMAL = 0x00000080;
FILE_ATTRIBUTE_TEMPORARY = 0x00000100;
FILE_ATTRIBUTE_SPARSE_FILE = 0x00000200;
FILE_ATTRIBUTE_REPARSE_POINT = 0x00000400;
FILE_ATTRIBUTE_COMPRESSED = 0x00000800;
FILE_ATTRIBUTE_OFFLINE = 0x00001000;
FILE_ATTRIBUTE_NOT_CONTENT_INDEXED = 0x00002000;
FILE_ATTRIBUTE_ENCRYPTED = 0x00004000;
//FILE_ATTRIBUTE_VIRTUAL           = 0x00010000; // do not use

// }}
// Use with MenuManager()
// {{
MF_STRING = 0x00000000;
MF_GRAYED = 0x00000001;
// }}
// Used in get_colors()
// {{
COLOR_WINDOW = 5;
COLOR_HIGHLIGHT = 13;
COLOR_BTNFACE = 15;
COLOR_BTNTEXT = 18;
// }}
// Used in window.SetCursor()
// {{
IDC_ARROW = 32512;
IDC_IBEAM = 32513;
IDC_WAIT = 32514;
IDC_CROSS = 32515;
IDC_UPARROW = 32516;
IDC_SIZE = 32640;
IDC_ICON = 32641;
IDC_SIZENWSE = 32642;
IDC_SIZENESW = 32643;
IDC_SIZEWE = 32644;
IDC_SIZENS = 32645;
IDC_SIZEALL = 32646;
IDC_NO = 32648;
IDC_APPSTARTING = 32650;
IDC_HAND = 32649;
IDC_HELP = 32651;
// }}
// Use with GdiDrawText()
// {{
var DT_LEFT = 0x00000000;
var DT_RIGHT = 0x00000002;
var DT_TOP = 0x00000000;
var DT_BOTTOM = 0x00000008;
var DT_CENTER = 0x00000001;
var DT_VCENTER = 0x00000004;
var DT_WORDBREAK = 0x00000010;
var DT_SINGLELINE = 0x00000020;
var DT_CALCRECT = 0x00000400;
var DT_NOPREFIX = 0x00000800;
var DT_EDITCONTROL = 0x00002000;
var DT_END_ELLIPSIS = 0x00008000;
// }}
// Keyboard Flags & Tools
// {{
var VK_F1 = 0x70;
var VK_F2 = 0x71;
var VK_F3 = 0x72;
var VK_F4 = 0x73;
var VK_F5 = 0x74;
var VK_F6 = 0x75;
var VK_BACK = 0x08;
var VK_TAB = 0x09;
var VK_RETURN = 0x0D;
var VK_SHIFT = 0x10;
var VK_CONTROL = 0x11;
var VK_ALT = 0x12;
var VK_ESCAPE = 0x1B;
var VK_PGUP = 0x21;
var VK_PGDN = 0x22;
var VK_END = 0x23;
var VK_HOME = 0x24;
var VK_LEFT = 0x25;
var VK_UP = 0x26;
var VK_RIGHT = 0x27;
var VK_DOWN = 0x28;
var VK_INSERT = 0x2D;
var VK_DELETE = 0x2E;
var VK_SPACEBAR = 0x20;
var KMask = {
	none: 0,
	ctrl: 1,
	shift: 2,
	ctrlshift: 3,
	ctrlalt: 4,
	ctrlaltshift: 5,
	alt: 6
}

function GetKeyboardMask() {
	var c = utils.IsKeyPressed(VK_CONTROL) ? true : false;
	var a = utils.IsKeyPressed(VK_ALT) ? true : false;
	var s = utils.IsKeyPressed(VK_SHIFT) ? true : false;
	var ret = KMask.none;
	if (c && !a && !s)
		ret = KMask.ctrl;
	if (!c && !a && s)
		ret = KMask.shift;
	if (c && !a && s)
		ret = KMask.ctrlshift;
	if (c && a && !s)
		ret = KMask.ctrlalt;
	if (c && a && s)
		ret = KMask.ctrlaltshift;
	if (!c && a && !s)
		ret = KMask.alt;
	return ret;
}
// }}
// {{
// Used in window.GetColorCUI()
ColorTypeCUI = {
	text: 0,
	selection_text: 1,
	inactive_selection_text: 2,
	background: 3,
	selection_background: 4,
	inactive_selection_background: 5,
	active_item_frame: 6
}
// Used in window.GetFontCUI()
FontTypeCUI = {
	items: 0,
	labels: 1
}
// Used in window.GetColorDUI()
ColorTypeDUI = {
	text: 0,
	background: 1,
	highlight: 2,
	selection: 3
}
// Used in window.GetFontDUI()
FontTypeDUI = {
	defaults: 0,
	tabs: 1,
	lists: 2,
	playlists: 3,
	statusbar: 4,
	console: 5
}
//}}
// {{
// Used in gr.DrawString()
function StringFormat() {
	var h_align = 0,
	v_align = 0,
	trimming = 0,
	flags = 0;
	switch (arguments.length) {
	case 3:
		trimming = arguments[2];
	case 2:
		v_align = arguments[1];
	case 1:
		h_align = arguments[0];
		break;
	default:
		return 0;
	}
	return ((h_align << 28) | (v_align << 24) | (trimming << 20) | flags);
}
StringAlignment = {
	Near: 0,
	Centre: 1,
	Far: 2
}
var lt_stringformat = StringFormat(StringAlignment.Near, StringAlignment.Near);
var ct_stringformat = StringFormat(StringAlignment.Centre, StringAlignment.Near);
var rt_stringformat = StringFormat(StringAlignment.Far, StringAlignment.Near);
var lc_stringformat = StringFormat(StringAlignment.Near, StringAlignment.Centre);
var cc_stringformat = StringFormat(StringAlignment.Centre, StringAlignment.Centre);
var rc_stringformat = StringFormat(StringAlignment.Far, StringAlignment.Centre);
var lb_stringformat = StringFormat(StringAlignment.Near, StringAlignment.Far);
var cb_stringformat = StringFormat(StringAlignment.Centre, StringAlignment.Far);
var rb_stringformat = StringFormat(StringAlignment.Far, StringAlignment.Far);
//}}
// {{
// Used in utils.GetAlbumArt()
AlbumArtId = {
	front: 0,
	back: 1,
	disc: 2,
	icon: 3,
	artist: 4
}
//}}
// {{
// Used everywhere!
function RGB(r, g, b) {
	return (0xff000000 | (r << 16) | (g << 8) | (b));
}
function RGBA(r, g, b, a) {
	return ((a << 24) | (r << 16) | (g << 8) | (b));
}
function getAlpha(color) {
	return ((color >> 24) & 0xff);
}

function getRed(color) {
	return ((color >> 16) & 0xff);
}

function getGreen(color) {
	return ((color >> 8) & 0xff);
}

function getBlue(color) {
	return (color & 0xff);
}

function negative(colour) {
	var R = getRed(colour);
	var G = getGreen(colour);
	var B = getBlue(colour);
	return RGB(Math.abs(R - 255), Math.abs(G - 255), Math.abs(B - 255));
}

function toRGB(d) { // convert back to RGB values
	var d = d - 0xff000000;
	var r = d >> 16;
	var g = d >> 8 & 0xFF;
	var b = d & 0xFF;
	return [r, g, b];
}

function blendColors(c1, c2, factor) {
	// When factor is 0, result is 100% color1, when factor is 1, result is 100% color2.
	var c1 = toRGB(c1);
	var c2 = toRGB(c2);
	var r = Math.round(c1[0] + factor * (c2[0] - c1[0]));
	var g = Math.round(c1[1] + factor * (c2[1] - c1[1]));
	var b = Math.round(c1[2] + factor * (c2[2] - c1[2]));
	return (0xff000000 | (r << 16) | (g << 8) | (b));
}

function draw_glass_reflect(w, h) {
	// Mask for glass effect
	var Mask_img = gdi.CreateImage(w, h);
	var gb = Mask_img.GetGraphics();
	gb.FillSolidRect(0, 0, w, h, 0xffffffff);
	gb.FillGradRect(0, 0, w - 20, h, 0, 0xaa000000, 0, 1.0);
	gb.SetSmoothingMode(2);
	gb.FillEllipse(-20, 25, w * 2 + 40, h * 2, 0xffffffff);
	Mask_img.ReleaseGraphics(gb);
	// drawing the white rect
	var glass_img = gdi.CreateImage(w, h);
	gb = glass_img.GetGraphics();
	gb.FillSolidRect(0, 0, w, h, 0xffffffff);
	glass_img.ReleaseGraphics(gb);
	// resizing and applying the mask
	var Mask = Mask_img.Resize(w, h);
	glass_img.ApplyMask(Mask);
	Mask.Dispose();
	return glass_img;
}

function num(strg, nb) {
	if (!strg) return "";
	var i;
	var str = strg.toString();
	var k = nb - str.length;
	if (k > 0) {
		for (i = 0; i < k; i++) {
			str = "0" + str;
		}
	}
	return str.toString();
}

function TrackType(trkpath) {
	var taggable;
	var type;
	switch (trkpath) {
	case "file":
		taggable = 1;
		type = 0;
		break;
	case "cdda":
		taggable = 1;
		type = 1;
		break;
	case "FOO_":
		taggable = 0;
		type = 2;
		break;
	case "http":
		taggable = 0;
		type = 3;
		break;
	case "mms:":
		taggable = 0;
		type = 3;
		break;
	case "unpa":
		taggable = 0;
		type = 4;
		break;
	default:
		taggable = 0;
		type = 5;
	}
	return type;
}

function replaceAll(str, search, repl) {
	while (str.indexOf(search) != -1) {
		str = str.replace(search, repl);
	}
	return str;
}

//=================================================// Button object
var ButtonStates = {
	normal: 0,
	hover: 1,
	down: 2
}

var button = function (normal, hover, down) {
	this.img = Array(normal, hover, down);
	this.w = this.img[0].Width;
	this.h = this.img[0].Height;
	this.state = ButtonStates.normal;
	this.update = function (normal, hover, down) {
		this.img = Array(normal, hover, down);
		this.w = this.img[0].Width;
		this.h = this.img[0].Height;
	}
	this.draw = function (gr, x, y, alpha) {
		this.x = x;
		this.y = y;
		this.img[this.state] && gr.DrawImage(this.img[this.state], this.x, this.y, this.w, this.h, 0, 0, this.w, this.h, 0, alpha);
	}
	this.repaint = function () {
		window.RepaintRect(this.x, this.y, this.w, this.h);
	}
	this.checkstate = function (event, x, y) {
		this.ishover = (x > this.x && x < this.x + this.w - 1 && y > this.y && y < this.y + this.h - 1);
		this.old = this.state;
		switch (event) {
		case "down":
			switch (this.state) {
			case ButtonStates.normal:
			case ButtonStates.hover:
				this.state = this.ishover ? ButtonStates.down : ButtonStates.normal;
				this.isdown = true;
				break;
			}
			break;
		case "up":
			this.state = this.ishover ? ButtonStates.hover : ButtonStates.normal;
			this.isdown = false;
			break;
		case "right":

			break;
		case "move":
			switch (this.state) {
			case ButtonStates.normal:
			case ButtonStates.hover:
				this.state = this.ishover ? ButtonStates.hover : ButtonStates.normal;
				break;
			}
			break;
		case "leave":
			this.state = this.isdown ? ButtonStates.down : ButtonStates.normal;
			break;
		}
		if (this.state != this.old)
			this.repaint();
		return this.state;
	}
}

//=================================================// Tools (general)

function DrawPolyStar(gr, x, y, out_radius, in_radius, points, line_thickness, line_color, fill_color, angle, opacity) {
	// ---------------------
	// code by ExtremeHunter
	// ---------------------

	if (!opacity && opacity != 0)
		opacity = 255;

	//---> Create points
	var point_arr = [];
	for (var i = 0; i != points; i++) {
		i % 2 ? r = Math.round((out_radius - line_thickness * 4) / 2) / in_radius : r = Math.round((out_radius - line_thickness * 4) / 2);
		var x_point = Math.floor(r * Math.cos(Math.PI * i / points * 2 - Math.PI / 2));
		var y_point = Math.ceil(r * Math.sin(Math.PI * i / points * 2 - Math.PI / 2));
		point_arr.push(x_point + out_radius / 2);
		point_arr.push(y_point + out_radius / 2);
	}

	//---> Crate poligon image
	var img = gdi.CreateImage(out_radius, out_radius);
	var _gr = img.GetGraphics();
	_gr.SetSmoothingMode(2);
	_gr.FillPolygon(fill_color, 1, point_arr);
	if (line_thickness > 0)
		_gr.DrawPolygon(line_color, line_thickness, point_arr);
	img.ReleaseGraphics(_gr);

	//---> Draw image
	gr.DrawImage(img, x, y, out_radius, out_radius, 0, 0, out_radius, out_radius, angle, opacity);
}

function get_system_scrollbar_width() {
	var tmp = utils.GetSystemMetrics(SM_CXVSCROLL);
	return tmp;
}

String.prototype.repeat = function (num) {
	if (num >= 0 && num <= 5) {
		var g = Math.round(num);
	} else {
		return "";
	}
	return new Array(g + 1).join(this);
}

// ===================================================== // Wallpaper
function setWallpaperImg() {
	var metadb = fb.GetNowPlaying();
	if (!metadb || !ppt.showwallpaper) return null;

	var tmp = null

	if (ppt.wallpapermode == 0) {
		tmp = utils.GetAlbumArtV2(metadb, 0);
	} else {
		tmp = gdi.Image(fb.ProfilePath + ppt.wallpaperpath);
	}

	return FormatWallpaper(tmp);;
}

function FormatWallpaper(img) {
	if (!img)
		return img;

	var tmp_img = gdi.CreateImage(ww, wh);
	var gp = tmp_img.GetGraphics();
	gp.SetInterpolationMode(7);
	drawImage(gp, img, 0, 0, ww, wh, 1);
	tmp_img.ReleaseGraphics(gp);

	// blur it!
	if (ppt.wallpaperblurred) {
		var blur_factor = ppt.wallpaperblurvalue; // [1-90]
		tmp_img = draw_blurred_image(tmp_img, 0, 0, tmp_img.Width, tmp_img.Height, 0, 0, tmp_img.Width, tmp_img.Height, blur_factor, 0x00ffffff);
	}

	return tmp_img.CreateRawBitmap();
}

function draw_blurred_image(image, ix, iy, iw, ih, bx, by, bw, bh, blur_value, overlay_color) {
	var blurValue = blur_value;
	try {
		var imgA = image.Resize(iw * blurValue / 100, ih * blurValue / 100, 2);
		var imgB = imgA.Resize(iw, ih, 2);
	} catch (e) {
		return null;
	}

	var bbox = gdi.CreateImage(bw, bh);
	// Get graphics interface like "gr" in on_paint
	var gb = bbox.GetGraphics();
	var offset = 90 - blurValue;
	gb.DrawImage(imgB, 0 - offset, 0 - (ih - bh) - offset, iw + offset * 2, ih + offset * 2, 0, 0, imgB.Width, imgB.Height, 0, 255);
	bbox.ReleaseGraphics(gb);

	var newImg = gdi.CreateImage(iw, ih);
	var gb = newImg.GetGraphics();

	if (ix != bx || iy != by || iw != bw || ih != bh) {
		gb.DrawImage(image, ix, iy, iw, ih, 0, 0, image.Width, image.Height, 0, 255);
		gb.FillSolidRect(bx, by, bw, bh, 0xffffffff);
	}
	gb.DrawImage(bbox, bx, by, bw, bh, 0, 0, bbox.Width, bbox.Height, 0, 255);

	// overlay
	if (overlay_color != null) {
		gb.FillSolidRect(bx, by, bw, bh, overlay_color);
	}

	// top border of blur area
	if (ix != bx || iy != by || iw != bw || ih != bh) {
		gb.FillSolidRect(bx, by, bw, 1, 0x22ffffff);
		gb.FillSolidRect(bx, by - 1, bw, 1, 0x22000000);
	}
	newImg.ReleaseGraphics(gb);

	return newImg;
}

//=================================================// Custom functions
function match(input, str) {
	var temp = "";
	input = input.toLowerCase();
	for (var j in str) {
		if (input.indexOf(str[j]) < 0)
			return false;
	}
	return true;
}

function process_string(str) {
	str_ = [];
	str = str.toLowerCase();
	while (str != (temp = str.replace("  ", " ")))
		str = temp;
	var str = str.split(" ").sort();
	for (var i in str) {
		if (str[i] != "")
			str_[str_.length] = str[i];
	}
	return str_;
}

// Used with plman.GetPlaylistLockFilterMask()
var PlaylistLockFilterMask = {
	filter_add : 1,
	filter_remove: 2,
	filter_reorder: 4,
	filter_replace: 8,
	filter_rename: 16,
	filter_remove_playlist: 32,
	filter_default_action: 64
}

function playlist_can_add_items(playlistIndex) {
	return !(plman.GetPlaylistLockFilterMask(playlistIndex) & PlaylistLockFilterMask.filter_add);
}

function playlist_can_remove_items(playlistIndex) {
	return !(plman.GetPlaylistLockFilterMask(playlistIndex) & PlaylistLockFilterMask.filter_remove);
}

function playlist_can_rename(playlistIndex) {
	return !(plman.GetPlaylistLockFilterMask(playlistIndex) & PlaylistLockFilterMask.filter_rename);
}

function playlist_can_remove(playlistIndex) {
	return !(plman.GetPlaylistLockFilterMask(playlistIndex) & PlaylistLockFilterMask.filter_remove_playlist);
}

function get_font() {
	var default_font;

	if (g_instancetype == 0) {
		default_font = window.GetFontCUI(FontTypeCUI.items);
		g_font_headers = window.GetFontCUI(FontTypeCUI.labels);
	} else if (g_instancetype == 1) {
		default_font = window.GetFontDUI(FontTypeDUI.playlists);
		g_font_headers = window.GetFontDUI(FontTypeDUI.tabs);
	}

	if (default_font) {
		g_fname = default_font.Name;
		g_fsize = default_font.Size;
		g_fstyle = default_font.Style;
	} else {
		console.log(window.Name, ": Unable to use the default font. Using Segoe UI instead.");
		g_fname = "Segoe UI";
		g_fsize = 12;
		g_fstyle = 0;
	}

	// adjust font size if extra zoom activated
	g_fsize += ppt.extra_font_size;
	g_font = gdi.Font(g_fname, g_fsize, 0);
	g_font_bold = gdi.Font(g_fname, g_fsize, 1);
	g_font_box = gdi.Font(g_fname, g_fsize - 2, 1);

	g_zoom_percent = Math.floor(g_fsize / 12 * 100);

	g_font_group1 = gdi.Font(g_fname, (g_fsize * 160 / 100), 1);
	g_font_group2 = gdi.Font(g_fname, (g_fsize * 140 / 100), 0);

	if (g_font_guifx_found) {
		g_font_rating = gdi.Font("guifx v2 transports", (g_fsize * 140 / 100), 0);
		g_font_mood = gdi.Font("guifx v2 transports", (g_fsize * 130 / 100), 0);
	} else if (g_font_wingdings2_found) {
		g_font_rating = gdi.Font("wingdings 2", (g_fsize * 140 / 100), 0);
		g_font_mood = gdi.Font("wingdings 2", (g_fsize * 200 / 100), 0);
	} else {
		g_font_rating = gdi.Font("arial", (g_fsize * 140 / 100), 0);
		g_font_mood = gdi.Font("arial", (g_fsize * 140 / 100), 0);
	}
}

function get_colors() {
	var arr;
	// get some system colors
	g_syscolor_window_bg = utils.GetSysColour(COLOR_WINDOW);
	g_syscolor_highlight = utils.GetSysColour(COLOR_HIGHLIGHT);
	g_syscolor_button_bg = utils.GetSysColour(COLOR_BTNFACE);
	g_syscolor_button_txt = utils.GetSysColour(COLOR_BTNTEXT);

	arr = window.GetProperty("CUSTOM COLOR TEXT NORMAL", "180-180-180").split("-");
	g_color_normal_txt = RGB(arr[0], arr[1], arr[2]);
	arr = window.GetProperty("CUSTOM COLOR TEXT SELECTED", "000-000-000").split("-");
	g_color_selected_txt = RGB(arr[0], arr[1], arr[2]);
	arr = window.GetProperty("CUSTOM COLOR BACKGROUND NORMAL", "025-025-035").split("-");
	g_color_normal_bg = RGB(arr[0], arr[1], arr[2]);
	arr = window.GetProperty("CUSTOM COLOR BACKGROUND SELECTED", "015-177-255").split("-");
	g_color_selected_bg = RGB(arr[0], arr[1], arr[2]);
	arr = window.GetProperty("CUSTOM COLOR HIGHLIGHT", "255-175-050").split("-");
	g_color_highlight = RGB(arr[0], arr[1], arr[2]);

	// get custom colors from window ppt first
	if (!ppt.enableCustomColors) {
		// get UI colors set in UI Preferences if no custom color set
		if (g_instancetype == 0) {
			g_color_normal_txt = window.GetColourCUI(ColorTypeCUI.text);
			g_color_selected_txt = window.GetColourCUI(ColorTypeCUI.selection_text);
			g_color_normal_bg = window.GetColourCUI(ColorTypeCUI.background);
			g_color_selected_bg = window.GetColourCUI(ColorTypeCUI.selection_background);
			g_color_highlight = window.GetColourCUI(ColorTypeCUI.active_item_frame);
		} else if (g_instancetype == 1) {
			g_color_normal_txt = window.GetColourDUI(ColorTypeDUI.text);
			g_color_selected_txt = window.GetColourDUI(ColorTypeDUI.selection);
			g_color_normal_bg = window.GetColourDUI(ColorTypeDUI.background);
			g_color_selected_bg = g_color_selected_txt;
			g_color_highlight = window.GetColourDUI(ColorTypeDUI.highlight);
		}
	}
}

function on_font_changed() {
	get_font();
	get_metrics();
	brw.repaint();
}

function on_colours_changed() {
	get_colors();
	get_images();
	if (brw)
		brw.scrollbar.setNewColors();
	g_filterbox.getImages();
	g_filterbox.reset_colors();
	brw.repaint();
}
