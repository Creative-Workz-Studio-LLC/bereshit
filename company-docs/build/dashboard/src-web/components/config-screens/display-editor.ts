/**
 * Display Editor — §5 build.config.yaml
 *
 * Banner toggle, colors toggle, icon mapping editor.
 */

import type { EditorDefinition } from './editor-base.js';
import { configBool, configKeyValue, configSection } from './editor-base.js';

interface DisplayData {
  banner?: boolean;
  colors?: boolean;
  icons?: Record<string, string>;
}

export const displayEditor: EditorDefinition = {
  sectionKey: 'display',
  title: 'Display',
  description: 'Banner, colors, and icon configuration for CLI output.',

  buildForm(data: unknown) {
    const d = (data ?? {}) as DisplayData;
    const el = document.createElement('div');

    const banner = configBool('Show banner on startup', d.banner ?? true);
    const colors = configBool('Enable colored output', d.colors ?? true);
    el.appendChild(banner.wrapper);
    el.appendChild(colors.wrapper);

    el.appendChild(configSection('Icons'));
    const icons = configKeyValue('Status Icons', d.icons ?? {});
    el.appendChild(icons.wrapper);

    return {
      element: el,
      getData: () => ({
        banner: banner.getValue(),
        colors: colors.getValue(),
        icons: icons.getValue(),
      }),
    };
  },
};
