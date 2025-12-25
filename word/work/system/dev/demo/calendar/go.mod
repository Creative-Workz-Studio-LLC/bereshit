module calendar-demo

go 1.24

require (
	system/lib/calendar v0.0.0
	system/lib/paths v0.0.0
	github.com/BurntSushi/toml v1.4.0
)

replace system/lib/calendar => ../../lib/calendar
replace system/lib/paths => ../../lib/paths
