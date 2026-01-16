<div align="center">

# 📋 Operations Reference

**Complete catalog of safe and protected operations**

![Bash](https://img.shields.io/badge/Bash-4EAA25?style=flat&logo=gnubash&logoColor=white)
![Linux](https://img.shields.io/badge/Linux-FCC624?style=flat&logo=linux&logoColor=black)

*CPI-SI Interactive Terminal System*

[Safe Operations](#safe-operations) • [Protected Operations](#protected-operations) • [Workflows](#common-workflows)

</div>

---

## Table of Contents

- [📋 Operations Reference](#-operations-reference)
  - [Table of Contents](#table-of-contents)
  - [Overview](#overview)
    - [Safe Operation Categories](#safe-operation-categories)
  - [Safe Operations](#safe-operations)
    - [Package Management](#package-management)
    - [Service Management](#service-management)
    - [File Operations](#file-operations)
    - [Network Configuration](#network-configuration)
    - [Docker Operations](#docker-operations)
    - [Process Management](#process-management)
    - [System File Editing](#system-file-editing)
    - [Log File Access](#log-file-access)
    - [Kernel Module Management](#kernel-module-management)
    - [Sudoers Configuration Testing](#sudoers-configuration-testing)
    - [PolicyKit Integration](#policykit-integration)
    - [System Information](#system-information)
  - [Protected Operations](#protected-operations)
    - [Essential Package Removal](#essential-package-removal)
    - [Filesystem Operations](#filesystem-operations)
    - [Bootloader Modifications](#bootloader-modifications)
  - [Common Workflows](#common-workflows)
    - [Installing Development Tools](#installing-development-tools)
    - [Service Management (Workflow)](#service-management-workflow)
    - [Docker Workflow](#docker-workflow)
    - [Project Setup](#project-setup)
  - [Operations Testing](#operations-testing)
    - [Test Commands](#test-commands)
    - [Health Scoring](#health-scoring)
    - [Viewing Test Results](#viewing-test-results)
    - [Custom Operation Tests](#custom-operation-tests)
    - [Automated Testing](#automated-testing)
  - [Safety Guidelines](#safety-guidelines)

---

## Overview

**What this is:** Complete catalog of every operation you can run - which ones work without password (safe) and which ones require password (protected).

**Quick guide:**

| I want to... | Password needed? | Where to look |
|--------------|------------------|---------------|
| Install packages, update system | No ✓ | [Package Management](#package-management) |
| Start/stop services | No ✓ | [Service Management](#service-management) |
| Use Docker containers | No ✓ | [Docker Operations](#docker-operations) |
| Edit system config files | No ✓ | [System File Editing](#system-file-editing) |
| Remove essential packages | Yes 🔒 | [Protected Operations](#protected-operations) |
| Format drives, modify partitions | Yes 🔒 | [Filesystem Operations](#filesystem-operations) |

**The rule:** Safe development work = no password. Dangerous system changes = password required.

### Safe Operation Categories

The system grants passwordless sudo access to the following operation categories:

| Category | Purpose | Example Operations |
|----------|---------|-------------------|
| Package Management | Install, upgrade, manage packages | `apt install`, `apt upgrade`, `dpkg -i` |
| Service Management | Start, stop, restart services | `systemctl start`, `systemctl enable` |
| File Operations | Modify permissions, ownership | `chmod`, `chown`, `chgrp` |
| Network Configuration | Network service management | `systemctl restart NetworkManager` |
| Docker Operations | Container and image management | `docker ps`, `docker build`, `docker-compose up` |
| Process Management | Kill processes | `kill`, `killall`, `pkill` |
| System File Editing | Edit system configs | `nano /etc/nginx/nginx.conf` |
| Log File Access | Read system logs | `tail /var/log/syslog`, `grep /var/log/` |
| Kernel Modules | Load/unload drivers | `modprobe`, `rmmod`, `lsmod` |
| Sudoers Testing | Validate sudoers syntax | `visudo -c` |
| PolicyKit Integration | GUI password prompts | `pkexec` (respects all sudoers rules) |
| System Information | View system state | `journalctl`, `dmesg`, `systemctl list-units` |

All operations include health tracking and logging for system monitoring.

---

## Safe Operations

### Package Management

**Update package lists:**

```bash
sudo apt update
```

- Updates available package information
- Safe operation - read-only
- No system changes

**Upgrade installed packages:**

```bash
sudo apt upgrade
sudo apt upgrade -y                    # Non-interactive
```

- Upgrades packages to latest versions
- Respects configuration in DEBIAN_FRONTEND
- Safe - doesn't remove essential packages

**Install new packages:**

```bash
sudo apt install <package-name>
sudo apt install lua5.4 luajit         # Multiple packages
sudo apt install -y python3-pip        # Non-interactive
```

- Installs any package from repositories
- Handles dependencies automatically
- Environment vars prevent prompts

**Install from .deb file:**

```bash
sudo dpkg -i package.deb
```

- Install manually downloaded packages
- Useful for packages not in repositories

**Clean up unused packages:**

```bash
sudo apt autoremove
sudo apt autoremove -y
```

- Removes automatically installed dependencies no longer needed
- Safe - essential packages protected

**Search for packages:**

```bash
apt search <search-term>               # No sudo needed
apt show <package-name>                # No sudo needed
```

### Service Management

**Start a service:**

```bash
sudo systemctl start <service-name>
sudo systemctl start postgresql
sudo systemctl start nginx
```

- Starts a stopped service
- Immediate effect
- Safe for non-critical services

**Stop a service:**

```bash
sudo systemctl stop <service-name>
sudo systemctl stop postgresql
```

- Stops a running service
- Immediate effect

**Restart a service:**

```bash
sudo systemctl restart <service-name>
sudo systemctl restart nginx
```

- Stops and starts service
- Reloads configuration
- Common after config changes

**Enable service on boot:**

```bash
sudo systemctl enable <service-name>
sudo systemctl enable docker
```

- Service starts automatically on boot
- Doesn't start immediately (use start for that)

**Disable service from boot:**

```bash
sudo systemctl disable <service-name>
```

- Service won't start on boot
- Doesn't stop running service
- **Note:** Disabling critical systemd services requires password

**Check service status:**

```bash
sudo systemctl status <service-name>
systemctl status ssh --no-pager       # No sudo needed for status
```

- Shows service state, recent logs
- Useful for troubleshooting

### File Operations

**Change file permissions:**

```bash
sudo chmod +x script.sh                # Make executable
sudo chmod 755 directory               # Full permissions
sudo chmod -R 644 files/               # Recursive
```

- Modify file/directory permissions
- Use octal (755) or symbolic (+x) notation
- **Note:** Critical system directories require password

**Change file ownership:**

```bash
sudo chown user:group file
sudo chown $USER:$USER project/
sudo chown -R user:group directory/
```

- Change file owner and group
- Useful for fixing permission issues
- **Note:** Critical system files require password

### Network Configuration

**Restart network services:**

```bash
sudo systemctl restart NetworkManager
sudo systemctl restart systemd-resolved
```

- Apply network configuration changes
- Refresh DNS settings

**Flush DNS cache:**

```bash
sudo systemd-resolve --flush-caches
```

- Clear DNS cache
- Useful for DNS troubleshooting

### Docker Operations

**Container management:**

```bash
sudo docker ps                         # List running containers
sudo docker ps -a                      # List all containers
sudo docker start <container>          # Start container
sudo docker stop <container>           # Stop container
sudo docker restart <container>        # Restart container
sudo docker rm <container>             # Remove container
```

- Full container lifecycle management
- Safe operations - containers are isolated
- No impact on host system

**Image management:**

```bash
sudo docker images                     # List images
sudo docker pull <image>               # Pull image from registry
sudo docker build -t <tag> .          # Build image from Dockerfile
sudo docker rmi <image>                # Remove image
sudo docker prune -a                   # Clean up unused images
```

- Manage container images
- Build from Dockerfiles
- Clean up disk space

**Docker Compose:**

```bash
sudo docker-compose up                 # Start services
sudo docker-compose up -d              # Start in background
sudo docker-compose down               # Stop services
sudo docker-compose logs               # View logs
sudo docker-compose ps                 # List services
```

- Multi-container orchestration
- Development environment management
- Both `/usr/bin/docker-compose` and `/usr/local/bin/docker-compose` supported

**Networking and volumes:**

```bash
sudo docker network ls                 # List networks
sudo docker network create <name>      # Create network
sudo docker volume ls                  # List volumes
sudo docker volume create <name>       # Create volume
```

- Manage Docker networking
- Persistent data storage

### Process Management

**Terminate processes by PID:**

```bash
sudo kill <pid>                        # Send SIGTERM (graceful)
sudo kill -9 <pid>                     # Send SIGKILL (force)
sudo kill -HUP <pid>                   # Send SIGHUP (reload config)
```

- Terminate specific process by ID
- Graceful or forceful termination
- Useful for stuck processes

**Terminate processes by name:**

```bash
sudo killall <process-name>            # Kill all instances
sudo killall nginx                     # Kill all nginx processes
sudo pkill -f pattern                  # Kill by pattern match
```

- Terminate processes by name
- Multiple instances at once
- Pattern matching support

**Common use cases:**

```bash
# Force kill stuck development server
sudo killall -9 node

# Gracefully reload nginx
sudo kill -HUP $(cat /var/run/nginx.pid)

# Kill processes matching pattern
sudo pkill -f "python.*test_server"
```

### System File Editing

**Edit configuration files:**

```bash
sudo nano /etc/nginx/nginx.conf
sudo vim /etc/systemd/system/myapp.service
sudo vi /etc/environment
```

- Direct editing of system configuration
- Supports nano, vim, vi editors
- Any file under `/etc/`

**Common configuration files:**

```bash
# Web server configuration
sudo nano /etc/nginx/sites-available/mysite

# Service configuration
sudo vim /etc/systemd/system/myapp.service

# Environment variables
sudo nano /etc/environment

# Network configuration
sudo vim /etc/netplan/01-netcfg.yaml

# DNS configuration
sudo nano /etc/resolv.conf
```

**Best practices:**

- Always backup before editing: `sudo cp /etc/file /etc/file.backup`
- Validate syntax after changes (e.g., `sudo nginx -t`)
- Reload services after configuration changes

### Log File Access

**View system logs:**

```bash
sudo tail /var/log/syslog              # View recent system logs
sudo tail -f /var/log/syslog           # Follow log in real-time
sudo tail -n 100 /var/log/syslog       # Last 100 lines
```

- Monitor system activity
- Real-time log following
- Useful for debugging

**Application logs:**

```bash
sudo cat /var/log/nginx/access.log
sudo cat /var/log/nginx/error.log
sudo less /var/log/postgresql/postgresql-14-main.log
```

- Read application-specific logs
- Troubleshoot service issues
- Access historical data

**Search logs:**

```bash
sudo grep "error" /var/log/syslog
sudo grep -i "failed" /var/log/auth.log
sudo grep -r "Exception" /var/log/
```

- Search for specific patterns
- Case-insensitive matching
- Recursive directory search

**Common log files:**

```bash
/var/log/syslog                       # System messages
/var/log/auth.log                     # Authentication logs
/var/log/kern.log                     # Kernel messages
/var/log/dmesg                        # Boot messages
/var/log/apache2/                     # Apache logs
/var/log/nginx/                       # Nginx logs
/var/log/postgresql/                  # PostgreSQL logs
```

### Kernel Module Management

**List loaded modules:**

```bash
sudo lsmod                             # List all loaded modules
sudo lsmod | grep nvidia               # Search for specific module
```

- View currently loaded kernel modules
- Check module status
- No password required for read-only operation

**Load kernel module:**

```bash
sudo modprobe <module-name>            # Load module
sudo modprobe nvidia                   # Load Nvidia driver
sudo modprobe -r <module>              # Remove module
```

- Load kernel modules dynamically
- Useful for driver management
- Hardware support

**Unload kernel module:**

```bash
sudo rmmod <module-name>               # Unload module
sudo rmmod nvidia_drm                  # Remove specific module
```

- Unload kernel modules
- Useful for driver updates
- Requires no dependencies on module

**Common use cases:**

```bash
# Load Nvidia drivers
sudo modprobe nvidia

# Load VirtualBox modules
sudo modprobe vboxdrv

# Check if module is loaded
lsmod | grep <module>

# Get module information
modinfo <module-name>
```

**Safety notes:**

- Removing critical modules can cause system instability
- Some modules may be required by other modules
- Use `lsmod` to check dependencies before removing

### Sudoers Configuration Testing

**Validate sudoers syntax:**

```bash
sudo visudo -c                         # Check default sudoers file
sudo visudo -c -f /etc/sudoers.d/90-cpi-si-safe-operations
```

- Critical safety operation
- Validates syntax before deployment
- Prevents sudo lockout from syntax errors

**Why this is passwordless:**

- Testing configuration files is safe - no changes made
- Allows automated validation in scripts
- Used by installation scripts for safety verification
- Read-only operation

**Usage in workflows:**

```bash
# Validate before copying to /etc/sudoers.d/
if sudo visudo -c -f ./90-cpi-si-safe-operations; then
    echo "✓ Syntax valid"
    sudo cp ./90-cpi-si-safe-operations /etc/sudoers.d/
else
    echo "✗ Syntax error - not installing"
    exit 1
fi
```

**Integration with system:**

- Used by `scripts/sudoers/install.sh` (Health Action 2/6, 30 points)
- Prevents installation of invalid configuration
- Part of health tracking system

### PolicyKit Integration

**GUI password prompts via pkexec:**

```bash
pkexec <command>                       # Shows GUI password dialog
pkexec ./install.sh                    # GUI prompt for installation
pkexec apt install package             # GUI prompt for package install
```

- Native desktop environment integration
- Graphical password dialog using PolicyKit
- Alternative to terminal `sudo` prompts
- Respects all sudoers rules defined above

**Why this is passwordless:**

- `pkexec` itself requires no password to invoke
- Password prompt appears in GUI dialog (not terminal)
- PolicyKit respects sudoers NOPASSWD rules
- Safe operations work without password (same as sudo)
- Protected operations still require password (safety boundaries)

**Usage patterns:**

```bash
# GUI password prompt (desktop workflow)
pkexec ./scripts/sudoers/install.sh

# Terminal password prompt (SSH/script workflow)
sudo ./scripts/sudoers/install.sh

# Both work identically - only difference is where password is entered
```

**When to use:**

- Desktop environment: Use `pkexec` for GUI password dialogs
- SSH session: Use `sudo` for terminal prompts
- Automated scripts: Use `sudo` (no GUI available)
- Mixed workflows: Both methods work with same permissions

**See also:** [GUI Password Prompts Documentation](gui-sudo-prompts.md)

### System Information

**List systemd units:**

```bash
sudo systemctl list-units              # All active units
sudo systemctl list-units --all        # All units including inactive
sudo systemctl list-unit-files         # All unit files
```

- View all systemd services
- Check service states
- Discover available services

**View system journal:**

```bash
sudo journalctl                        # All journal entries
sudo journalctl -u nginx               # Specific service
sudo journalctl -f                     # Follow journal
sudo journalctl --since today          # Today's entries
sudo journalctl -b                     # Current boot
```

- Systemd journal access
- Service-specific logs
- Real-time monitoring
- Time-based filtering

**View kernel messages:**

```bash
sudo dmesg                             # Kernel ring buffer
sudo dmesg | grep -i error             # Search for errors
sudo dmesg -w                          # Watch for new messages
```

- Hardware detection messages
- Kernel warnings and errors
- Boot sequence information

**Why passwordless:**

- Read-only operations
- Essential for debugging
- No system modifications
- Safe for automated monitoring

---

## Protected Operations

These operations require password authentication for safety.

### Essential Package Removal

**Attempting to remove critical packages:**

```bash
sudo apt remove systemd                # BLOCKED - requires password
sudo apt remove linux-image-*          # BLOCKED - kernel removal
sudo apt remove grub*                  # BLOCKED - bootloader removal
sudo apt remove network-manager        # BLOCKED - network critical
```

**Why protected:**

- Removing these packages can brick the system
- Require explicit manual confirmation
- Safety boundary prevents accidental removal

### Filesystem Operations

**Format filesystem:**

```bash
sudo mkfs.ext4 /dev/sdX                # BLOCKED - requires password
sudo mkfs.* /dev/*                     # All mkfs variants protected
```

**Partition management:**

```bash
sudo fdisk /dev/sdX                    # BLOCKED - requires password
sudo parted /dev/sdX                   # BLOCKED - requires password
```

**Low-level disk operations:**

```bash
sudo dd if=/dev/zero of=/dev/sdX       # BLOCKED - requires password
```

**Why protected:**

- Can destroy data or make system unbootable
- Irreversible operations
- Require explicit manual confirmation

### Bootloader Modifications

**GRUB operations:**

```bash
sudo grub-install /dev/sdX             # BLOCKED - requires password
sudo update-grub                       # BLOCKED - requires password
```

**Why protected:**

- Can make system unbootable
- Critical to boot process
- Mistakes require recovery media to fix

---

## Common Workflows

**Real-world examples:** Step-by-step workflows for common development tasks.

<details>
<summary><b>Installing Development Tools</b></summary>

### Installing Development Tools

**Python development:**

```bash
# Install Python and pip
sudo apt install python3 python3-pip python3-venv

# Install packages with pip (no sudo)
pip install black pytest django
```

**Node.js development:**

```bash
# Install Node.js and npm
sudo apt install nodejs npm

# Install global packages
npm install -g typescript @types/node
```

**Rust development:**

```bash
# Rust installed via rustup (no sudo needed)
# But installing build dependencies:
sudo apt install build-essential

# Cargo operates without sudo
cargo install ripgrep bat
```

**Lua development:**

```bash
# Install Lua and LuaJIT
sudo apt install lua5.4 luajit

# Install LuaRocks (Lua package manager)
sudo apt install luarocks
```

</details>

<details>
<summary><b>Service Management Workflows</b></summary>

### Service Management (Workflow)

**Database setup:**

```bash
# Install PostgreSQL
sudo apt install postgresql postgresql-contrib

# Start service
sudo systemctl start postgresql

# Enable on boot
sudo systemctl enable postgresql

# Check status
systemctl status postgresql
```

**Web server setup:**

```bash
# Install nginx
sudo apt install nginx

# Start and enable
sudo systemctl start nginx
sudo systemctl enable nginx

# Restart after config changes
sudo systemctl restart nginx
```

</details>

<details>
<summary><b>Docker Workflows</b></summary>

### Docker Workflow

**Docker installation and setup:**

```bash
# Install Docker
sudo apt update
sudo apt install docker.io docker-compose

# Start and enable Docker service
sudo systemctl start docker
sudo systemctl enable docker

# Verify installation
sudo docker --version
sudo docker-compose --version
```

**Development container workflow:**

```bash
# Create Dockerfile
cat > Dockerfile << 'EOF'
FROM node:18-alpine
WORKDIR /app
COPY package*.json ./
RUN npm install
COPY . .
EXPOSE 3000
CMD ["npm", "start"]
EOF

# Build image
sudo docker build -t myapp:latest .

# Run container
sudo docker run -d -p 3000:3000 --name myapp myapp:latest

# View logs
sudo docker logs -f myapp

# Stop and remove
sudo docker stop myapp
sudo docker rm myapp
```

**Multi-container with Docker Compose:**

```bash
# Create docker-compose.yml
cat > docker-compose.yml << 'EOF'
version: '3.8'
services:
  web:
    build: .
    ports:
      - "3000:3000"
  db:
    image: postgres:14
    environment:
      POSTGRES_PASSWORD: password
EOF

# Start all services
sudo docker-compose up -d

# View status
sudo docker-compose ps

# Stop all services
sudo docker-compose down
```

**Container debugging:**

```bash
# Execute command in running container
sudo docker exec -it myapp sh

# View container logs
sudo docker logs myapp

# Inspect container
sudo docker inspect myapp

# View resource usage
sudo docker stats
```

</details>

<details>
<summary><b>Project Setup Workflows</b></summary>

### Project Setup

**New project initialization:**

```bash
# Create project directory
mkdir -p ~/projects/my-project
cd ~/projects/my-project

# Initialize git
git init

# Create virtual environment (Python)
python3 -m venv venv
source venv/bin/activate

# Install dependencies
pip install -r requirements.txt

# Set permissions
sudo chown -R $USER:$USER .
chmod +x scripts/*.sh
```

**Fixing permission issues:**

```bash
# Take ownership of project
sudo chown -R $USER:$USER ~/projects/my-project

# Make scripts executable
sudo chmod +x build.sh test.sh

# Set directory permissions
sudo chmod -R 755 bin/
```

</details>

---

## Operations Testing

The CPI-SI Interactive Terminal System includes comprehensive operations testing with health tracking.

### Test Commands

**Standard operation tests:**

```bash
# Test safe operation (should succeed without password)
system test safe-op "sudo apt update"

# Test protected operation (should require password or be denied)
system test protected-op "sudo apt remove systemd"

# Test safety boundary (should be blocked)
system test safety "sudo mkfs.ext4 /dev/sda1"
```

**Available test types:**

- `safe-op` - Verifies operation works without password prompt
- `protected-op` - Verifies operation correctly requires password
- `safety` - Verifies dangerous operation is blocked

### Health Scoring

Each operation test contributes to system health:

| Test Type | Success | Failure |
|-----------|---------|---------|
| Safe Operation | +10 | -10 |
| Protected Operation | +10 | -10 (safety breach if allowed) |
| Safety Boundary | +10 | -10 (critical safety breach) |

> [!NOTE]
> The system uses **TRUE SCORE** philosophy - actual impact values (not always rounded). Test operations use ±10 for simplicity, but production code uses specific values like +17, -73, +6 to create unique fingerprints for debugging. See [Architecture](./architecture.md#health-scoring) for details.

**Running all tests:**

```bash
# Execute standard test suite
system test
```

Tests validate:

- Package management operations
- Service control operations
- File operations
- Network configuration
- Docker operations
- Process management
- System file editing
- Log file access
- Kernel module operations

### Viewing Test Results

Test results are logged with health tracking:

```bash
# View operations test logs
cat ~/.claude/system/logs/libraries/operations.log

# Check recent test results
tail -n 50 ~/.claude/system/logs/libraries/operations.log
```

**Log entry format:**

```json
{
  "timestamp": "2025-10-25T02:31:27Z",
  "level": "CHECK",
  "component": "operations",
  "action": "test-safe-operation",
  "success": true,
  "health_delta": 10,
  "context": {
    "command": "sudo apt update",
    "test_type": "safe-op",
    "exit_code": 0
  }
}
```

### Custom Operation Tests

Create custom tests for specific operations:

```bash
# Test custom sudoers rule
system test safe-op "sudo visudo -c -f /etc/sudoers.d/my-config"

# Test Docker permissions
system test safe-op "sudo docker ps"

# Verify protection
system test protected-op "sudo rm -rf /boot"
```

### Automated Testing

Operations tests integrate with build process:

```bash
# Build script runs health checks
./build.sh

# Includes operations testing
# Validates sudoers configuration
# Checks environment setup
```

**Health tracking integration:**

- Each test logged with health delta
- Results contribute to overall system health
- Failed tests indicate configuration issues
- Success demonstrates proper safety boundaries

---

## Safety Guidelines

**Safe Practices:**

1. **Use specific package names** - Avoid wildcards in removals
2. **Check before installing** - `apt show <package>` to verify
3. **Test configuration** - Validate before restarting services
4. **Use version control** - Git track configuration changes
5. **Backup data** - Before major system changes
6. **Validate sudoers syntax** - Always use `sudo visudo -c -f <file>` before installing
7. **Test operations** - Use `system test` to verify operations work as expected
8. **Monitor logs** - Check health tracking logs for anomalies

**When to use password operations:**

- Removing packages you're unsure about
- Any filesystem/partition operations
- Bootloader modifications
- When explicitly warned by the system
- Anything not in the Safe Operations categories above

**Red flags (operations that should always require password):**

- Anything involving `/dev/sd*` or other block devices
- Commands containing `mkfs`, `fdisk`, `dd`
- Removing `systemd`, `grub`, or kernel packages
- Modifying bootloader
- Package removal (especially with wildcards like `linux-image-*`)

**Testing safely:**

```bash
# Use --dry-run or -s for simulation
apt install -s package-name           # Simulate install

# Check what would be removed
apt autoremove --dry-run

# Verify syntax before applying
sudo nginx -t                          # Test nginx config
sudo visudo -c -f config-file         # Validate sudoers syntax
sudo systemctl daemon-reload          # Reload systemd after changes

# Test operations with health tracking
system test safe-op "sudo apt update"
system test protected-op "sudo apt remove systemd"
```

**Using health tracking:**

The system automatically logs all operations with health scoring:

```bash
# View component health
system status

# Check operation logs
cat ~/.claude/system/logs/libraries/operations.log

# View recent health events
tail -n 50 ~/.claude/system/logs/libraries/*.log
```

**Health indicators:**

| Symbol | Health | Meaning |
|--------|--------|---------|
| 💚 | 90-100 | Excellent - all operations healthy |
| 💙 | 80-89 | Good - minor issues |
| 💛 | 70-79 | Fair - some concerns |
| 🧡 | 60-69 | Poor - attention needed |
| ❤️ | 50-59 | Critical - immediate attention |
| 🤍 | 40-49 | Severe - system degraded |
| 💔 | 30-39 | Failing - major issues |
| 🩹 | 20-29 | Emergency - critical failure |
| ⚠️ | 10-19 | Danger - near total failure |
| ☠️ | 1-9 | Dead - system non-functional |
| 💀 | 0 | Complete failure |

Monitor health indicators to catch issues early before they impact system functionality.

**Best practices for new operations:**

1. **Test first** - Use `system test safe-op` to verify operation works
2. **Check logs** - Review health tracking logs after testing
3. **Validate syntax** - For configuration files, always validate before applying
4. **Document changes** - Keep track of what operations were added and why
5. **Monitor health** - Watch for health degradation after changes

---

<div align="center">

**Built with safety and intentionality for Kingdom Technology**

*"In the beginning, God created the heavens and the earth." - Genesis 1:1*

[Back to System Documentation](../README.md)

</div>
