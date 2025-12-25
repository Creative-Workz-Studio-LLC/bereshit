module demo/config

go 1.24.4

replace system/lib/config => ../../../runtime/lib/config

require system/lib/config v0.0.0

require github.com/BurntSushi/toml v1.5.0 // indirect
