/**
 * Config Routes — Read/write build.config.yaml and dashboard preferences.
 */

import { Router } from 'express';
import { readFileSync, writeFileSync, existsSync, mkdirSync } from 'fs';
import { resolve, dirname } from 'path';
import { fileURLToPath } from 'url';
import YAML from 'yaml';

const __filename = fileURLToPath(import.meta.url);
const __dirname = dirname(__filename);

export const configRoutes = Router();

/** Resolve the config file path. */
function getConfigPath(): string {
  return resolve(__dirname, '..', '..', '..', '..', 'build', 'build.config.yaml');
}

/** Resolve the preferences file path. */
function getPreferencesPath(): string {
  const configHome = process.env.XDG_CONFIG_HOME
    ?? resolve(process.env.HOME ?? '.', '.config');
  return resolve(configHome, 'cws-dashboard', 'preferences.json');
}

// GET /api/config — Read full config as JSON.
configRoutes.get('/', (_req, res) => {
  try {
    const configPath = getConfigPath();
    if (!existsSync(configPath)) {
      return res.status(404).json({ error: 'Config not found', path: configPath });
    }
    const yamlStr = readFileSync(configPath, 'utf-8');
    const data = YAML.parse(yamlStr);
    res.json(data);
  } catch (err) {
    res.status(500).json({ error: String(err) });
  }
});

// PATCH /api/config/:section — Update one section.
configRoutes.patch('/:section', (req, res) => {
  try {
    const { section } = req.params;
    const sectionData = req.body;
    const configPath = getConfigPath();

    const yamlStr = readFileSync(configPath, 'utf-8');
    const doc = YAML.parseDocument(yamlStr);

    // Set the section in the YAML document (preserves other sections).
    doc.set(section!, doc.createNode(sectionData));

    writeFileSync(configPath, doc.toString(), 'utf-8');
    res.json({ status: 'updated', section });
  } catch (err) {
    res.status(500).json({ error: String(err) });
  }
});

// GET /api/config/preferences — Read dashboard preferences.
configRoutes.get('/preferences', (_req, res) => {
  try {
    const prefsPath = getPreferencesPath();
    if (!existsSync(prefsPath)) {
      return res.json({});
    }
    const data = JSON.parse(readFileSync(prefsPath, 'utf-8'));
    res.json(data);
  } catch (err) {
    res.status(500).json({ error: String(err) });
  }
});

// PUT /api/config/preferences — Write dashboard preferences.
configRoutes.put('/preferences', (req, res) => {
  try {
    const prefsPath = getPreferencesPath();
    const dir = dirname(prefsPath);

    if (!existsSync(dir)) {
      mkdirSync(dir, { recursive: true });
    }

    writeFileSync(prefsPath, JSON.stringify(req.body, null, 2), 'utf-8');
    res.json({ status: 'saved' });
  } catch (err) {
    res.status(500).json({ error: String(err) });
  }
});
