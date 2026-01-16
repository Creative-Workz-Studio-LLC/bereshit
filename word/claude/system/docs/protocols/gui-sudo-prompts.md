# GUI Password Prompts for Elevated Operations

**Quick Reference: Using graphical password prompts instead of terminal prompts**

---

## Overview

Your system supports **two methods** for password-protected operations:

| Method | Password Prompt | Best For |
|--------|----------------|----------|
| `pkexec` | GUI dialog (PolicyKit) | Desktop use, visual workflows |
| `sudo` | Terminal prompt | SSH, scripts, quick terminal work |

Both provide the same level of security and access - the only difference is where you type your password.

---

## Using GUI Password Prompts

### Basic Syntax

```bash
# GUI prompt
pkexec <command> [args]

# Terminal prompt (traditional)
sudo <command> [args]
```

### Common Examples

```bash
# Installing packages
pkexec apt install tree
sudo apt install tree    # same thing, terminal prompt

# Running scripts that need root
pkexec ./install.sh
sudo ./install.sh       # same thing, terminal prompt

# System service management
pkexec systemctl restart nginx
sudo systemctl restart nginx
```

---

## CPI-SI System Scripts

### Sudoers Installation

```bash
cd ~/.claude/system/scripts/sudoers

# GUI password prompt (recommended for desktop)
pkexec ./install.sh

# Terminal password prompt
sudo ./install.sh
```

### Future Go Commands

Once the Go commands are compiled, both methods work:

```bash
# GUI prompt
pkexec ~/.claude/system/bin/validate
pkexec ~/.claude/system/bin/diagnose

# Terminal prompt
sudo ~/.claude/system/bin/validate
sudo ~/.claude/system/bin/diagnose
```

---

## How It Works

### PolicyKit (pkexec)

- **Native desktop integration**: Uses your desktop environment's authentication
- **PolicyKit framework**: System-wide authentication policies
- **GUI dialog**: Clean graphical password prompt
- **Same security**: Uses the same PolicyKit policies as sudo

### Traditional sudo

- **Terminal-based**: Password typed directly in terminal
- **Sudoers configuration**: `/etc/sudoers` and `/etc/sudoers.d/`
- **Familiar workflow**: Traditional Unix/Linux approach
- **Script-friendly**: Works in SSH sessions and automated scripts

---

## SUDO_ASKPASS Configuration

Your shell is configured with `SUDO_ASKPASS` environment variable pointing to a GUI password helper:

```bash
# In ~/.bashrc and ~/.profile:
export SUDO_ASKPASS="$HOME/.local/bin/sudo-askpass"
```

**Note:** Your system uses `sudo-rs` (Rust implementation) which doesn't support the `-A` flag yet. When `sudo-rs` adds askpass support, you'll be able to use:

```bash
sudo -A <command>    # Force GUI prompt (not working yet with sudo-rs)
```

For now, use `pkexec` directly for GUI password prompts.

---

## When to Use Each Method

### Use `pkexec` (GUI) when:

- ✅ Working in desktop environment
- ✅ Visual workflow (clicking, dragging, GUI apps)
- ✅ Installing system software from GUI
- ✅ Running system configuration scripts
- ✅ Prefer graphical password entry

### Use `sudo` (terminal) when:

- ✅ Working in SSH session
- ✅ Quick terminal commands
- ✅ Automated scripts
- ✅ Piping commands
- ✅ Prefer terminal workflow

---

## Passwordless Operations

After installing the CPI-SI sudoers configuration, **safe operations don't need passwords at all** (with either method):

```bash
# No password needed (safe operations)
sudo apt update
sudo apt install python3-pip
sudo systemctl restart nginx
sudo chmod +x script.sh

pkexec apt update           # Same - no password needed
pkexec systemctl restart nginx
```

**Protected operations still require password** (safety boundaries):

```bash
# Password required (dangerous operations)
sudo apt remove systemd      # Bricks system
sudo mkfs.ext4 /dev/sda1     # Data destruction
pkexec apt remove systemd    # Same protection
```

See [Sudoers Configuration](../sudoers/README.md) for details on safe vs. protected operations.

---

## Troubleshooting

### pkexec command not found

```bash
# Install PolicyKit if missing
sudo apt install policykit-1
```

### GUI dialog doesn't appear

- **Check display:** Ensure `$DISPLAY` environment variable is set
- **Test manually:** `echo $DISPLAY` should show `:0` or similar
- **Try terminal:** Fall back to `sudo` if pkexec doesn't work

### Permission denied

- Both `pkexec` and `sudo` require you to be in the appropriate groups
- Check: `groups` should include `sudo` or `admin`
- Fix: `sudo usermod -aG sudo $USER` (then logout/login)

---

## Technical Details

### PolicyKit Authentication

`pkexec` uses PolicyKit (polkit) for authentication:

- **Policy files**: `/usr/share/polkit-1/actions/*.policy`
- **Admin group**: Users in `sudo` or `admin` group can authenticate
- **Session-based**: Authentication can be remembered for session
- **Audit logging**: All privileged operations are logged

### SUDO_ASKPASS Helper

The askpass helper (`~/.local/bin/sudo-askpass`) uses `zenity`:

```bash
#!/usr/bin/env bash
# Display password dialog
zenity --password \
    --title="Sudo Password Required" \
    --text="Enter your password to authorize this operation:" \
    --timeout=60
```

This is ready for when `sudo-rs` adds askpass support.

---

## Future Enhancements

### When sudo-rs adds `-A` flag support:

```bash
# Will work in the future
sudo -A apt install package    # Forces GUI prompt via SUDO_ASKPASS
```

### Integration with CPI-SI commands:

Go commands can detect if display is available and auto-select:

```go
func needsElevation() {
    if os.Getenv("DISPLAY") != "" {
        // GUI environment - use pkexec
        exec.Command("pkexec", "operation").Run()
    } else {
        // Terminal - use sudo
        exec.Command("sudo", "operation").Run()
    }
}
```

---

## Summary

**Quick Commands:**

```bash
# GUI password prompt
pkexec ./script.sh
pkexec apt install package

# Terminal password prompt
sudo ./script.sh
sudo apt install package

# Check which you prefer
echo "GUI: pkexec"
echo "Terminal: sudo"
echo "Both work identically!"
```

**Remember:** After CPI-SI sudoers installation, most development operations don't need passwords at all - the system handles it automatically within safety boundaries.

---

<div align="center">

**Kingdom Technology: Enabling good work freely while protecting against destruction**

[Back to System Documentation](../README.md) • [Sudoers Configuration](../sudoers/README.md)

</div>
