module system/lib/validation

go 1.24

require (
	system/lib/display v0.0.0
	system/lib/jsonc v0.0.0
)

replace (
	system/lib/display => ../display
	system/lib/jsonc => ../jsonc
)
