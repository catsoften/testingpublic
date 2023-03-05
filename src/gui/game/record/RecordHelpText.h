#pragma once
#include "Config.h"

const char* const recordHelpTextData =
	"\bo"		"Format:"
	"\n\bw\t"	"Output image format. WebP is much slower at processing frames with long recordings but is lossless. Gif framerate may be incorrect due to precision limitations (10ms increments). Old disables all settings except FPS."

	"\n\bo"		"FPS (Max 60):"
	"\n\bw\t"	"Framerate of final recording, dropping frames if necessary. Not affected by lag, but assumes game should be running at 60fps (Ignores tpt.setfpscap)."

	"\n\bo"		"Compression Strength (WebP only):"
	"\n\bw\t"	"Amount of effort put into compression. Larger values result in smaller files at the cost of write time. Max value (M) enables additional size reduction. "
	"\n\bu\t"	"Note: WebP images are always stored with lossless compression."

	"\n\bo"			"Select Area:"
	"\n\bw\t"	"Select the area of the screen to record (inclusive)."

	"\n\bo"		"Fullscreen:"
	"\n\bw\t"	"Records the entire window, including the UI. Overrides area selection."

	"\n\bo"		"Include UI:"
	"\n\bw\t"	"Includes the HUD in recordings."

	"\n\bo"		"Scale:"
	"\n\bl\t"	"*** Slows write performance with large areas ***"
	"\n\bw\t"	"Duplicates pixels to make recording larger. Useful if blurry when being upscaled. \"8+Spacing\" adds black areas between pixels (like zoom window)."

	"\n\bo"		"Buffer:"
	"\n\bw\t"	"Stores image data before processing to improve performance while recording. Final image is created after recording stops. Ram stores data in memory and is fast, but may use large amounts of ram. Disk stores data in the recordings folder temporarily. Size depends on area."

	"\n\bo"		"Buffer Usage/Limit:"
	"\n\bl\t"	"*** Limiter does not account for memory usage of other programs ***"
	"\n\bw\t"	"Usage shows amount of memory or disk space used for each second of recording with the current settings. Limiter will automatically stop the recording if the buffer is about to exceed this size."

	"\n\bo"		"Write Thread:"
	"\n\bw\t"	"Start processing frames on a separate thread immediately. Can reduce write time with similar game performance if multiple cores are available."

	"\n\bo"		"Reset:"
	"\n\bw\t"	"Reset all settings (including record area) to defaults."

	"\n\bo"		"Pause:"
	"\n\bw\t"	"Temporarily stops recording."

	"\n\bo"		"\uE06B:"
	"\n\bw\t"	"Bagel."
	;
