module system/lib/temporal

go 1.24

require (
	system/lib/calendar v0.0.0
	system/lib/planner v0.0.0
	system/lib/sessiontime v0.0.0
)

require (
	github.com/BurntSushi/toml v1.5.0 // indirect
	system/lib/config v0.0.0 // indirect
	system/lib/paths v0.0.0 // indirect
)

replace system/lib/calendar => ../calendar

replace system/lib/config => ../config

replace system/lib/paths => ../paths

replace system/lib/planner => ../planner

replace system/lib/sessiontime => ../sessiontime
