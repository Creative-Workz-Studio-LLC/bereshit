module system/lib/config

go 1.24

require (
	github.com/BurntSushi/toml v1.5.0
	system/lib/jsonc v0.0.0
)

replace system/lib/jsonc => ../jsonc
