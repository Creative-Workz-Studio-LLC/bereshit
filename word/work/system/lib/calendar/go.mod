module system/lib/calendar

go 1.24

require (
	system/lib/jsonc v0.0.0
	system/lib/paths v0.0.0
)

require github.com/BurntSushi/toml v1.5.0 // indirect

replace (
	system/lib/jsonc => ../jsonc
	system/lib/paths => ../paths
)
