powder_files += files(
	'BitmapBrush.cpp',
	'Brush.cpp',
	'DecorationTool.cpp',
	'Favorite.cpp',
	'GameController.cpp',
	'GameModel.cpp',
	'GameView.cpp',
	'GOLTool.cpp',
	'Menu.cpp',
	'PropertyTool.cpp',
	'SampleTool.cpp',
	'SignTool.cpp',
	'ToolButton.cpp',
	'Tool.cpp',
)

if get_option('touch_ui')
	powder_files += files('GameViewTouchUI.cpp')
else
	powder_files += files('GameViewBasic.cpp')
endif
