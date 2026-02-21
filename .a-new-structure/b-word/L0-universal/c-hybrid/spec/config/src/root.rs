//! Thread-safe root directory guard — set once at startup, checked by all loaders.

//omni:code --rust -library
//omni:key B-L0-hybrid-config-root
//omni:version b-03.00

use std::path::PathBuf;
use std::sync::RwLock;

use crate::error::ConfigError;

/// Thread-safe root directory guard. Set once at startup, checked by all Load* functions.
pub(crate) struct RootGuard {
    name: String,
    path: RwLock<Option<PathBuf>>,
}

impl RootGuard {
    pub(crate) fn new(name: &str) -> Self {
        Self {
            name: name.to_owned(),
            path: RwLock::new(None),
        }
    }

    pub(crate) fn set(&self, p: PathBuf) {
        let mut guard = self.path.write().unwrap_or_else(|e| e.into_inner());
        *guard = Some(p);
    }

    pub(crate) fn check(&self) -> Result<(), ConfigError> {
        let guard = self.path.read().unwrap_or_else(|e| e.into_inner());
        if guard.is_none() {
            return Err(ConfigError::RootNotSet {
                name: self.name.clone(),
            });
        }
        Ok(())
    }

    pub(crate) fn get(&self) -> Option<PathBuf> {
        let guard = self.path.read().unwrap_or_else(|e| e.into_inner());
        guard.clone()
    }

    pub(crate) fn clear(&self) {
        let mut guard = self.path.write().unwrap_or_else(|e| e.into_inner());
        *guard = None;
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use std::path::PathBuf;

    #[test]
    fn test_root_guard_lifecycle() {
        let guard = RootGuard::new("test");
        assert!(guard.check().is_err());
        assert!(guard.get().is_none());

        guard.set(PathBuf::from("/tmp/test"));
        assert!(guard.check().is_ok());
        assert_eq!(guard.get().unwrap(), PathBuf::from("/tmp/test"));

        guard.clear();
        assert!(guard.check().is_err());
        assert!(guard.get().is_none());
    }
}
